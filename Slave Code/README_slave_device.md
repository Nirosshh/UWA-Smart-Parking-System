
# Smart Parking System – Slave Device (LoRa + Magnetometer)

This Arduino sketch is for a **slave node** in a smart parking system. Each node detects whether a vehicle is parked above a slot and communicates the status to a master node using LoRa.

## Hardware Used
- TTGO T-Beam ESP32 (with LoRa SX1262)
- QMC6310 Magnetometer Sensor (I2C)
- 18650 Li-ion Battery
- Jumper wires / breadboard

## Communication
- **Sensor → Slave ESP32** via I2C
- **Slave ESP32 → Master ESP32** via LoRa (868 MHz)
- Message format: `SlotID:Status` (e.g., `1:1` for occupied, `1:0` for available)
- Master returns ACK using the same format

## Functionality
- Reads magnetic field values from the QMC6310
- Compares with calibrated baseline to detect vehicle presence
- Sends status to Master via LoRa
- Waits for ACK and updates local status accordingly
- Turns LED ON (occupied) or OFF (free)

## Pin Configuration
| Component     | Pin     |
|---------------|---------|
| QMC6310 SDA   | GPIO 21 |
| QMC6310 SCL   | GPIO 22 |
| LoRa SS       | GPIO 18 |
| LoRa RST      | GPIO 14 |
| LoRa DIO0     | GPIO 26 |
| LED           | GPIO 25 |

## Uploading
Use Arduino IDE with the ESP32 board installed. Select the correct port and board:  
**Board:** TTGO T-Beam  
**Upload Speed:** 921600  
**Flash Frequency:** 80 MHz

## Notes
- Baseline recalibrates every 2 seconds
- Sensitivity can be adjusted via `detectionThreshold`
- Ensure wiring is correct or the sensor will fail to initialize
- Check the Slave ID before uploading the code to SLAVE

---

© 2025 | Group 13 – UWA CITS5506
