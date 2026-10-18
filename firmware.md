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