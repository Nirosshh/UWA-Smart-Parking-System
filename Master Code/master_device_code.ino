#include <WiFi.h>  // Library for Wi-Fi connectivity
#include <PubSubClient.h>  // MQTT client library
#include <SPI.h>  // SPI interface for LoRa
#include <LoRa.h>  // LoRa communication library

// LoRa pins (for TTGO T-Beam)
#define SS    18  // LoRa chip select pin
#define RST   14  // LoRa reset pin
#define DIO0  26  // LoRa DIO0 interrupt pin

int Slave_No = 2;  // Number of slave devices expected

// Wi-Fi credentials
const char* ssid = "##";
const char* password = "vile3204";

// MQTT broker info
const char* mqtt_server = "mqtt.eclipseprojects.io";
const int mqtt_port = 1883;
const char* mqtt_status_topic = "nirosshh/status";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  // Connect to Wi-Fi network
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Reconnect to MQTT broker if disconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  Serial.println("LoRa Master ready");
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // Main loop: handles MQTT connection and LoRa message processing
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int packetSize = LoRa.parsePacket();  // Check if a LoRa packet is received
  if (packetSize) {
    String message = "";
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }

    Serial.print("Received from Slave: ");
    Serial.println(message);

    // Parse message: format expected = "SLAVE_ID:VALUE"
    int sepIndex = message.indexOf(':');
    if (sepIndex != -1) {
      String slaveId = message.substring(0, sepIndex);
      String value = message.substring(sepIndex + 1);

      if (slaveId.length() > 0 && slaveId.toInt() <= Slave_No && value.length() > 0) {
        Serial.println("========== LoRa Message Received Start ");
        Serial.print("Parsed -> Slave ID: ");
        Serial.print(slaveId);
        Serial.print(", Value: ");
        Serial.println(value);

        // Publish full message to MQTT
        client.publish(mqtt_status_topic, message.c_str());
        Serial.println("Published to MQTT");

        // Send ACK back to the slave with same message
        delay(100); // short wait before sending back
        LoRa.beginPacket();
        LoRa.print(message);  // Echoing back same message
        LoRa.endPacket();
        Serial.println("ACK sent to Slave");
        Serial.println("========== LoRa Message Received End ");
        Serial.println("");
      } else {
        Serial.println("Invalid message format: missing data.");
      }
    } else {
      Serial.println("Invalid message format: no ':' found.");
    }
  }
}
