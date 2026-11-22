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
├── gateway_node/                       # ESP32 Super Mini 433MHz LoRa Fallback Gateway & LittleFS GUI
│   ├── gateway_node.ino                # LoRa RX (SCK=4, MISO=5, MOSI=6, CS=7, RST=3, DIO0=10) -> MQTT/Telegram
│   ├── pins.h                          # Super Mini LoRa SPI & Status LED Pinout (from docs/BluePrint.md)
│   └── data/                           # LittleFS Web GUI Files (Upload via Ctrl+Shift+P -> Upload LittleFS)
├── tests/
│   └── polarity_and_hardware_test/     # Bench diagnostic sketch for I2C scan + Flame/Relay polarity check
└── .gitignore
```

## Quick Start in Arduino IDE 2.3.11

### 1. Main Node (`ESP32-S3-N16R8`) — [`main_node/main_node.ino`](./main_node/main_node.ino)
- **Board:** `ESP32S3 Dev Module`
- **Flash Size:** `16MB (128Mb)` (`QIO 80MHz`)
- **PSRAM:** `OPI PSRAM`
- **Partition Scheme:** `Custom` (uses `main_node/partitions.csv`)
- **Fallback AP:** `LabSafety-S3-Setup` (password: `12345678`, IP: `http://192.168.4.1`, login: `admin` / `admin123`)

### 2. Gateway Node (`ESP32 Super Mini`) — [`gateway_node/gateway_node.ino`](./gateway_node/gateway_node.ino)
- **Board:** `ESP32C3 Dev Module` (or `ESP32S3 Dev Module` if using an S3 Super Mini)
- **USB CDC On Boot:** `Enabled`
- **Flash Size:** `4MB (32Mb)`
- **Partition Scheme:** `Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)`
- **Fallback AP:** `LabSafety-GW-Setup` (password: `12345678`, IP: `http://192.168.4.1`, login: `admin` / `admin123`)
