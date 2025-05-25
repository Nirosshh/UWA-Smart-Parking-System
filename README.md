
# 🅿️ UWA Smart Parking System using LoRa and MQTT

This is an IoT-based parking space detection and monitoring system using TTGO T-Beam ESP32 microcontrollers, LoRa communication, and MQTT protocol. The project detects vehicle presence using a magnetometer sensor and updates a web interface in real-time via MQTT.

---

## 📦 Project Structure

```
Smart-Parking-System/
├── Master Code/               # Code for Master ESP32 (receives via LoRa, sends to MQTT)
├── Slave Code/                # Code for Slave ESP32 (sensor + LED + LoRa)
├── Flow Chart.jpg             # Project logic and communication flow
└── README.md                  # This file
```

---

## ⚙️ Hardware Components

- **TTGO T-Beam ESP32**  
  LoRa SX1262, Wi-Fi, GPS – used as both Master and Slave
- **QMC6310 Magnetometer**  
  Triple-axis sensor detects magnetic field disturbance from parked vehicles
- **18650 Li-ion Battery (3.7V)**  
  Powered through TTGO’s built-in battery management system

---

## 🔁 Communication Architecture

- **LoRa (868 MHz)**:  
  Slave → Master  
  Format: `SlaveID:Status` (e.g., `1:1`, `2:0`)

- **MQTT Protocol**:  
  Master publishes to: `nirosshh/status`  
  Broker: `mqtt.eclipseprojects.io`

---

## 💡 Features

- Real-time parking slot monitoring
- Wireless long-range communication
- MQTT integration with web interface
- Visual indicators (LEDs) and dashboard

---

## 🚀 How to Use

1. Flash `Slave Code` to T-Beam connected to QMC6310 sensor
2. Flash `Master Code` to T-Beam connected to MQTT server
3. Power each device via battery or USB
4. Subscribe to topic `nirosshh/status` on any MQTT-compatible dashboard or webpage
5. Sensor detects car → Slave sends status → Master publishes → Web updates

---

## 🖥️ Web Integration

The master node publishes each slot's status.  
The web app subscribes to the MQTT topic and changes color:
- 🟢 Green: Slot available
- 🔴 Red: Slot occupied

---

## 📸 Visual Flow

![Flow Chart](./Flow%20Chart.jpg)

---

## 👤 Authors

Group 13 – University of Western Australia  
Course: CITS5506 – The Internet of Things

---

© 2025 | All Rights Reserved
