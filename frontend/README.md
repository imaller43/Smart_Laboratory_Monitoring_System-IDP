# Sensor Value - Real-Time IoT Dashboard

A sleek, responsive, and dark-themed IoT monitoring and control web dashboard built with standard **HTML5**, **CSS3**, and **JavaScript (ES6+)**.

---

## 📸 Overview & Features

- **Live Environmental Gauges**:
  - **Humidity (%)**: Semi-circular gauge with color gradient arc, live pulsing indicator, and dynamic needle.
  - **Temperature (°C)**: Semi-circular gauge showing ambient temperature in degrees Celsius.
  - **Light Density (LPD)**: Wide-range luminous intensity meter (0–3000 LPD).
- **Interactive GPIO Remote Switches**:
  - Control hardware relays or pins for **FAN**, **INTERRUPT**, and **LIGHT** with real-time feedback and state sync.
- **Real-Time Data Logs**:
  - Live tabular logs for Humidity, Temperature, and Light Density with timestamps and color-coded headers.
- **Dual-Axis & Area Charts (Chart.js)**:
  - Dual-Axis line chart for simultaneous tracking of Humidity (%) and Temperature (°C).
  - Area curve for real-time Light Density.
  - Full-width Mean Hourly Data chart showing 24/48-hour cyclical diurnals.
- **Dual-Mode Telemetry Engine**:
  - **Internal Simulation Engine**: Realistic random-walk data generator running out of the box with zero external dependencies.
  - **Live MQTT over WebSockets**: Seamlessly connect to public brokers (e.g., `broker.emqx.io`, `test.mosquitto.org`) or your local Mosquitto/HiveMQ broker.
- **Multi-Room Support**:
  - Switch between `DESIGN ROOM`, `ROOM 2`, and `ROOM 3`.
- **Theme Toggler**:
  - High-contrast Cyberpunk / Dark Slate theme and clean Light theme.

---

## 🚀 Quick Start

### Option 1: Direct File Open
Simply double-click `index.html` or open it directly in any modern browser (Chrome, Edge, Firefox, Safari).

### Option 2: Local Web Server
You can serve the directory using any HTTP server:

```bash
# Python 3
python -m http.server 8080

# Or Node / npx
npx serve .

# Or VS Code
Right-click index.html -> "Open with Live Server"
```
Then navigate to `http://localhost:8080`.

---

## 📡 Hardware Integration (ESP32 / Arduino / Raspberry Pi)

### 1. Sensor Telemetry Topic
The dashboard subscribes to:
`iot/room/sensors` (or the topic specified in the Settings modal).

Send payload in JSON format:
```json
{
  "humidity": 50.25,
  "temperature": 21.40,
  "light": 315.80,
  "room": "design"
}
```

### 2. GPIO Control Topic
When you toggle switches on the dashboard, it publishes to:
`iot/room/gpio`

Example command payload published:
```json
{
  "command": "GPIO_SET",
  "pin": "FAN",
  "state": 1,
  "room": "design",
  "timestamp": "2026-09-24T17:56:46.000Z"
}
```

---

## 💻 Sample ESP32 / Arduino C++ Code

```cpp
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "broker.emqx.io";

const int PIN_FAN = 26;
const int PIN_LIGHT = 27;

WiFiClient espClient;
PubSubClient client(espClient);

void mqttCallback(char* topic, byte* message, unsigned int length) {
  StaticJsonDocument<256> doc;
  deserializeJson(doc, message, length);

  const char* pin = doc["pin"];
  int state = doc["state"];

  if (strcmp(pin, "FAN") == 0) {
    digitalWrite(PIN_FAN, state == 1 ? HIGH : LOW);
  } else if (strcmp(pin, "LIGHT") == 0) {
    digitalWrite(PIN_LIGHT, state == 1 ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_LIGHT, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqttCallback);
}

void loop() {
  if (!client.connected()) {
    while (!client.connected()) {
      if (client.connect("ESP32_Client_01")) {
        client.subscribe("iot/room/gpio");
      } else {
        delay(2000);
      }
    }
  }
  client.loop();

  // Periodically send sensor telemetry every 2 seconds
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 2000) {
    lastMsg = millis();

    StaticJsonDocument<200> doc;
    doc["humidity"] = 50.0 + random(-10, 10) / 10.0;
    doc["temperature"] = 20.2 + random(-5, 5) / 10.0;
    doc["light"] = 280.0 + random(-20, 20);

    char buffer[256];
    serializeJson(doc, buffer);
    client.publish("iot/room/sensors", buffer);
  }
}
```

---

## 🎨 File Hierarchy

```
iot-dashboard/
├── index.html        # Main dashboard HTML structure
├── style.css         # Modern dark theme styles, animations & grid layout
├── app.js            # Gauges, charts, real-time simulator & MQTT integration
└── README.md         # Documentation & hardware integration guide
```
