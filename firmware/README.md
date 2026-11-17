# Firmware (`ESP32-S3-N16R8` Main Node & `ESP32 Super Mini` LoRa Gateway)

**Owners:** Hardware & Embedded Engineer + Control Logic Developer

## Directory Structure
```text
firmware/
├── main_node/                          # ESP32-S3-N16R8 Main Node Firmware & LittleFS GUI
│   ├── main_node.ino                   # Dual-Core FreeRTOS Safety State Machine + Web/MQTT/Telegram/LoRa
│   ├── pins.h                          # Centralized GPIO Pinout & Active-LOW/HIGH Polarity Macros
│   ├── partitions.csv                  # Custom 16MB Flash Partition Table (Dual 4MB OTA + 2MB LittleFS)
│   └── data/                           # LittleFS Web GUI Files (Upload via Ctrl+Shift+P -> Upload LittleFS)
│       ├── login.html                  # Cookie-session login page
│       ├── index.html                  # Dashboard (ESP32 Health + Wi-Fi Credentials + Authentication)
│       ├── iot.html                    # IoT Configuration (MQTT Broker Connection: Local TCP/TLS & Cloud WS/WSS)
│       ├── telegram.html               # Telegram Config (Credentials, Alert Settings timer + dropdown, Bot Cmds)
│       ├── update.html                 # System Update (Check for Update button + Firmware/LittleFS .bin upload)
│       ├── style.css                   # Light/Dark responsive stylesheet
│       ├── app.js                      # Theme toggle & shared utilities
│       ├── config.example.json         # Tracked default configuration template
│       ├── telegram.json               # Default Telegram bot settings
│       ├── users.json                  # Default login credentials (admin / admin123)
│       └── fs_version.txt              # LittleFS image version
├── tests/
│   └── polarity_and_hardware_test/     # Bench diagnostic sketch for I2C scan + Flame/Relay polarity check
└── .gitignore
```

## Quick Start in Arduino IDE 2.3.11
1. Open [`main_node/main_node.ino`](./main_node/main_node.ino) in **Arduino IDE 2.3.11**.
2. Configure **Tools** menu:
   - **Board:** `ESP32S3 Dev Module`
   - **Flash Size:** `16MB (128Mb)` (`QIO 80MHz`)
   - **PSRAM:** `OPI PSRAM`
   - **Partition Scheme:** `Custom` (automatically uses `main_node/partitions.csv`)
   - **USB CDC On Boot:** `Enabled` (if connected to native `USB` port) or `Disabled` (if connected to `UART`/`COM` port)
3. **Upload Firmware:** Click **Upload** (`Ctrl + U`).
4. **Upload LittleFS Web GUI:** Close the Serial Monitor, press `Ctrl + Shift + P`, and select **`Upload LittleFS to Pico/ESP8266/ESP32`**.
5. **Connect & Log In:**
   - If Wi-Fi is not configured yet, connect to the ESP32 Access Point **`LabSafety-S3-Setup`** (password: `12345678`) and open `http://192.168.4.1`.
   - Default login: **`admin`** / **`admin123`**.
