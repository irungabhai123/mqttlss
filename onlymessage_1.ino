#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Main Server";         // Wi-Fi network name
const char* password = "admin@123";      // Wi-Fi password

// MQTT broker details
const char* mqtt_server = "5.196.78.28"; // MQTT broker IP
const char* mqtt_user = "";               // MQTT username (if any)
const char* mqtt_password = "";           // MQTT password (if any)

// MQTT topics
const char* publish_topic = "device/status";   // Topic to publish messages
const char* subscribe_topic = "sensor/data";  // Topic to subscribe

// Create WiFi and MQTT clients
WiFiClient gigaClient;
PubSubClient client(gigaClient);

// Function prototypes
void callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT();

void setup() {
  // Start the serial monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Waiting for WiFi...");
  }
  Serial.println("WiFi connected");

  // Setup MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Connect to MQTT broker
  reconnectMQTT();

  // Publish an initial message
  if (client.connected()) {
    client.publish(publish_topic, "Device is online");
  }
}

void loop() {
  // Ensure MQTT connection stays alive
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Periodically send a message
  static unsigned long lastMessageTime = 0;
  if (millis() - lastMessageTime > 5000) { // Every 5 seconds
    lastMessageTime = millis();
    client.publish(publish_topic, "Hello from Arduino GIGA!");
    Serial.println("Message published: Hello from Arduino GIGA!");
  }
}

// Callback function to handle messages from subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Optionally respond to a received message
  if (strcmp(topic, subscribe_topic) == 0) {
    client.publish(publish_topic, "Message received!");
  }
}

// Function to connect/reconnect to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ArduinoGIGAClient", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT");
      // Subscribe to topics
      client.subscribe(subscribe_topic);

      // Publish a connection status message
      client.publish(publish_topic, "Reconnected to MQTT");
    } else {
      Serial.print("Failed to connect. MQTT state: ");
      Serial.println(client.state());
      Serial.println("Retrying in 2 seconds...");
      delay(2000);
    }
  }
}
