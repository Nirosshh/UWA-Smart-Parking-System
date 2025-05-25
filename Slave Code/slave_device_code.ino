// Slave MQTT Sensor

#include <SPI.h>  // SPI communication for LoRa
#include <LoRa.h>  // LoRa library for SX1262 communication

// Sensor
#include <Wire.h>  // I2C communication
#include <Arduino.h>
#include "SensorQMC6310.hpp"  // Magnetometer sensor header

#define SS 18  // LoRa chip select pin
#define RST 14
#define DIO0 26
#define LED_PIN 25  // Adjust according to your setup

// I2C pin config for regular T-Beam
#ifndef SENSOR_SDA
#define SENSOR_SDA  21
#endif

#ifndef SENSOR_SCL
#define SENSOR_SCL  22
#endif

int SLAVE_ID = 1;  // Unique ID for this parking slot device

SensorQMC6310 qmc;  // Create sensor object

// Magnetic field baseline
int baselineX = 0, baselineY = 0, baselineZ = 0;

// Sensitivity threshold (in μT)
const int detectionThreshold = 12; // lower more sensitive (max 15)

// Recalibration interval (in ms)
const unsigned long baselineIntervalMs = 2000; // every 2 seconds
unsigned long lastBaselineUpdate = 0;

// Parking Space Indicator
int Parking_Indicator_In = 0;
int Parking_Indicator_Responce = 0;

void setup() {
  // LED Indicator
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial);

  // Iniciate QMC6310 Sensor
  if (!qmc.begin(Wire, QMC6310U_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
    Serial.println("Failed to find QMC6310 - check your wiring!");
    while (1) {
      delay(1000);
    }
  }

  Serial.print("Device ID: ");
  Serial.println(qmc.getChipID(), HEX);

  // Configration of the QMC6310 Sensor
  qmc.configMagnetometer(
    SensorQMC6310::MODE_CONTINUOUS,
    SensorQMC6310::RANGE_8G,
    SensorQMC6310::DATARATE_200HZ,
    SensorQMC6310::OSR_1,
    SensorQMC6310::DSR_1
  );

  delay(500);  // Allow sensor to stabilize
  getBaseline();  // Initial baseline
  lastBaselineUpdate = millis();  // Start date

  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(868E6)) {  // Use 915E6 in Australia if needed
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }
  Serial.println("LoRa Slave ready");
  digitalWrite(LED_PIN, 0); // LED OFF
}

void sendCommand(String cmd) {
  // Send status to master and wait for response
  Serial.print("Sending to Master: ");
  Serial.println(cmd);
  Serial.println();

  // Send command via LoRa
  LoRa.beginPacket();
  LoRa.print(cmd);
  LoRa.endPacket();

  // Wait for response from Master
  long startTime = millis();
  while (millis() - startTime < 2000) { // waiting for 2 seconds
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String response = "";
      while (LoRa.available()) {
        response += (char)LoRa.read();
      }

      Serial.print("Received from Master: ");
      Serial.println(response);

      // Parse and check if the response is for this slave
      int sepIndex = response.indexOf(':');
      if (sepIndex != -1) {
        String receivedId = response.substring(0, sepIndex);
        String value = response.substring(sepIndex + 1);

        // Conform with slave ID
        if (receivedId.toInt() == SLAVE_ID) {
          if (value.toInt() == 0 || value.toInt() == 1) {
            Parking_Indicator_Responce = value.toInt();   // String to Int conversion
          }
          Serial.print("Updated Parking_Indicator_Responce = ");
          Serial.println(Parking_Indicator_Responce);
        } else {
          Serial.println("Response not for this SLAVE_ID. Ignored.");
          //Parking_Indicator_Responce = 1;   // ACK from master
        }
      } else {
        Serial.println("Invalid response format.");
        //Parking_Indicator_Responce = 1;
      }

      break;
    } else {
      //Parking_Indicator_Responce = 1;
    }
  }
}


// Geting base line of the QMC6310 sensor
void getBaseline() {
  // Recalibrate magnetic baseline
  //Serial.println("Recalibrating baseline...");

  while (!qmc.isDataReady()) {
    delay(10);
  }

  qmc.readData();
  baselineX = qmc.getX();
  baselineY = qmc.getY();
  baselineZ = qmc.getZ();

  //Serial.println("Baseline set:");
  //Serial.print("X: "); Serial.println(baselineX);
  //Serial.print("Y: "); Serial.println(baselineY);
  //Serial.print("Z: "); Serial.println(baselineZ);
  //Serial.println("Resuming monitoring...");
}

void loop() {
  // Main loop: read sensor, compare with baseline, update parking status
  if (millis() - lastBaselineUpdate >= baselineIntervalMs) {
    getBaseline();
    lastBaselineUpdate = millis();
  }

  if (qmc.isDataReady()) {
    qmc.readData();

    int x = qmc.getX();
    int y = qmc.getY();
    int z = qmc.getZ();

    int dx = abs(x - baselineX);
    int dy = abs(y - baselineY);
    int dz = abs(z - baselineZ);

    // Check the QMC6310 sensor reading with baseline threshold every time 
    if (dx > detectionThreshold || dy > detectionThreshold || dz > detectionThreshold) {
      Parking_Indicator_In = 1;   // If reading changed 
    } else {
      Parking_Indicator_In = 0;
    }

    if (Parking_Indicator_In) {
      if (!Parking_Indicator_Responce) {
        Serial.print("Occupied Parking Space : "); Serial.println(SLAVE_ID);
        String msg = String(SLAVE_ID) + ":1";
        Serial.println(msg);
        sendCommand(msg);
        digitalWrite(LED_PIN, 1); // LED ON
      }
    } else {
      if (Parking_Indicator_Responce) {
        Serial.print("Unoccupied Parking Space : "); Serial.println(SLAVE_ID);
        String msg = String(SLAVE_ID) + ":0";
        Serial.println(msg);
        sendCommand(msg);
        digitalWrite(LED_PIN, 0); // LED OFF
      }
    }
  }
}
