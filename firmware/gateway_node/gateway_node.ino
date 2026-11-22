// ============================================================================
//  Smart Laboratory Safety Monitoring System — Gateway Node (ESP32 Super Mini)
//  Role      : 433 MHz LoRa SX1278 Fallback Receiver -> Wi-Fi / MQTT / Telegram
//  Pinout    : SCK=4, MISO=5, MOSI=6, CS=7, RST=3, DIO0=10 (docs/BluePrint.md)
// ============================================================================

#include "FS.h"
#include "pins.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <LoRa.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <UniversalTelegramBot.h>
#include <Update.h>
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define FIRMWARE_VERSION 1.0f
float currentFsVersion = 1.0f;

// ─── WebSocket Bridge for Cloud MQTT (WS / WSS) ─────────────────────────────
#define WSBRIDGE_RX_SIZE 1024
class WsClientBridge : public Client {
  WebSocketsClient &_ws;
  uint8_t _buf[WSBRIDGE_RX_SIZE];
  uint32_t _head = 0, _tail = 0;
  bool _connected = false;
  uint32_t _avail() const { return _head - _tail; }

public:
  explicit WsClientBridge(WebSocketsClient &ws) : _ws(ws) {}
  void push(const uint8_t *d, size_t n) {
    for (size_t i = 0; i < n && _avail() < WSBRIDGE_RX_SIZE - 1; i++)
      _buf[(_head + i) % WSBRIDGE_RX_SIZE] = d[i];
    _head += n;
  }
  void setConnected(bool c) {
    _connected = c;
    if (!c) {
      _head = _tail = 0;
    }
  }
  int connect(IPAddress, uint16_t) override { return (int)_connected; }
  int connect(const char *, uint16_t) override { return (int)_connected; }
  size_t write(uint8_t b) override { return write(&b, 1); }
  size_t write(const uint8_t *buf, size_t sz) override {
    _ws.sendBIN(buf, sz);
    return sz;
  }
  int available() override {
    if (!_avail())
      _ws.loop();
    return (int)_avail();
  }
  int read() override {
    unsigned long t = millis();
    while (!_avail()) {
      _ws.loop();
      if (millis() - t > 3000)
        return -1;
    }
    return _buf[_tail++ % WSBRIDGE_RX_SIZE];
  }
  int read(uint8_t *buf, size_t sz) override {
    int n = 0;
    while ((size_t)n < sz) {
      int b = read();
      if (b < 0)
        break;
      buf[n++] = (uint8_t)b;
    }
    return n;
  }
  int peek() override {
    if (!_avail())
      _ws.loop();
    return _avail() ? _buf[_tail % WSBRIDGE_RX_SIZE] : -1;
  }
  void flush() override {}
  void stop() override {
    _ws.disconnect();
    setConnected(false);
  }
  uint8_t connected() override { return _connected; }
  operator bool() override { return _connected; }
};

const char *const STATE_LABELS[5] = {
    "NORMAL", "WARNING (GAS LEAK)", "CRITICAL (TOXIC LEAK)",
    "EMERGENCY (FIRE)", "SENSOR GLITCH"};

// ============================================================================
//  LORA RECEIVER & TELEMETRY TRACKING
// ============================================================================
bool loraReady = false;
uint32_t loraPacketCount = 0;
int lastLoraRssi = 0;
float lastLoraSnr = 0.0f;
unsigned long lastLoraRxMs = 0;

struct LastMainTelemetry {
  char node[32];
  uint8_t state;
  float temp;
  float humidity;
  float pressure;
  uint16_t tvoc;
  uint16_t eco2;
  bool flame;
  bool presence;
  uint16_t distance_cm;
  uint32_t uptime_s;
} lastTelemetry = {"lab_main_s3", 0,     25.0f, 55.0f, 1013.2f, 0,
                   400,           false, false, 0,     0};

uint8_t prevMainState = 0;

// ============================================================================
//  NETWORK, MQTT, AUTH & TELEGRAM CONFIGURATION
// ============================================================================
const char *AP_FALLBACK_SSID = "LabSafety-GW-Setup";
const char *AP_FALLBACK_PASS = "12345678";

char deviceName[32] = "lab_gateway_mini";
char wifiSsid[64] = "";
char wifiPass[64] = "";

struct MqttConfig {
  char mode[10];
  char proto[10];
  char host[100];
  int port;
  char path[40];
  char user[33];
  char pass[65];
} mqttCfg;

struct TelegramConfig {
  char botToken[128];
  char chatId[24];
  bool enabled;
  uint32_t alertEveryVal;
  char alertEveryUnit[12];
} tgConfig;

struct UserAccount {
  char username[30];
  char password[30];
};
UserAccount users[5];
int userCount = 0;
char activeToken[33] = "";
char activeUser[30] = "admin";

WiFiClient tcpClient;
WiFiClientSecure mqttTlsClient;
WebSocketsClient wsClient;
WsClientBridge wsBridge(wsClient);
PubSubClient mqtt(tcpClient);
WebServer server(80);

WiFiClientSecure tgSecureClient;
UniversalTelegramBot tgBot("", tgSecureClient);
bool tgInitialized = false;
QueueHandle_t tgOutQueue = NULL;
TaskHandle_t tgTaskHandle = NULL;
const unsigned long TG_POLL_MS = 2500;
unsigned long lastPeriodicTgMs = 0;

unsigned long lastMqttRetry = 0;
const unsigned long mqttRetryInterval = 5000;
bool shouldReboot = false;
volatile bool otaRunning = false;
String lastOtaStatus = "";

void applyMqttTransport();
void wsEventHandler(WStype_t type, uint8_t *payload, size_t length);
void mqttCallback(char *topic, byte *payload, unsigned int length);
void tgSend(const String &msg);

uint32_t getAlertIntervalMs() {
  if (tgConfig.alertEveryVal == 0)
    return 0;
  if (strcmp(tgConfig.alertEveryUnit, "seconds") == 0)
    return tgConfig.alertEveryVal * 1000UL;
  if (strcmp(tgConfig.alertEveryUnit, "hours") == 0)
    return tgConfig.alertEveryVal * 3600000UL;
  return tgConfig.alertEveryVal * 60000UL;
}

// ============================================================================
//  CONFIG LOAD / SAVE (LittleFS)
// ============================================================================
void loadConfig() {
  strlcpy(deviceName, "lab_gateway_mini", sizeof(deviceName));
  wifiSsid[0] = '\0';
  wifiPass[0] = '\0';
  strlcpy(mqttCfg.mode, "local", sizeof(mqttCfg.mode));
  strlcpy(mqttCfg.proto, "tcp", sizeof(mqttCfg.proto));
  mqttCfg.host[0] = '\0';
  mqttCfg.port = 1883;
  strlcpy(mqttCfg.path, "/mqtt", sizeof(mqttCfg.path));
  mqttCfg.user[0] = '\0';
  mqttCfg.pass[0] = '\0';

  const char *cfgPath = LittleFS.exists("/config.json")
                            ? "/config.json"
                            : (LittleFS.exists("/config.example.json")
                                   ? "/config.example.json"
                                   : NULL);
  if (!cfgPath)
    return;

  File f = LittleFS.open(cfgPath, "r");
  if (!f)
    return;
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  DynamicJsonDocument doc(2048);
#endif
  if (!deserializeJson(doc, f)) {
    strlcpy(deviceName, doc["deviceName"] | "lab_gateway_mini",
            sizeof(deviceName));
    strlcpy(wifiSsid, doc["wifiSsid"] | "", sizeof(wifiSsid));
    strlcpy(wifiPass, doc["wifiPass"] | "", sizeof(wifiPass));
    strlcpy(mqttCfg.mode, doc["mqttMode"] | "local", sizeof(mqttCfg.mode));
    strlcpy(mqttCfg.proto, doc["mqttProto"] | "tcp", sizeof(mqttCfg.proto));
    strlcpy(mqttCfg.host, doc["mqttHost"] | "", sizeof(mqttCfg.host));
    mqttCfg.port = doc["mqttPort"] | 1883;
    strlcpy(mqttCfg.path, doc["mqttPath"] | "/mqtt", sizeof(mqttCfg.path));
    strlcpy(mqttCfg.user, doc["mqttUser"] | "", sizeof(mqttCfg.user));
    strlcpy(mqttCfg.pass, doc["mqttPass"] | "", sizeof(mqttCfg.pass));
  }
  f.close();
}

void saveConfig() {
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  DynamicJsonDocument doc(2048);
#endif
  doc["deviceName"] = deviceName;
  doc["wifiSsid"] = wifiSsid;
  doc["wifiPass"] = wifiPass;
  doc["mqttMode"] = mqttCfg.mode;
  doc["mqttProto"] = mqttCfg.proto;
  doc["mqttHost"] = mqttCfg.host;
  doc["mqttPort"] = mqttCfg.port;
  doc["mqttPath"] = mqttCfg.path;
  doc["mqttUser"] = mqttCfg.user;
  doc["mqttPass"] = mqttCfg.pass;

  File f = LittleFS.open("/config.json", "w");
  if (f) {
    serializeJsonPretty(doc, f);
    f.close();
  }
}

void loadUsers() {
  userCount = 1;
  strlcpy(users[0].username, "admin", sizeof(users[0].username));
  strlcpy(users[0].password, "admin123", sizeof(users[0].password));

  if (!LittleFS.exists("/users.json"))
    return;
  File f = LittleFS.open("/users.json", "r");
  if (!f)
    return;
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<512> doc;
#endif
  if (!deserializeJson(doc, f) && doc.is<JsonArray>()) {
    int idx = 0;
    for (JsonObject u : doc.as<JsonArray>()) {
      if (idx >= 5)
        break;
      const char *uname = u["username"] | "";
      const char *upass = u["password"] | "";
      if (strlen(uname) > 0) {
        strlcpy(users[idx].username, uname, sizeof(users[idx].username));
        strlcpy(users[idx].password, upass, sizeof(users[idx].password));
        idx++;
      }
    }
    if (idx > 0)
      userCount = idx;
  }
  f.close();
}

void saveUsers() {
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();
  for (int i = 0; i < userCount; i++) {
    JsonObject u = arr.add<JsonObject>();
    u["username"] = users[i].username;
    u["password"] = users[i].password;
  }
#else
  StaticJsonDocument<512> doc;
  JsonArray arr = doc.to<JsonArray>();
  for (int i = 0; i < userCount; i++) {
    JsonObject u = arr.createNestedObject();
    u["username"] = users[i].username;
    u["password"] = users[i].password;
  }
#endif
  File f = LittleFS.open("/users.json", "w");
  if (f) {
    serializeJsonPretty(doc, f);
    f.close();
  }
}

void loadTelegramConfig() {
  memset(&tgConfig, 0, sizeof(tgConfig));
  tgConfig.alertEveryVal = 0;
  strlcpy(tgConfig.alertEveryUnit, "minutes", sizeof(tgConfig.alertEveryUnit));

  if (!LittleFS.exists("/telegram.json"))
    return;
  File f = LittleFS.open("/telegram.json", "r");
  if (!f)
    return;
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<512> doc;
#endif
  if (!deserializeJson(doc, f)) {
    strlcpy(tgConfig.botToken, doc["botToken"] | "", sizeof(tgConfig.botToken));
    strlcpy(tgConfig.chatId, doc["chatId"] | "", sizeof(tgConfig.chatId));
    tgConfig.enabled = doc["enabled"] | false;
    tgConfig.alertEveryVal = doc["alertEveryVal"] | 0;
    strlcpy(tgConfig.alertEveryUnit, doc["alertEveryUnit"] | "minutes",
            sizeof(tgConfig.alertEveryUnit));
  }
  f.close();
}

void saveTelegramConfig() {
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<512> doc;
#endif
  doc["botToken"] = tgConfig.botToken;
  doc["chatId"] = tgConfig.chatId;
  doc["enabled"] = tgConfig.enabled;
  doc["alertEveryVal"] = tgConfig.alertEveryVal;
  doc["alertEveryUnit"] = tgConfig.alertEveryUnit;

  File f = LittleFS.open("/telegram.json", "w");
  if (f) {
    serializeJsonPretty(doc, f);
    f.close();
  }
}

// ============================================================================
//  BACKUP / RESTORE CONFIGS ACROSS LITTLEFS OTA
// ============================================================================
String bk_config, bk_telegram, bk_users;

void backupConfigs() {
  bk_config = "";
  bk_telegram = "";
  bk_users = "";
  File f;
  if ((f = LittleFS.open("/config.json", "r"))) {
    bk_config = f.readString();
    f.close();
  }
  if ((f = LittleFS.open("/telegram.json", "r"))) {
    bk_telegram = f.readString();
    f.close();
  }
  if ((f = LittleFS.open("/users.json", "r"))) {
    bk_users = f.readString();
    f.close();
  }
}

void restoreConfigs() {
  LittleFS.begin(true);
  File f;
  if (bk_config.length() > 0 && (f = LittleFS.open("/config.json", "w"))) {
    f.print(bk_config);
    f.close();
  }
  if (bk_telegram.length() > 0 && (f = LittleFS.open("/telegram.json", "w"))) {
    f.print(bk_telegram);
    f.close();
  }
  if (bk_users.length() > 0 && (f = LittleFS.open("/users.json", "w"))) {
    f.print(bk_users);
    f.close();
  }
}

// ============================================================================
//  TELEGRAM BOT LOGIC
// ============================================================================
void initTelegramBot() {
  tgInitialized = false;
  if (strlen(tgConfig.botToken) < 10)
    return;
  tgSecureClient.setInsecure();
  tgSecureClient.setTimeout(8);
  tgBot = UniversalTelegramBot(tgConfig.botToken, tgSecureClient);
  tgInitialized = true;
}

void tgSend(const String &msg) {
  if (!tgInitialized || !tgConfig.enabled)
    return;
  if (!strlen(tgConfig.chatId) || tgOutQueue == NULL)
    return;
  char buf[384] = {0};
  msg.substring(0, 382).toCharArray(buf, sizeof(buf));
  xQueueSend(tgOutQueue, buf, pdMS_TO_TICKS(0));
}

String buildStatusMsg() {
  String ip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString()
                                              : WiFi.softAPIP().toString();
  uint8_t st = lastTelemetry.state <= 4 ? lastTelemetry.state : 0;
  char buf[420];
  snprintf(buf, sizeof(buf),
           "*LoRa Gateway Status*\n"
           "Gateway IP: `%s`\n"
           "LoRa 433MHz Radio: *%s*\n"
           "Packets Received: `%lu`\n"
           "Last RSSI / SNR: `%d dBm` / `%.1f dB`\n"
           "Last Main Node State: *State %u — %s*",
           ip.c_str(), loraReady ? "ONLINE (RX)" : "NOT DETECTED",
           (unsigned long)loraPacketCount, lastLoraRssi, lastLoraSnr, st,
           STATE_LABELS[st]);
  return String(buf);
}

String buildSensorsMsg() {
  if (loraPacketCount == 0) {
    return "*No LoRa packets received from Main Node yet.*";
  }
  unsigned long ageS = (millis() - lastLoraRxMs) / 1000UL;
  char buf[420];
  snprintf(buf, sizeof(buf),
           "*Last LoRa Telemetry (%lus ago)*\n"
           "Node: `%s`\n"
           "Temperature: `%.1f C`\n"
           "Humidity: `%.1f %%`\n"
           "Pressure: `%.1f hPa`\n"
           "TVOC: `%u ppb`\n"
           "eCO2: `%u ppm`\n"
           "IR Flame: *%s*\n"
           "Human Presence: *%s* (`%u cm`)",
           ageS, lastTelemetry.node, lastTelemetry.temp, lastTelemetry.humidity,
           lastTelemetry.pressure, lastTelemetry.tvoc, lastTelemetry.eco2,
           lastTelemetry.flame ? "DETECTED" : "Clear",
           lastTelemetry.presence ? "Detected" : "None",
           lastTelemetry.distance_cm);
  return String(buf);
}

String buildHealthMsg() {
  unsigned long upS = millis() / 1000, upM = upS / 60, upH = upM / 60,
                upD = upH / 24;
  char up[32], buf[320];
  if (upD > 0)
    snprintf(up, sizeof(up), "%lud %02lu:%02lu:%02lu", upD, upH % 24, upM % 60,
             upS % 60);
  else
    snprintf(up, sizeof(up), "%02lu:%02lu:%02lu", upH, upM % 60, upS % 60);

  String ip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString()
                                              : WiFi.softAPIP().toString();
  snprintf(buf, sizeof(buf),
           "*ESP32 Super Mini Gateway Health*\n"
           "Chip Temp: `%.1f C`\n"
           "Free Heap: `%lu KB`\n"
           "WiFi RSSI: `%d dBm`\n"
           "IP: `%s`\n"
           "CPU: `%u MHz`\n"
           "Uptime: `%s`\n"
           "FW: `v%.1f` | FS: `v%.1f`",
           temperatureRead(), (unsigned long)(ESP.getFreeHeap() / 1024),
           (int)WiFi.RSSI(), ip.c_str(), getCpuFrequencyMhz(), up,
           FIRMWARE_VERSION, currentFsVersion);
  return String(buf);
}

void handleTelegramMessage(const telegramMessage &m) {
  if (strlen(tgConfig.chatId) > 0 && m.chat_id != String(tgConfig.chatId)) {
    tgBot.sendMessage(m.chat_id, "Unauthorized Chat ID.", "");
    return;
  }
  String text = m.text;
  text.trim();
  if (text == "/help" || text == "/start") {
    tgBot.sendMessage(
        m.chat_id,
        "*Smart Lab LoRa Gateway Commands*\n\n"
        "/status — Gateway radio status, packet count & last Main state\n"
        "/sensors — Last sensor payload received via 433 MHz LoRa\n"
        "/health — Gateway chip temp, heap, WiFi RSSI, IP & uptime\n"
        "/help — Show command list",
        "Markdown");
  } else if (text == "/status") {
    tgBot.sendMessage(m.chat_id, buildStatusMsg(), "Markdown");
  } else if (text == "/sensors") {
    tgBot.sendMessage(m.chat_id, buildSensorsMsg(), "Markdown");
  } else if (text == "/health") {
    tgBot.sendMessage(m.chat_id, buildHealthMsg(), "Markdown");
  } else {
    tgBot.sendMessage(m.chat_id, "Unknown command. Send /help for the list.",
                      "");
  }
}

void telegramTask(void *pv) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(TG_POLL_MS));
    if (!tgInitialized || !tgConfig.enabled)
      continue;
    if (WiFi.status() != WL_CONNECTED)
      continue;

    char outMsg[384];
    while (xQueueReceive(tgOutQueue, outMsg, 0) == pdTRUE) {
      tgBot.sendMessage(tgConfig.chatId, outMsg, "Markdown");
      vTaskDelay(pdMS_TO_TICKS(150));
    }

    int n = tgBot.getUpdates(tgBot.last_message_received + 1);
    for (int i = 0; i < n; i++) {
      handleTelegramMessage(tgBot.messages[i]);
    }
  }
}

// ============================================================================
//  MQTT & LORA PACKET FORWARDING
// ============================================================================
void applyMqttTransport() {
  mqtt.disconnect();
  wsBridge.setConnected(false);
  wsClient.disconnect();
  if (mqttCfg.host[0] == '\0' || mqttCfg.port <= 0)
    return;

  if (strcmp(mqttCfg.mode, "cloud") == 0) {
    mqtt.setClient(wsBridge);
    if (strcmp(mqttCfg.proto, "wss") == 0)
      wsClient.beginSSL(mqttCfg.host, mqttCfg.port, mqttCfg.path, "", "mqtt");
    else
      wsClient.begin(mqttCfg.host, mqttCfg.port, mqttCfg.path, "mqtt");
    wsClient.onEvent(wsEventHandler);
    wsClient.setReconnectInterval(5000);
  } else {
    if (strcmp(mqttCfg.proto, "ssl") == 0) {
      mqttTlsClient.setInsecure();
      mqtt.setClient(mqttTlsClient);
    } else {
      mqtt.setClient(tcpClient);
    }
  }
  mqtt.setServer(mqttCfg.host, mqttCfg.port);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512);
  mqtt.setKeepAlive(15);
  lastMqttRetry = 0;
}

void wsEventHandler(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
  case WStype_CONNECTED:
    wsBridge.setConnected(true);
    break;
  case WStype_DISCONNECTED:
    wsBridge.setConnected(false);
    break;
  case WStype_BIN:
    wsBridge.push(payload, length);
    break;
  default:
    break;
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {}

void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED || mqttCfg.host[0] == '\0')
    return;
  if (strcmp(mqttCfg.mode, "cloud") == 0 && !wsBridge.connected())
    return;

  if (strlen(mqttCfg.user) > 0)
    mqtt.connect(deviceName, mqttCfg.user, mqttCfg.pass);
  else
    mqtt.connect(deviceName);
}

void checkAndForwardLoRaPackets() {
  if (!loraReady)
    return;

  int packetSize = LoRa.parsePacket();
  if (packetSize <= 0)
    return;

  // Blink onboard LED briefly when a LoRa packet arrives
  digitalWrite(PIN_STATUS_LED, LED_ON_LEVEL);

  String incoming = "";
  while (LoRa.available() && incoming.length() < 480) {
    incoming += (char)LoRa.read();
  }

  lastLoraRssi = LoRa.packetRssi();
  lastLoraSnr = LoRa.packetSnr();
  lastLoraRxMs = millis();
  loraPacketCount++;

#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<512> doc;
#endif
  if (!deserializeJson(doc, incoming)) {
    strlcpy(lastTelemetry.node, doc["node"] | "lab_main_s3",
            sizeof(lastTelemetry.node));
    lastTelemetry.state = doc["state"] | 0;
    lastTelemetry.temp = doc["temp"] | 0.0f;
    lastTelemetry.humidity = doc["humidity"] | 0.0f;
    lastTelemetry.pressure = doc["pressure"] | 0.0f;
    lastTelemetry.tvoc = doc["tvoc"] | 0;
    lastTelemetry.eco2 = doc["eco2"] | 0;
    lastTelemetry.flame = (doc["flame"] | 0) != 0;
    lastTelemetry.presence = doc["presence"] | false;
    lastTelemetry.distance_cm = doc["distance_cm"] | 0;
    lastTelemetry.uptime_s = doc["uptime_s"] | 0;

    // Enrich JSON with LoRa gateway metadata before forwarding to MQTT
    doc["via"] = "lora_gateway";
    doc["lora_rssi"] = lastLoraRssi;

    String outJson;
    serializeJson(doc, outJson);

    // Forward to the Main Node's MQTT telemetry topic (e.g. lab_main_s3/telemetry)
    String topic = String(lastTelemetry.node) + "/telemetry";
    if (WiFi.status() == WL_CONNECTED && mqtt.connected()) {
      mqtt.publish(topic.c_str(), outJson.c_str());
    }

    // Trigger Telegram fallback alert if Main Node state transitioned to an emergency
    if (lastTelemetry.state != prevMainState && lastTelemetry.state != 0 &&
        lastTelemetry.state <= 4) {
      char alertBuf[360];
      snprintf(alertBuf, sizeof(alertBuf),
               "*[LoRa 433MHz FALLBACK ALERT]*\n"
               "State: *State %u — %s*\n"
               "Temp: `%.1f C` | Hum: `%.1f %%`\n"
               "TVOC: `%u ppb` | eCO2: `%u ppm`\n"
               "Flame: *%s*\n"
               "Presence: *%s* (`%u cm`)\n"
               "LoRa RSSI: `%d dBm`",
               lastTelemetry.state, STATE_LABELS[lastTelemetry.state],
               lastTelemetry.temp, lastTelemetry.humidity, lastTelemetry.tvoc,
               lastTelemetry.eco2, lastTelemetry.flame ? "DETECTED" : "No",
               lastTelemetry.presence ? "Yes" : "No",
               lastTelemetry.distance_cm, lastLoraRssi);
      tgSend(String(alertBuf));
    }
    prevMainState = lastTelemetry.state;
  } else {
    // Raw non-JSON fallback forward
    if (WiFi.status() == WL_CONNECTED && mqtt.connected()) {
      mqtt.publish("lab_main_s3/telemetry", incoming.c_str());
    }
  }

  digitalWrite(PIN_STATUS_LED, LED_OFF_LEVEL);
}

// ============================================================================
//  WEB SERVER AUTHENTICATION & HTTP ROUTE HANDLERS
// ============================================================================
bool checkCredentials(const String &u, const String &p) {
  for (int i = 0; i < userCount; i++) {
    if (strcmp(users[i].username, u.c_str()) == 0 &&
        strcmp(users[i].password, p.c_str()) == 0) {
      strlcpy(activeUser, users[i].username, sizeof(activeUser));
      return true;
    }
  }
  return false;
}

void generateToken(char *buf) {
  randomSeed(micros() ^ millis());
  for (int i = 0; i < 32; i++) {
    int r = random(16);
    buf[i] = r < 10 ? '0' + r : 'a' + r - 10;
  }
  buf[32] = '\0';
}

bool isAuthenticated() {
  if (activeToken[0] == '\0')
    return false;
  String c = server.header("Cookie");
  int idx = c.indexOf("session=");
  if (idx == -1)
    return false;
  String v = c.substring(idx + 8);
  int e = v.indexOf(';');
  if (e != -1)
    v = v.substring(0, e);
  v.trim();
  return strcmp(activeToken, v.c_str()) == 0;
}

bool requireAuth() {
  if (!isAuthenticated()) {
    server.sendHeader("Location", "/login");
    server.send(302);
    return false;
  }
  return true;
}

void serveStatic(const char *path, const char *mime, bool noCache = false) {
  String gzPath = String(path) + ".gz";
  File f;
  if (LittleFS.exists(gzPath)) {
    f = LittleFS.open(gzPath, "r");
  } else if (LittleFS.exists(path)) {
    f = LittleFS.open(path, "r");
  }
  if (!f) {
    server.send(404, "text/plain", "File Not Found on LittleFS");
    return;
  }
  if (!noCache)
    server.sendHeader("Cache-Control", "public, max-age=86400");
  else
    server.sendHeader("Cache-Control", "no-cache");
  server.streamFile(f, mime);
  f.close();
}

void handleLogin() {
  if (server.method() == HTTP_GET) {
    serveStatic("/login.html", "text/html", true);
  } else {
    if (checkCredentials(server.arg("username"), server.arg("password"))) {
      generateToken(activeToken);
      server.sendHeader("Set-Cookie",
                        String("session=") + activeToken + "; Path=/");
      server.sendHeader("Location", "/");
      server.send(302);
    } else {
      server.sendHeader("Location", "/login?error=1");
      server.send(302);
    }
  }
}

void handleLogout() {
  activeToken[0] = '\0';
  server.sendHeader("Set-Cookie", "session=; Path=/; Max-Age=0");
  server.sendHeader("Location", "/login");
  server.send(302);
}

void handleStatus() {
  if (!requireAuth())
    return;
  bool staConn = (WiFi.status() == WL_CONNECTED);
  String ip =
      staConn ? WiFi.localIP().toString() : WiFi.softAPIP().toString();
  String net =
      staConn ? ("WiFi (" + WiFi.SSID() + ")") : String("AP Fallback");

#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<384> doc;
#endif
  doc["ip"] = ip;
  doc["network"] = net;
  doc["wifiConnected"] = staConn;
  doc["ssid"] = staConn ? WiFi.SSID() : "";
  doc["apSsid"] = AP_FALLBACK_SSID;
  doc["configuredSsid"] = wifiSsid;
  doc["currentUser"] = activeUser;
  doc["state"] = lastTelemetry.state;
  doc["loraReady"] = loraReady;
  doc["loraCount"] = loraPacketCount;
  doc["loraRssi"] = lastLoraRssi;

  String out;
  serializeJson(doc, out);
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "application/json", out);
}

void handleGetHealth() {
  if (!requireAuth())
    return;
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<256> doc;
#endif
  doc["temp"] = temperatureRead();
  doc["freeHeap"] = (uint32_t)ESP.getFreeHeap();
  doc["rssi"] = (WiFi.status() == WL_CONNECTED) ? (int)WiFi.RSSI() : 0;
  doc["cpuFreq"] = getCpuFrequencyMhz();
  doc["uptime"] = millis();
  doc["fw_ver"] = FIRMWARE_VERSION;
  doc["fs_ver"] = currentFsVersion;
  doc["otaStatus"] = lastOtaStatus;

  String out;
  serializeJson(doc, out);
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "application/json", out);
}

void handleScanWifi() {
  if (!requireAuth())
    return;
  int n = WiFi.scanNetworks();
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();
  for (int i = 0; i < n; i++) {
    JsonObject obj = arr.add<JsonObject>();
    obj["ssid"] = WiFi.SSID(i);
    obj["rssi"] = WiFi.RSSI(i);
  }
#else
  DynamicJsonDocument doc(2048);
  JsonArray arr = doc.to<JsonArray>();
  for (int i = 0; i < n; i++) {
    JsonObject obj = arr.createNestedObject();
    obj["ssid"] = WiFi.SSID(i);
    obj["rssi"] = WiFi.RSSI(i);
  }
#endif
  WiFi.scanDelete();
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleSaveWifi() {
  if (!requireAuth())
    return;
  String newSsid = server.arg("ssid");
  String newPass = server.arg("pass");
  newSsid.trim();
  if (newSsid.length() == 0) {
    server.send(400, "text/plain", "SSID cannot be empty");
    return;
  }
  strlcpy(wifiSsid, newSsid.c_str(), sizeof(wifiSsid));
  if (newPass.length() > 0) {
    strlcpy(wifiPass, newPass.c_str(), sizeof(wifiPass));
  }
  saveConfig();
  server.send(200, "text/plain", "Wi-Fi credentials saved! Reconnecting...");
  delay(200);
  WiFi.disconnect();
  WiFi.begin(wifiSsid, wifiPass);
}

void handleSaveAuth() {
  if (!requireAuth())
    return;
  String uname = server.arg("username");
  String curPass = server.arg("currentPass");
  String newPass = server.arg("newPass");
  uname.trim();

  if (uname.length() == 0 || newPass.length() == 0) {
    server.send(400, "text/plain", "Username and new password are required");
    return;
  }

  int targetIdx = -1;
  for (int i = 0; i < userCount; i++) {
    if (strcmp(users[i].password, curPass.c_str()) == 0) {
      targetIdx = i;
      break;
    }
  }
  if (targetIdx == -1) {
    server.send(403, "text/plain", "Current password is incorrect");
    return;
  }

  strlcpy(users[targetIdx].username, uname.c_str(),
          sizeof(users[targetIdx].username));
  strlcpy(users[targetIdx].password, newPass.c_str(),
          sizeof(users[targetIdx].password));
  strlcpy(activeUser, uname.c_str(), sizeof(activeUser));
  saveUsers();
  server.send(200, "text/plain", "OK");
}

void handleGetConfig() {
  if (!requireAuth())
    return;
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<512> doc;
#endif
  doc["deviceName"] = deviceName;
  doc["mqttMode"] = mqttCfg.mode;
  doc["mqttProto"] = mqttCfg.proto;
  doc["mqttHost"] = mqttCfg.host;
  doc["mqttPort"] = mqttCfg.port;
  doc["mqttPath"] = mqttCfg.path;
  doc["mqttUser"] = mqttCfg.user;
  doc["mqttPass"] = mqttCfg.pass;
  doc["mqttConnected"] = mqtt.connected();

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleSaveMqtt() {
  if (!requireAuth())
    return;
  String dev = server.arg("deviceName");
  String mode = server.arg("mqttMode");
  String proto = server.arg("mqttProto");
  String host = server.arg("mqttHost");
  String port = server.arg("mqttPort");
  String path = server.arg("mqttPath");
  String user = server.arg("mqttUser");
  String pass = server.arg("mqttPass");

  if (dev.length() > 0)
    strlcpy(deviceName, dev.c_str(), sizeof(deviceName));
  if (mode.length() > 0) {
    strlcpy(mqttCfg.mode, mode.c_str(), sizeof(mqttCfg.mode));
    strlcpy(mqttCfg.proto, proto.c_str(), sizeof(mqttCfg.proto));
    strlcpy(mqttCfg.host, host.c_str(), sizeof(mqttCfg.host));
    if (port.length() > 0)
      mqttCfg.port = port.toInt();
    strlcpy(mqttCfg.path, path.length() ? path.c_str() : "/mqtt",
            sizeof(mqttCfg.path));
    strlcpy(mqttCfg.user, user.c_str(), sizeof(mqttCfg.user));
    strlcpy(mqttCfg.pass, pass.c_str(), sizeof(mqttCfg.pass));
  }
  saveConfig();
  applyMqttTransport();
  server.send(200, "text/plain", "OK");
}

void handleGetTelegramConfig() {
  if (!requireAuth())
    return;
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<384> doc;
#endif
  bool ht = strlen(tgConfig.botToken) > 8;
  doc["tokenMasked"] =
      ht ? (String(tgConfig.botToken).substring(0, 8) + "........") : "";
  doc["hasToken"] = ht;
  doc["chatId"] = tgConfig.chatId;
  doc["enabled"] = tgConfig.enabled;
  doc["alertEveryVal"] = tgConfig.alertEveryVal;
  doc["alertEveryUnit"] = tgConfig.alertEveryUnit;
  doc["botReady"] = tgInitialized;

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleSaveTelegramConfig() {
  if (!requireAuth())
    return;
  String token = server.arg("botToken");
  String chatId = server.arg("chatId");
  String en = server.arg("enabled");
  String everyVal = server.arg("alertEveryVal");
  String everyUnit = server.arg("alertEveryUnit");

  if (token.length() > 8 && !token.endsWith("........"))
    strlcpy(tgConfig.botToken, token.c_str(), sizeof(tgConfig.botToken));
  if (chatId.length() > 0)
    strlcpy(tgConfig.chatId, chatId.c_str(), sizeof(tgConfig.chatId));
  tgConfig.enabled = (en == "1" || en == "true");
  if (everyVal.length() > 0)
    tgConfig.alertEveryVal = constrain(everyVal.toInt(), 0, 9999);
  if (everyUnit == "seconds" || everyUnit == "minutes" || everyUnit == "hours")
    strlcpy(tgConfig.alertEveryUnit, everyUnit.c_str(),
            sizeof(tgConfig.alertEveryUnit));

  saveTelegramConfig();
  initTelegramBot();
  lastPeriodicTgMs = millis();
  server.send(200, "text/plain", "OK");
}

void handleTelegramTest() {
  if (!requireAuth())
    return;
  if (!tgInitialized || !tgConfig.enabled || !strlen(tgConfig.chatId)) {
    server.send(400, "text/plain", "Bot not configured or disabled");
    return;
  }
  saveConfig();
  saveTelegramConfig();
  tgSend("*Test OK!* Smart Laboratory LoRa Gateway Bot is online.\n\n" +
         buildStatusMsg());
  server.send(200, "text/plain", "Queued — check Telegram in ~3 seconds");
}

void handleCheckUpdate() {
  if (!requireAuth())
    return;
  lastOtaStatus = "System is already up to date (Firmware v" +
                  String(FIRMWARE_VERSION, 1) + ", LittleFS v" +
                  String(currentFsVersion, 1) + "). No update applied.";
  server.send(200, "text/plain", lastOtaStatus);
}

// ============================================================================
//  SETUP
// ============================================================================
void setup() {
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, LED_OFF_LEVEL);

  // 1. Mount LittleFS & Load Configurations
  if (LittleFS.begin(true)) {
    if (LittleFS.exists("/fs_version.txt")) {
      File f = LittleFS.open("/fs_version.txt", "r");
      if (f) {
        currentFsVersion = f.readString().toFloat();
        f.close();
      }
    }
  }

  loadConfig();
  loadUsers();
  loadTelegramConfig();

  // 2. Initialize LoRa SX1278 (433 MHz) using BluePrint.md Custom SPI Pins
  // SCK = GPIO 4, MISO = GPIO 5, MOSI = GPIO 6, CS = GPIO 7, RST = GPIO 3, DIO0 = GPIO 10
  SPI.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI, PIN_LORA_CS);
  LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  if (LoRa.begin(LORA_FREQUENCY)) {
    loraReady = true;
  }

  // 3. Start Wi-Fi (AP + STA so portal is always reachable)
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_FALLBACK_SSID, AP_FALLBACK_PASS);

  if (strlen(wifiSsid) > 0) {
    WiFi.begin(wifiSsid, wifiPass);
    WiFi.setSleep(false);
    unsigned long startWait = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startWait < 10000) {
      delay(250);
    }
  }

  // 4. Initialize Telegram Bot Task (Core 0 — safe on single-core ESP32-C3 & dual-core S3)
  tgOutQueue = xQueueCreate(8, 384);
  initTelegramBot();
  xTaskCreatePinnedToCore(telegramTask, "TGTask", 12288, NULL, 1, &tgTaskHandle,
                          0);

  applyMqttTransport();

  // 5. Register HTTP Server Routes
  const char *headerKeys[] = {"Cookie"};
  server.collectHeaders(headerKeys, 1);

  server.on("/login", handleLogin);
  server.on("/logout", handleLogout);
  server.on("/style.css", HTTP_GET,
            []() { serveStatic("/style.css", "text/css"); });
  server.on("/app.js", HTTP_GET,
            []() { serveStatic("/app.js", "application/javascript"); });

  server.on("/", HTTP_GET, []() {
    if (!requireAuth())
      return;
    serveStatic("/index.html", "text/html", true);
  });
  server.on("/iot", HTTP_GET, []() {
    if (!requireAuth())
      return;
    serveStatic("/iot.html", "text/html", true);
  });
  server.on("/telegram", HTTP_GET, []() {
    if (!requireAuth())
      return;
    serveStatic("/telegram.html", "text/html", true);
  });
  server.on("/update", HTTP_GET, []() {
    if (!requireAuth())
      return;
    serveStatic("/update.html", "text/html", true);
  });

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/getHealth", HTTP_GET, handleGetHealth);
  server.on("/scanWifi", HTTP_GET, handleScanWifi);
  server.on("/saveWifi", HTTP_POST, handleSaveWifi);
  server.on("/saveAuth", HTTP_POST, handleSaveAuth);
  server.on("/getConfig", HTTP_GET, handleGetConfig);
  server.on("/save", HTTP_POST, handleSaveMqtt);
  server.on("/getTelegramConfig", HTTP_GET, handleGetTelegramConfig);
  server.on("/saveTelegramConfig", HTTP_POST, handleSaveTelegramConfig);
  server.on("/telegramTest", HTTP_POST, handleTelegramTest);
  server.on("/checkUpdate", HTTP_POST, handleCheckUpdate);

  // Manual Firmware OTA Upload
  server.on(
      "/update/firmware", HTTP_POST,
      []() {
        if (!isAuthenticated()) {
          server.send(401, "text/plain", "Unauthorized");
          return;
        }
        server.sendHeader("Connection", "close");
        if (Update.hasError()) {
          server.send(500, "text/plain", "Firmware Update Failed");
        } else {
          server.send(200, "text/plain", "OK");
          shouldReboot = true;
        }
      },
      []() {
        if (!isAuthenticated())
          return;
        HTTPUpload &upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
          Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH);
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          Update.write(upload.buf, upload.currentSize);
        } else if (upload.status == UPLOAD_FILE_END) {
          Update.end(true);
        }
      });

  // Manual LittleFS OTA Upload
  server.on(
      "/update/filesystem", HTTP_POST,
      []() {
        if (!isAuthenticated()) {
          server.send(401, "text/plain", "Unauthorized");
          return;
        }
        server.sendHeader("Connection", "close");
        if (Update.hasError()) {
          server.send(500, "text/plain", "Filesystem Update Failed");
        } else {
          server.send(200, "text/plain", "OK");
          shouldReboot = true;
        }
      },
      []() {
        if (!isAuthenticated())
          return;
        HTTPUpload &upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
          backupConfigs();
          Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS);
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          Update.write(upload.buf, upload.currentSize);
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) {
            restoreConfigs();
          }
        } else if (upload.status == UPLOAD_FILE_ABORTED) {
          Update.end();
        }
      });

  ArduinoOTA.setHostname(deviceName);
  ArduinoOTA.onStart([]() {
    if (ArduinoOTA.getCommand() == U_SPIFFS) {
      backupConfigs();
    }
  });
  ArduinoOTA.onEnd([]() {
    if (ArduinoOTA.getCommand() == U_SPIFFS) {
      restoreConfigs();
    }
  });
  ArduinoOTA.begin();
  server.begin();

  String ip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString()
                                              : WiFi.softAPIP().toString();
  tgSend("*Smart Lab LoRa Gateway Online*\nIP: `" + ip + "`\nLoRa 433MHz: `" +
         String(loraReady ? "OK" : "NOT DETECTED") + "`\nFW: `v" +
         String(FIRMWARE_VERSION, 1) + "` | FS: `v" +
         String(currentFsVersion, 1) + "`");
}

// ============================================================================
//  LOOP
// ============================================================================
void loop() {
  if (shouldReboot) {
    delay(500);
    ESP.restart();
  }

  server.handleClient();
  ArduinoOTA.handle();

  if (strcmp(mqttCfg.mode, "cloud") == 0)
    wsClient.loop();
  mqtt.loop();

  unsigned long now = millis();

  if (WiFi.status() == WL_CONNECTED && !mqtt.connected() &&
      mqttCfg.host[0] != '\0') {
    if (now - lastMqttRetry > mqttRetryInterval) {
      lastMqttRetry = now;
      reconnectMQTT();
    }
  }

  // Continuously check for incoming 433 MHz LoRa fallback packets from Main Node
  checkAndForwardLoRaPackets();

  // Periodic Telegram Gateway Report ("Alert Every" user-configured timer)
  uint32_t alertIntervalMs = getAlertIntervalMs();
  if (alertIntervalMs > 0 && tgInitialized && tgConfig.enabled) {
    if (now - lastPeriodicTgMs >= alertIntervalMs) {
      lastPeriodicTgMs = now;
      tgSend("*Periodic LoRa Gateway Report*\n\n" + buildStatusMsg());
    }
  }
}
