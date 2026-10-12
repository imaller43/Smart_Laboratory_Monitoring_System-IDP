# Project Proposal: IoT-Based Smart Laboratory Safety Monitoring System

## 1. Project Description
This project aims to develop an IoT-based smart laboratory safety monitoring system for real-time monitoring of laboratory environmental conditions. The microcontroller integrated with suitable sensors will be used to monitor temperature, humidity, air quality, and potential hazardous conditions. The collected data will be displayed locally and transmitted to an IoT platform for remote monitoring. When abnormal conditions are detected, the system will provide visual and audible alerts. The developed prototype will be tested under different conditions to evaluate its monitoring performance, response time, and overall functionality.

## 2. Project Overview
Laboratories containing chemicals, specialized equipment, and hazardous materials require strict environmental monitoring. This project aims to design and develop a low-cost, professional-grade IoT safety monitoring system. It goes beyond simple thresholds by implementing Machine Learning for anomaly detection, a hybrid control architecture for absolute fail-safe reliability, and a modern decoupled cloud microservices architecture.

## 3. Core Features & System Upgrades
*   **Real-Time Environmental Monitoring:** Continuous tracking of Temperature, Humidity, Barometric Pressure, and Air Quality (TVOC/eCO2).
*   **Sensor Fault Tolerance (Sensor Fusion):** Cross-references multiple sensors using hardcoded logic to distinguish between a real fire, a chemical leak, or a broken/short-circuited sensor, preventing false alarms.
*   **Absolute Fail-Safe Architecture:** Uses unbreakable C++ "Hardcoded Rules" on the edge to guarantee immediate alarm triggering in emergencies, regardless of network status.
*   **Captive Web Portal (LittleFS):** Zero hardcoded credentials. Wi-Fi and MQTT configurations are updated via a local ESP32 Access Point webpage and stored persistently in Flash memory.
*   **Remote Alerting with Radio Fallback:** Instant push notifications sent to administrators via Telegram Bot API, with a LoRa radio fallback to guarantee delivery during Wi-Fi outages.

---

## 4. Hardware Architecture & Budget (Pro-Tier Setup)

**Microcontroller:**
*   **ESP32 ESP32-S3-N16R8 (38-pin, Type-C) (~RM 25):** The dual-core brain of the system, handling I2C sensors, LittleFS storage, and Wi-Fi communication simultaneously. **For Data Logger
*   **ESP32 Super Mini:** For LoRa Gateway to receive the alert from Main ESP32-S3 and forward it to the cloud when WiFi is not avaiable (Backup Plan)

**Input Sensors (Digital I2C & UART):**
*   **SGP30 or CCS811 Sensor (~RM 55):** Factory-calibrated digital sensor for highly accurate TVOC (Toxic Gas) and eCO2 readings without analog noise.
*   **BME280 Sensor (~RM 25):** High-speed digital I2C sensor for Temperature, Humidity, and Pressure.
*   **HLK-LD2410 mmWave Radar (~RM 25):** Advanced radar sensor to detect human micro-movements (presence/breathing) for off-hours security.
*   **IR Flame Sensor (~RM 5):** Instant optical detection of an active fire.

**Output, Actuators & Communications:**
*   **2 x LoRa Communication Module (e.g., SX1278) (~RM 30):** Acts as an independent radio-frequency fallback to transmit emergency payloads to a secondary gateway if the primary lab Wi-Fi fails.
*   **2-Channel 5V Relay Module (~RM 12):** Controls higher-power devices (like the Exhaust Fan).
*   **Small 5V DC Exhaust Fan (~RM 10):** Used to simulate emergency ventilation (clearing gas leaks).
*   **Active Buzzer (~RM 5):** The loud physical siren for evacuation alerts.
*   **ARGB LED Module / WS2812B (~RM 10):** A smart, color-changing addressable LED for the presentation model. Uses only 1 data pin to show visual states (Green = Safe, Yellow = Warning, Red = Fire).
*   **5V Ultrasonic Atomizer / Humidifier Module (~RM 10):** A creative and safe way to simulate an emergency "Water Sprinkler" system for the presentation diorama. Instead of spraying real water that could damage electronics, it generates a visible, safe cool mist when a fire is detected!
*   **0.96" I2C OLED Display (~RM 15):** Mounted on the device to show live status.

*Estimated Hardware Total: ~RM 192 (Well within the RM 350 budget, leaving room for professional casing and power supplies).*

---

## 5. System Logic & Emergency Conditions (Sensor Fusion)

The system relies on "Sensor Fusion" (combining data from multiple sensors) to make intelligent edge decisions and avoid false alarms. The outputs (Fan, Sprinkler, Alarm, ARGB LED) are controlled by the following logic matrix:

### State 0: Normal / Safe
*   **Condition:** Gas TVOC < 500 ppb AND Temperature < 32°C AND Flame == 0
*   **Outputs:** ARGB LED = **GREEN** | Fan = OFF | Sprinkler = OFF | Buzzer = OFF
*   **Action:** System passively logs data to the database.

### State 1: Minor Chemical Leak (Warning)
*   **Condition:** Gas TVOC > 500 ppb AND Temperature < 32°C AND Flame == 0
*   **Outputs:** ARGB LED = **YELLOW** | Fan = **ON** | Sprinkler = OFF | Buzzer = **BEEPING 2 TIMES, 2 SECONDS APART**
*   **Action:** The ESP32 instantly forces the exhaust fan on to clear the toxic air. A Telegram Warning payload is routed to Node-RED. 

### State 2: Severe Toxic Leak (Critical)
*   **Condition:** Gas TVOC > 1500 ppb AND Flame == 0
*   **Outputs:** ARGB LED = **FLASHING YELLOW** | Fan = **ON** | Sprinkler = OFF | Buzzer = **BEEPING 2 TIMES, 1 SECOND APART**
*   **Action:** The lab must be evacuated. Fan runs at full speed to clear the deadly gas. Sprinkler is off (water doesn't help gas leaks). Telegram Emergency Alert payload routed to Node-RED.

### State 3: Confirmed Fire (Emergency)
*   **Condition:** Flame == 1 AND (Temperature > 45°C OR Temp rising rapidly)
*   **Outputs:** ARGB LED = **FLASHING RED** | Fan = **OFF** | Sprinkler (Atomizer) = **ON** | Buzzer = **ON**
*   **Action:** The Exhaust Fan is instantly forced OFF to stop feeding the fire with fresh oxygen. The Ultrasonic Atomizer ("Sprinkler") turns ON to suppress the fire. The buzzer sounds. Telegram Emergency Alert payload routed to Node-RED.

### State 4: Sensor Glitch / False Alarm
*   **Condition:** Flame == 1 AND Temperature < 30°C AND Gas TVOC < 250 ppb
*   **Outputs:** ARGB LED = **BLUE** | Fan = OFF | Sprinkler = OFF | Buzzer = OFF
*   **Action:** The ESP32 standard logic identifies a physical impossibility (a fire with no heat and no smoke). It forces the outputs off, ignores the flame sensor, and pushes a maintenance alert payload to Node-RED to notify administrators of a broken sensor.

---

## 6. Modern Decoupled Cloud Architecture

This system utilizes a highly scalable, decoupled microservices approach:

1.  **Ingestion (Edge):** The Main ESP32 reads sensors and publishes data to a Mosquitto MQTT Broker hosted on a cloud Virtual Machine.
2.  **Multistage Radio Fallback:** If the local Wi-Fi drops, the Main ESP32 broadcasts emergency payloads via a LoRa module to a Secondary Gateway ESP32 located on a different network, which then publishes the payload to the cloud MQTT broker.
3.  **Middleware & Logic (Node-RED):** Node-RED runs on the VM, subscribes to the MQTT topics, processes the ML/Anomaly logic, triggers Telegram alerts, and formats the data.
4.  **Storage Layer:** Node-RED writes the processed historical data to a time-series database (e.g., **InfluxDB** or **MySQL**) residing on the VM.
5.  **API Layer:** Node-RED exposes custom HTTP REST API endpoints (`/api/data`) to securely serve database queries.
6.  **Presentation (Frontend):** A responsive, custom dashboard built with a modern web framework (React/Vue) and hosted separately on **Netlify**. The Netlify frontend makes API calls to Node-RED to visualize the data.

---

## 7. Team Task Delegation (4 Members)

1.  **Hardware & Embedded Engineer:** 
    *   Wiring the I2C bus, power management, physical casing, and the LoRa radio module.
    *   Programming the ESP32 (C++), implementing the LittleFS/WiFiManager Captive Portal, and reading digital sensors.
2.  **Control Logic Developer:** 
    *   Designing the Sensor Fusion logic matrix.
    *   Writing the hardcoded C++ fallback rules (States 0 through 4) to ensure the ESP32 processes edge emergencies instantly.
3.  **Cloud Backend Engineer:** 
    *   Deploying the Linux VM, Dockerizing EMQX MQTT, Node-RED, and the Database.
    *   Writing Node-RED flows to route data, format Telegram alerts, and build the REST API endpoints.
4.  **Frontend/UI Developer:** 
    *   Designing the interactive web dashboard (React/Vue or Any other frameworks).
    *   Deploying the dashboard to Netlify and connecting it to the Node-RED API.

## 8. Power Architecture & Delivery (Multi-Rail Setup)

To prevent the ESP32-S3 from crashing or entering a brownout reset loop during a State 3 emergency, the hardware relies on a highly decoupled "Multi-Rail" power architecture to safely distribute heavy current loads. 

**1. Main Power Supply & Step-Down**
*   The system is powered by an external 12V 2A power adapter.
*   This input routes directly into an **LM2596S Step-Down Converter**, manually tuned to output exactly **5.0V**.
*   This 5.0V output connects to the **VIN / 5V pin** on the ESP32-S3 to power the microcontroller itself.

**2. High-Power 5.0V Rail (External)**
The following high-current components pull their VCC/5V power directly from the LM2596S converter, bypassing the ESP32-S3's onboard regulator entirely:
*   5V Ultrasonic Atomizer / Humidifier
*   5V DC Exhaust Fan
*   2-Channel Relay Module (VCC/JD-VCC pins)
*   ARGB WS2812B LED Module
*   HLK-LD2410 mmWave Radar (Requires stable 5V for reliable detection)

**3. Dedicated LoRa 3.3V Rail (Hardware Fallback Stability)**
To ensure peak radio transmission spikes do not overload the microcontroller, the LoRa module runs on its own isolated power supply:
*   A tiny **AMS1117-3.3V Step-Down Module** takes input from the 5.0V rail and outputs 3.3V strictly to the **LoRa SX1278 VCC pin**.

**4. Low-Power 3.3V Logic Rail (Internal)**
The ESP32-S3's internal 3.3V pin is reserved strictly for lightweight, low-current digital sensors:
*   BME280 Sensor
*   SGP30 or CCS811 Sensor
*   0.96" I2C OLED Display
*   IR Flame Sensor

**5. Common Ground (Critical Requirement)**
To ensure digital logic signals (I2C, SPI, UART, HIGH/LOW triggers) pass successfully between the ESP32-S3 and the external components, all system grounds (ESP32-S3 GND, LM2596S GND, AMS1117 GND, and Sensor GNDs) must be physically tied together into a single common ground.

---

## 9. ESP32-S3 GPIO Pinout Configuration

The system utilizes the ESP32-S3's flexible GPIO matrix. The mapping below strategically avoids all system-reserved "strapping pins" (GPIO 0, 3, 45, 46) to guarantee safe booting and reliable hardware execution.

| Component / Function | Component Pin Name | ESP32-S3 GPIO | Notes & Safety Checks |
| :--- | :--- | :--- | :--- |
| **Shared I2C Bus** | SDA | **GPIO 8** | Connect BME280, SGP30, and OLED SDA here. |
| (Sensors & Display) | SCL | **GPIO 9** | Connect BME280, SGP30, and OLED SCL here. |
| **HLK-LD2410 Radar** | TX (Transmit) | **GPIO 18** | **Cross-wire:** Radar TX connects to ESP32 RX. |
| (UART Serial) | RX (Receive) | **GPIO 17** | **Cross-wire:** Radar RX connects to ESP32 TX. |
| **LoRa SX1278** | SCK (Clock) | **GPIO 12** | Standard SPI clock. |
| (SPI Bus) | MISO (Data In) | **GPIO 13** | Standard SPI data in. |
| | MOSI (Data Out) | **GPIO 11** | Standard SPI data out. |
| | CS / NSS (Chip Select)| **GPIO 10** | Tells the LoRa module to listen. |
| | RST (Reset) | **GPIO 14** | Resets the LoRa module on boot. |
| | DIO0 (Interrupt) | **GPIO 15** | Triggers the ESP32 when a radio packet arrives. |
| **IR Flame Sensor** | D0 / OUT (Digital Out) | **GPIO 4** | Simple HIGH/LOW digital input. |
| **Exhaust Fan Relay** | IN1 (Relay Channel 1) | **GPIO 5** | Triggers the 5V fan (State 1 & 2). |
| **Atomizer Relay** | IN2 (Relay Channel 2) | **GPIO 6** | Triggers the water sprinkler (State 3). |
| **Active Buzzer** | I/O or SIG | **GPIO 7** | Simple HIGH/LOW digital output. |
| **ARGB LED (WS2812B)** | DIN (Data In) | **GPIO 21** | Uses the FastLED or Adafruit NeoPixel library. |
