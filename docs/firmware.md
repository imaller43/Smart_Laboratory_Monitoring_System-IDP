# IDP Firmware Architecture: Lab Safety Monitoring System
**Hardware:** ESP32-S3-N16R8 (Main Node) & ESP32 Super Mini (Gateway)
**OS:** FreeRTOS (Dual-Core Implementation)
**Toolchain:** Arduino IDE 2.3.11 (`esp32` by Espressif Systems v3.x, `16MB Flash`, `OPI PSRAM`, `Custom` Partition Scheme + `arduino-littlefs-upload` plugin)

## 1. FreeRTOS Core Allocation & Multithreading
To ensure zero-latency emergency response, the firmware strictly separates hardware actuation from network transmission.

*   **Core 1: Hardware & Logic (High Priority)**
    *   **Hardware Interrupt (ISR):** Attached to the IR Flame Sensor (GPIO 4). Triggers instantly upon flame detection.
    *   **Sensor & Display Task:** Polls BME280 and SGP30 via I2C (`GPIO 8` SDA, `GPIO 9` SCL) every 1000ms, reads human presence and target distance (`cm`) from the HLK-LD2410 mmWave radar via `HardwareSerial(1)` (`256000` baud on RX `GPIO 18`, TX `GPIO 17`), and updates the **1.3" SH1106 I2C OLED Display** with live readings, system state, and presence/distance. Maintains a Running Sum/Count for averaging (does not use arrays to save RAM).
    *   **State Machine Task:** Evaluates sensor data against the safety thresholds, triggers local GPIO actuators instantly, and passes payload data to Core 0.
*   **Core 0: Network & Publishing (Lower Priority)**
    *   **Network Task:** Waits for data from Core 1, formats it into JSON (including `temp`, `humidity`, `pressure`, `tvoc`, `eco2`, `flame`, `state`, `presence`, and `distance_cm`), and transmits via Wi-Fi/MQTT or falls back to SPI **433 MHz LoRa SX1278 (`433E6`)**.

## 2. Inter-Task Communication
*   **Mutex (Shared Memory):** A global `SensorSnapshot` struct holds the latest temperature, humidity, pressure, TVOC, eCO2, flame status, radar `presence` (boolean), and radar target `distance_cm` (`uint16_t`). The Mutex is locked briefly by the Sensor Task to update data, and by the State Machine to read it, preventing asynchronous data corruption.
*   **FreeRTOS Queue:** When a state changes or a publish interval is reached, Core 1 pushes a data packet into a Queue. Core 0 reads from this Queue to format the JSON and publish. This ensures network lag never blocks GPIO actuation.

## 3. Sensor Polling, Radar & Local OLED Display
*   **SGP30 & BME280:** Strictly polled at **1Hz (every 1 second)** to maintain the SGP30 MOX heater baseline.
*   **Averaging:** Temperature and TVOC use a continuous `sum` and `count` variable. Upon publishing during periodic modes, the sum is divided by the count, and variables reset to zero.
*   **HLK-LD2410 mmWave Radar:** Continuously parsed over `HardwareSerial(1)` at `256000` baud. Extracts `presence` (`true`/`false`) and target `distance_cm`, which are included in every MQTT/LoRa JSON payload and shown on both the **1.3" SH1106 OLED Display** and the remote web dashboard.
*   **1.3" SH1106 OLED Display (`U8g2`):** Displays live Temperature, Humidity, TVOC/eCO2, active Safety State (`0–4`), and Radar Presence + Target Distance (`cm`).

## 4. State Machine Logic
*   **State 0 (Normal):** TVOC < 500 ppb AND Temp < 32°C AND Flame == 0
    *   *Outputs:* ARGB = GREEN | Fan = OFF | Sprinkler = OFF | Buzzer = OFF
*   **State 1 (Warning):** TVOC > 500 ppb AND Temp < 32°C AND Flame == 0
    *   *Outputs:* ARGB = YELLOW | Fan = ON | Sprinkler = OFF | Buzzer = BEEPING (2x / 2 sec)
*   **State 2 (Critical):** TVOC > 1500 ppb AND Flame == 0
    *   *Outputs:* ARGB = FLASHING YELLOW | Fan = ON | Sprinkler = OFF | Buzzer = BEEPING (2x / 1 sec)
*   **State 3 (Emergency):** Flame == 1 AND (Temp > 45°C OR Temp rising rapidly)
    *   *Outputs:* ARGB = FLASHING RED | Fan = OFF | Sprinkler (Atomizer) = ON | Buzzer = ON
*   **State 4 (Glitch):** Flame == 1 AND Temp < 30°C AND TVOC < 250 ppb
    *   *Outputs:* ARGB = BLUE | Fan = OFF | Sprinkler = OFF | Buzzer = OFF

## 5. Dynamic Publishing Rules (Edge-Triggered)
The firmware does not use absolute time (RTC). It uses relative uptime (`millis()`) to manage dual publishing modes:

*   **Mode A: Periodic Telemetry (States 0 & 4)**
    *   Interval: **Every 3 minutes (180,000 ms)**.
    *   Action: Publishes the calculated average over the last 3 minutes.
*   **Mode B: Real-Time Burst (States 1, 2, & 3)**
    *   Interval: **Every 1 second (1000 ms)**.
    *   Action: Publishes live 1Hz raw data.
*   **Edge-Trigger Override:** The exact millisecond the system transitions from any state to another, the ESP32 bypasses all timers and fires an **immediate MQTT payload**.

## 6. Backend Data Routing (Node-RED)
The JSON payload published by the ESP32 is managed by Node-RED using a Split-Routing strategy:
*   **Path 1 (WebSockets):** All incoming data is routed directly to the frontend dashboard for 1Hz real-time updates during an emergency.
*   **Path 2 (InfluxDB):** Data routed to the database passes through a Rate Limiting node (max 1 message per 10 seconds) to prevent TSDB disk bloat during burst mode.

## 7. Telegram Alerting & Rate Limiting (Node-RED)
To prevent Telegram Bot API bans (spamming) during the 1-second emergency burst mode, Node-RED handles alerts through a strict routing and throttling sequence:

*   **`Switch` Node (Logic Routing):** Evaluates `msg.payload.state`. Drops State 0 (Normal) to prevent unnecessary processing, and routes States 1, 2, 3, and 4 to their respective alert paths.
*   **`RBE` Node (Report by Exception):** Acts as a critical spam-blocker. It stops the continuous 1Hz data stream, only allowing an alert to pass through to Telegram if the `state` value has *changed* (e.g., only sends one message the exact moment it transitions from Warning to Emergency).
*   **`Function` Node (Message Formatting):** Executes JavaScript to parse the JSON (`temp`, `tvoc`, `state`) and concatenates it into a human-readable text string with contextual emergency emojis.
*   **`Telegram Sender` Node:** Utilizes the `node-red-contrib-telegrambot` palette to receive the formatted string and push it securely to the designated Chat ID.

## 8. File System (LittleFS) & Admin Dashboard
The ESP32-S3 hosts an embedded Single Page Application (SPA) dashboard used strictly as an **Admin Maintenance Portal** (for Wi-Fi setup, sensor threshold adjustments, diagnostics, and OTA updates). This operates completely independently of the Node-RED live operator feed.

### Memory Allocation & Web Server
*   **Capacity:** Out of the available flash memory, ~1.5MB is partitioned for LittleFS. The static dashboard files (HTML, CSS, JS) are gzip-compressed (`index.html.gz`) to consume less than 100KB, leaving ample room for potential offline data logging.
*   **Asynchronous Processing (`ESPAsyncWebServer`):** The web server runs exclusively on **Core 0**. It handles multiple client connections asynchronously, guaranteeing that HTTP requests never block or delay the Core 1 hardware safety interrupts.
*   **AJAX & REST APIs:** The ESP32 does not build HTML dynamically. It serves the static files once, and the browser's JavaScript uses background AJAX requests to hit ESP32 REST API endpoints (e.g., `GET /api/health`, `GET /api/config`). The ESP32 responds with lightweight JSON (CPU temp, free heap, RSSI, active state) to populate the UI.

### Non-Volatile Data Management
Writing to flash memory briefly halts the CPU. To maintain industrial reliability, writes are strictly managed:
*   **Dynamic Configuration (`config.json`):** Stores network credentials and safety thresholds. Parsed via `ArduinoJson` during `setup()`, eliminating hardcoded variables.
*   **SGP30 Baseline (`baseline.txt`):** Prevents the 12-hour MOX sensor recalibration cycle on reboot. A low-priority Core 0 task saves the baseline once per hour. Hardware interrupts are briefly muted for ~10ms during this flash write to prevent ISR watchdog faults.

### OTA Updates & Memory Persistence
The flash memory utilizes a custom partition table separating application logic from stored data.
*   **Data Persistence:** Web-based OTA updates only overwrite the Application partitions (`app0`/`app1`). The LittleFS Data partition remains completely untouched. Wi-Fi credentials, MQTT configs, and sensor baselines persist seamlessly across firmware updates.
*   **Safety OTA Interlock:** The firmware enforces a strict HTTP 403 Forbidden block on the `/update` POST route if the system is in any state other than `STATE_NORMAL`. This guarantees the ESP32 cannot be rebooted or blinded by an administrator during an active chemical leak or fire.

## 9. Firmware Updates & Memory Partitioning
The ESP32 flash memory is governed by a custom partition table separating the application code from the non-volatile data.

*   **OTA Updates:** Over-The-Air updates exclusively target the Application Partition (`app0`/`app1`). 
*   **Data Persistence:** The LittleFS Data Partition remains completely untouched during standard OTA updates. The `config.json` (network credentials) and `baseline.txt` (sensor calibration) persist across firmware versions, allowing the device to instantly reconnect to the network upon reboot without administrator intervention.
*   **OTA Interlock:** Web-based OTA updates are strictly rejected via a 403 HTTP status if the system is not in `STATE_NORMAL`, preventing system blindness during a safety incident.

## 10. Arduino IDE 2.3.11 Setup, Libraries & MQTT Payload Schema

### Board Configuration (`ESP32-S3-N16R8`)
*   **Board Package:** `esp32` by Espressif Systems (v3.x)
*   **Board:** `ESP32S3 Dev Module`
*   **Flash Size:** `16MB (128Mb)` (`QIO 80MHz`)
*   **PSRAM:** `OPI PSRAM` (8 MB Octal PSRAM)
*   **Partition Scheme:** `Custom` (uses `firmware/main_node/partitions.csv` with dual 4 MB OTA slots + 2 MB LittleFS)
*   **Filesystem Uploader:** `arduino-littlefs-upload` VSIX plugin (`Ctrl + Shift + P` -> `Upload LittleFS to Pico/ESP8266/ESP32`)

### Required Libraries
| Subsystem | Library | Author / Maintainer | Notes |
| :--- | :--- | :--- | :--- |
| **BME280** | `Adafruit BME280 Library` | Adafruit | I2C (`0x76`/`0x77`) |
| **SGP30** | `Adafruit SGP30 Sensor` | Adafruit | I2C (`0x58`), 1 Hz polling |
| **1.3" OLED (SH1106)** | `U8g2` | olikraus | `U8G2_SH1106_128X64_NONAME_F_HW_I2C` (`0x3C`) |
| **HLK-LD2410 Radar** | `MyLD2410` | Iavor Veltchev | `HardwareSerial(1)` at `256000` baud (`RX=18`, `TX=17`) |
| **LoRa SX1278 (433 MHz)** | `LoRa` | Sandeep Mistry | `SPI.begin(12, 13, 11, 10)` + `LoRa.setPins(10, 14, 15)` at `433E6` |
| **WS2812B ARGB LED** | `Adafruit NeoPixel` | Adafruit | Data on `GPIO 21` |
| **Async Web Portal** | `ESPAsyncWebServer` + `AsyncTCP` | `ESP32Async` | Core 3.x compatible forks |
| **JSON & MQTT** | `ArduinoJson` (v7.x) + `PubSubClient` | Benoit Blanchon / Nick O'Leary | `mqttClient.setBufferSize(512)` |

### Telemetry JSON Schema (MQTT & LoRa Fallback)
```json
{
  "node": "lab_main_s3",
  "state": 0,
  "temp": 26.4,
  "humidity": 58.2,
  "pressure": 1011.5,
  "tvoc": 45,
  "eco2": 412,
  "flame": 0,
  "presence": true,
  "distance_cm": 124,
  "uptime_s": 3600
}
```