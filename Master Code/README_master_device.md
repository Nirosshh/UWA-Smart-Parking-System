
# Smart Parking System – Master Device (LoRa + MQTT)

This Arduino sketch is for the **master node** in a smart parking system. It receives messages from slave nodes via LoRa and publishes slot status to an MQTT broker for web integration.

## Hardware Used
- TTGO T-Beam ESP32 (with LoRa SX1262)
- Wi-Fi enabled for MQTT communication
- Connected to cloud via MQTT broker

## Communication
- **LoRa (868 MHz)** to receive `SlotID:Status` messages from slave devices
- **MQTT** to publish data to `nirosshh/status` topic on `mqtt.eclipseprojects.io`

## Functionality
- Listens for messages from slave devices like `1:1`, `2:0`
- Parses and validates messages
- Publishes received data to MQTT broker
- Sends acknowledgment back to slave via LoRa

## Pin Configuration
| LoRa Pin | GPIO | Function            |
|----------|------|---------------------|
| SS       | 18   | SPI Chip Select     |
| RST      | 14   | LoRa Reset          |
| DIO0     | 26   | LoRa Interrupt Pin  |

## MQTT Details
- **Broker:** mqtt.eclipseprojects.io
- **Port:** 1883
- **Topic:** nirosshh/status

## Uploading
Use Arduino IDE with the ESP32 board installed.  
**Board:** TTGO T-Beam  
**Upload Speed:** 921600  
**Flash Frequency:** 80 MHz

---

© 2025 | Group 13 – UWA CITS5506
