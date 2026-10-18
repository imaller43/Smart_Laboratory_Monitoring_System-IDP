# IDP Firmware Architecture: Lab Safety Monitoring System
**Hardware:** ESP32-S3 (Main Node) & ESP32 Super Mini (Gateway)
**OS:** FreeRTOS (Dual-Core Implementation)

## 1. FreeRTOS Core Allocation & Multithreading
To ensure zero-latency emergency response, the firmware strictly separates hardware actuation from network transmission.

*   **Core 1: Hardware & Logic (High Priority)**
    *   **Hardware Interrupt (ISR):** Attached to the IR Flame Sensor (GPIO 4). Triggers instantly upon flame detection.
    *   **Sensor Task:** Polls BME280 and SGP30 via I2C exactly every 1000ms. Maintains a Running Sum/Count for averaging (does not use arrays to save RAM).
    *   **State Machine Task:** Evaluates sensor data against the safety thresholds, triggers local GPIO actuators instantly, and passes payload data to Core 0.
*   **Core 0: Network & Publishing (Lower Priority)**
    *   **Network Task:** Waits for data from Core 1, formats it into JSON, and transmits via SPI to the LoRa SX1278 (or via Wi-Fi/MQTT).

## 2. Inter-Task Communication
*   **Mutex (Shared Memory):** A global `SensorSnapshot` struct holds the latest temperature, TVOC, and flame status. The Mutex is locked briefly by the Sensor Task to update data, and by the State Machine to read it, preventing asynchronous data corruption.
*   **FreeRTOS Queue:** When a state changes or a publish interval is reached, Core 1 pushes a data packet into a Queue. Core 0 reads from this Queue to format the JSON and publish. This ensures network lag never blocks GPIO actuation.

## 3. Sensor Polling & Data Handling
*   **SGP30 & BME280:** Strictly polled at **1Hz (every 1 second)** to maintain the SGP30 MOX heater baseline.
*   **Averaging:** Temperature and TVOC use a continuous `sum` and `count` variable. Upon publishing during periodic modes, the sum is divided by the count, and variables reset to zero.

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