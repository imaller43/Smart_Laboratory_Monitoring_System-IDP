// ============================================================================
//  Smart Laboratory Safety Monitoring System — Main Node (ESP32-S3-N16R8)
//  Toolchain : Arduino IDE 2.3.11 (Espressif esp32 Core 3.x)
//  Board     : ESP32S3 Dev Module (16MB Flash QIO, OPI PSRAM, Custom Partition)
//  Reference : docs/BluePrint.md, docs/hardware.md, docs/firmware.md
//              & ESP32_GUI_SIB-1.0 LittleFS Web GUI Architecture
// ============================================================================

#include "FS.h"
#include "pins.h"
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SGP30.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <HardwareSerial.h>
#include <LittleFS.h>
#include <LoRa.h>
#include <MyLD2410.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <UniversalTelegramBot.h>
#include <Update.h>
#include <WebServer.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>

#define FIRMWARE_VERSION 1.1f
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

// ============================================================================
//  SAFETY STATES & SHARED TELEMETRY TYPES
// ============================================================================
enum SafetyState : uint8_t {
  STATE_NORMAL = 0,    // TVOC < 500 && Temp < 32C && Flame == 0
  STATE_WARNING = 1,   // Minor Chemical Leak: TVOC >= 500 && Temp < 32C && Flame == 0
  STATE_CRITICAL = 2,  // Severe Toxic Leak: TVOC > 1500 && Flame == 0
  STATE_EMERGENCY = 3, // Confirmed Fire: Flame == 1 && (Temp > 45C || rapid rise)
  STATE_GLITCH = 4     // Sensor Glitch: Flame == 1 && Temp < 30C && TVOC < 250
};

const char *const STATE_LABELS[5] = {
    "NORMAL", "WARNING (GAS LEAK)", "CRITICAL (TOXIC LEAK)",
    "EMERGENCY (FIRE)", "SENSOR GLITCH"};

struct SensorSnapshot {
  float temp;
  float humidity;
  float pressure;
  uint16_t tvoc;
  uint16_t eco2;
  bool flame;
  bool presence;
  uint16_t distance_cm;
  SafetyState state;
  bool fanOn;
  bool atomizerOn;
  bool buzzerOn;
};

struct TelemetryPacket {
  float temp;
  float humidity;
  float pressure;
  uint16_t tvoc;
  uint16_t eco2;
  bool flame;
  bool presence;
  uint16_t distance_cm;
  uint8_t state;
  bool isEdgeTrigger;
  uint32_t uptime_s;
};

// FreeRTOS Synchronization Primitives
SemaphoreHandle_t sensorMutex = NULL;
QueueHandle_t telemetryQueue = NULL;
QueueHandle_t tgOutQueue = NULL;
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t tgTaskHandle = NULL;

SensorSnapshot latestSnapshot = {25.0f, 55.0f, 1013.2f, 0,     400,
                                 false, false, 0,       STATE_NORMAL,
                                 false, false, false};

// Flame ISR flag
volatile bool flameIsrTriggered = false;
void IRAM_ATTR onFlameISR() { flameIsrTriggered = true; }

// ============================================================================
//  HARDWARE DRIVER INSTANCES & DETECTION FLAGS
// ============================================================================
Adafruit_BME280 bme;
Adafruit_SGP30 sgp;
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
HardwareSerial radarSerial(1);
MyLD2410 radar(radarSerial);
Adafruit_NeoPixel strip(NEOPIXEL_COUNT, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

bool bmeReady = false;
bool sgpReady = false;
bool oledReady = false;
bool radarReady = false;
bool loraReady = false;

// ============================================================================
//  NETWORK, MQTT, AUTH & TELEGRAM CONFIGURATION
// ============================================================================
const char *AP_FALLBACK_SSID = "LabSafety-S3-Setup";
const char *AP_FALLBACK_PASS = "12345678";

char deviceName[32] = "lab_main_s3";
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
  char alertEveryUnit[12]; // "seconds", "minutes", "hours"
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
const unsigned long TG_POLL_MS = 2500;
unsigned long lastPeriodicTgMs = 0;

unsigned long lastMqttRetry = 0;
const unsigned long mqttRetryInterval = 5000;
bool shouldReboot = false;
volatile bool otaRunning = false;
String lastOtaStatus = "";

// Forward declarations
void applyMqttTransport();
void wsEventHandler(WStype_t type, uint8_t *payload, size_t length);
void mqttCallback(char *topic, byte *payload, unsigned int length);
void tgSend(const String &msg);

// ============================================================================
//  HELPERS & SGP30 BASELINE PERSISTENCE
// ============================================================================
uint32_t getAlertIntervalMs() {
  if (tgConfig.alertEveryVal == 0)
    return 0;
  if (strcmp(tgConfig.alertEveryUnit, "seconds") == 0)
    return tgConfig.alertEveryVal * 1000UL;
  if (strcmp(tgConfig.alertEveryUnit, "hours") == 0)
    return tgConfig.alertEveryVal * 3600000UL;
  return tgConfig.alertEveryVal * 60000UL; // default: minutes
}

void loadSgpBaseline() {
  if (!sgpReady || !LittleFS.exists("/baseline.txt"))
    return;
  File f = LittleFS.open("/baseline.txt", "r");
  if (!f)
    return;
  String line = f.readStringUntil('\n');
  f.close();
  uint32_t eco2_base = 0, tvoc_base = 0;
  if (sscanf(line.c_str(), "%lx,%lx", &eco2_base, &tvoc_base) == 2) {
    sgp.setIAQBaseline((uint16_t)eco2_base, (uint16_t)tvoc_base);
  }
}

void saveSgpBaseline() {
  if (!sgpReady)
    return;
  uint16_t eco2_base = 0, tvoc_base = 0;
  if (!sgp.getIAQBaseline(&eco2_base, &tvoc_base))
    return;

  // Briefly mute GPIO 4 hardware interrupt during flash write per docs/firmware.md
  detachInterrupt(digitalPinToInterrupt(PIN_FLAME));
  File f = LittleFS.open("/baseline.txt", "w");
  if (f) {
    f.printf("%04X,%04X\n", eco2_base, tvoc_base);
    f.close();
  }
  attachInterrupt(digitalPinToInterrupt(PIN_FLAME), onFlameISR, CHANGE);
}

// ============================================================================
//  CONFIG LOAD / SAVE (LittleFS)
// ============================================================================
void loadConfig() {
  strlcpy(deviceName, "lab_main_s3", sizeof(deviceName));
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
    strlcpy(deviceName, doc["deviceName"] | "lab_main_s3", sizeof(deviceName));
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

  detachInterrupt(digitalPinToInterrupt(PIN_FLAME));
  File f = LittleFS.open("/config.json", "w");
  if (f) {
    serializeJsonPretty(doc, f);
    f.close();
  }
  attachInterrupt(digitalPinToInterrupt(PIN_FLAME), onFlameISR, CHANGE);
}

void loadUsers() {
  userCount = 0;
  // Always ensure default admin account if users.json missing
  strlcpy(users[0].username, "admin", sizeof(users[0].username));
  strlcpy(users[0].password, "admin123", sizeof(users[0].password));
  userCount = 1;

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
  detachInterrupt(digitalPinToInterrupt(PIN_FLAME));
  File f = LittleFS.open("/users.json", "w");
  if (f) {
    serializeJsonPretty(doc, f);
    f.close();
  }
  attachInterrupt(digitalPinToInterrupt(PIN_FLAME), onFlameISR, CHANGE);
}

void loadTelegramConfig() {
  memset(&tgConfig, 0, sizeof(tgConfig));
  tgConfig.alertEveryVal = 5;
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
    tgConfig.alertEveryVal = doc["alertEveryVal"] | 5;
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

  detachInterrupt(digitalPinToInterrupt(PIN_FLAME));
  File f = LittleFS.open("/telegram.json", "w");
  if (f) {
    serializeJsonPretty(doc, f);
    f.close();
  }
  attachInterrupt(digitalPinToInterrupt(PIN_FLAME), onFlameISR, CHANGE);
}

// ============================================================================
//  BACKUP / RESTORE CONFIGS ACROSS LITTLEFS OTA
// ============================================================================
String bk_config, bk_telegram, bk_users, bk_baseline;

void backupConfigs() {
  bk_config = "";
  bk_telegram = "";
  bk_users = "";
  bk_baseline = "";
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
  if ((f = LittleFS.open("/baseline.txt", "r"))) {
    bk_baseline = f.readString();
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
  if (bk_baseline.length() > 0 && (f = LittleFS.open("/baseline.txt", "w"))) {
    f.print(bk_baseline);
    f.close();
  }
}

// ============================================================================
//  TELEGRAM BOT LOGIC (Core 0 Task)
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
  SensorSnapshot snap;
  if (sensorMutex && xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
    snap = latestSnapshot;
    xSemaphoreGive(sensorMutex);
  } else {
    snap = latestSnapshot;
  }

  String ip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString()
                                              : WiFi.softAPIP().toString();
  char buf[420];
  snprintf(buf, sizeof(buf),
           "*Lab Safety Status*\n"
           "State: *State %d — %s*\n"
           "IP: `%s`\n\n"
           "*Actuator Outputs:*\n"
           "Exhaust Fan: *%s*\n"
           "Sprinkler (Atomizer): *%s*\n"
           "Buzzer Alarm: *%s*",
           (int)snap.state, STATE_LABELS[snap.state], ip.c_str(),
           snap.fanOn ? "ON" : "OFF", snap.atomizerOn ? "ON" : "OFF",
           snap.buzzerOn ? "ON" : "OFF");
  return String(buf);
}

String buildSensorsMsg() {
  SensorSnapshot snap;
  if (sensorMutex && xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
    snap = latestSnapshot;
    xSemaphoreGive(sensorMutex);
  } else {
    snap = latestSnapshot;
  }

  char buf[420];
  snprintf(buf, sizeof(buf),
           "*Live Lab Sensor Readings*\n"
           "Temperature: `%.1f C`\n"
           "Humidity: `%.1f %%`\n"
           "Pressure: `%.1f hPa`\n"
           "TVOC: `%u ppb`\n"
           "eCO2: `%u ppm`\n"
           "IR Flame: *%s*\n"
           "Human Presence: *%s*\n"
           "Target Distance: `%u cm`",
           snap.temp, snap.humidity, snap.pressure, snap.tvoc, snap.eco2,
           snap.flame ? "DETECTED" : "Clear",
           snap.presence ? "Detected" : "None", snap.distance_cm);
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
           "*ESP32-S3 Health*\n"
           "Chip Temp: `%.1f C`\n"
           "Free Heap: `%lu KB`\n"
           "Free PSRAM: `%lu KB`\n"
           "WiFi RSSI: `%d dBm`\n"
           "IP: `%s`\n"
           "CPU: `%u MHz`\n"
           "Uptime: `%s`\n"
           "FW: `v%.1f` | FS: `v%.1f`",
           temperatureRead(), (unsigned long)(ESP.getFreeHeap() / 1024),
           (unsigned long)(ESP.getFreePsram() / 1024), (int)WiFi.RSSI(),
           ip.c_str(), getCpuFrequencyMhz(), up, FIRMWARE_VERSION,
           currentFsVersion);
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
        "*Smart Lab Safety Bot Commands*\n\n"
        "/status — Current Safety State (0-4) & actuator outputs\n"
        "/sensors — Live Temp, Humidity, Pressure, TVOC, eCO2, Flame & Radar\n"
        "/health — ESP32-S3 chip temp, heap, WiFi RSSI, IP & uptime\n"
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
//  REMOTE GITHUB OTA UPDATE (On-Demand via "Check for Update" Button)
// ============================================================================
bool downloadAndFlash(WiFiClientSecure *client, const String &url,
                      int command) {
  HTTPClient http;
  http.begin(*client, url);
  int httpCode = http.GET();
  bool success = false;
  if (httpCode == HTTP_CODE_OK) {
    if (command == U_SPIFFS)
      backupConfigs();
    int contentLength = http.getSize();
    detachInterrupt(digitalPinToInterrupt(PIN_FLAME));
    if (Update.begin(contentLength, command)) {
      WiFiClient *stream = http.getStreamPtr();
      size_t written = Update.writeStream(*stream);
      if (written == (size_t)contentLength && Update.end() &&
          Update.isFinished()) {
        if (command == U_SPIFFS)
          restoreConfigs();
        success = true;
      }
    }
    attachInterrupt(digitalPinToInterrupt(PIN_FLAME), onFlameISR, CHANGE);
  }
  http.end();
  return success;
}

void performGitHubOTA() {
  lastOtaStatus = "Checking remote repository for updates...";
  tgSend("*OTA Update Check Started*");

  WiFiClientSecure *client = new WiFiClientSecure;
  if (!client) {
    lastOtaStatus = "OTA failed: Out of memory.";
    return;
  }
  client->setInsecure();

  HTTPClient http;
  http.begin(*client, "https://raw.githubusercontent.com/imaller43/"
                      "Smart_Laboratory_Monitoring_System-IDP/main/firmware/"
                      "version.json");
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    lastOtaStatus = "Update check failed: Could not fetch firmware/version.json (HTTP " +
                    String(httpCode) + ").";
    tgSend("OTA check failed: Could not fetch firmware/version.json");
    http.end();
    delete client;
    return;
  }
  String payload = http.getString();
  http.end();

#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<512> doc;
#endif
  if (deserializeJson(doc, payload)) {
    lastOtaStatus = "Update check failed: Invalid version.json format.";
    delete client;
    return;
  }

  float newFwVer = doc["firmware_version"] | FIRMWARE_VERSION;
  float newFsVer = doc["fs_version"] | currentFsVersion;
  String fwUrl = doc["firmware_url"] | "";
  String fsUrl = doc["fs_url"] | "";

  bool fwHasNewer = (newFwVer - FIRMWARE_VERSION) > 0.001f && fwUrl.length() > 0;
  bool fsHasNewer = (newFsVer - currentFsVersion) > 0.001f && fsUrl.length() > 0;

  // Guard: If remote versions are the same (or older), do NOT update or overwrite
  if (!fwHasNewer && !fsHasNewer) {
    lastOtaStatus = "System is already up to date (Firmware v" +
                    String(FIRMWARE_VERSION, 1) + ", LittleFS v" +
                    String(currentFsVersion, 1) + "). No update applied.";
    tgSend("*System is already up to date.*\nFirmware: `v" +
           String(FIRMWARE_VERSION, 1) + "`\nLittleFS: `v" +
           String(currentFsVersion, 1) + "`");
    delete client;
    return;
  }

  bool rebootNeeded = false;

  if (fwHasNewer) {
    lastOtaStatus = "Downloading firmware v" + String(newFwVer, 1) + "...";
    tgSend("*Downloading firmware v" + String(newFwVer, 1) + "...*");
    if (downloadAndFlash(client, fwUrl, U_FLASH)) {
      lastOtaStatus = "Firmware updated to v" + String(newFwVer, 1) + "!";
      tgSend("Firmware updated successfully!");
      rebootNeeded = true;
    } else {
      lastOtaStatus = "Firmware update failed.";
      tgSend("Firmware update failed.");
    }
  }

  if (fsHasNewer) {
    lastOtaStatus = "Downloading LittleFS v" + String(newFsVer, 1) + "...";
    tgSend("*Downloading LittleFS v" + String(newFsVer, 1) + "...*");
    if (downloadAndFlash(client, fsUrl, U_SPIFFS)) {
      lastOtaStatus = "LittleFS updated to v" + String(newFsVer, 1) + "!";
      tgSend("LittleFS updated successfully!");
      rebootNeeded = true;
    } else {
      lastOtaStatus = "LittleFS update failed.";
      tgSend("LittleFS update failed.");
    }
  }

  if (rebootNeeded) {
    lastOtaStatus = "Update complete! Rebooting ESP32-S3...";
    tgSend("*OTA Update Complete!* Rebooting...");
    unsigned long waitStart = millis();
    while (uxQueueMessagesWaiting(tgOutQueue) > 0 &&
           millis() - waitStart < 5000) {
      delay(100);
    }
    delay(1000);
    ESP.restart();
  }

  delete client;
}

void otaTask(void *pv) {
  performGitHubOTA();
  otaRunning = false;
  vTaskDelete(NULL);
}

void triggerOTA() {
  if (!otaRunning) {
    otaRunning = true;
    xTaskCreatePinnedToCore(otaTask, "OTATask", 8192, NULL, 1, NULL, 0);
  }
}

// ============================================================================
//  CORE 1: SENSOR POLLING, DISPLAY & SAFETY STATE MACHINE TASK
// ============================================================================
void updateActuatorsAndLed(SafetyState state, bool &fanOut, bool &atomOut,
                           bool &buzzOut) {
  unsigned long now = millis();
  uint32_t ledColor = 0;

  switch (state) {
  case STATE_NORMAL:
    fanOut = false;
    atomOut = false;
    buzzOut = false;
    ledColor = strip.Color(0, 180, 0); // Green
    break;

  case STATE_WARNING: {
    // State 1: Minor Chemical Leak -> Yellow, Fan ON, Buzzer beeps 2x / 2 sec
    fanOut = true;
    atomOut = false;
    unsigned long phase = now % 2000UL;
    buzzOut = (phase < 150) || (phase >= 300 && phase < 450);
    ledColor = strip.Color(220, 160, 0); // Yellow
    break;
  }

  case STATE_CRITICAL: {
    // State 2: Severe Toxic Leak -> Flashing Yellow, Fan ON, Buzzer 2x / 1 sec
    fanOut = true;
    atomOut = false;
    unsigned long phase = now % 1000UL;
    buzzOut = (phase < 120) || (phase >= 240 && phase < 360);
    bool flashOn = ((now / 250) % 2) == 0;
    ledColor = flashOn ? strip.Color(255, 180, 0) : strip.Color(0, 0, 0);
    break;
  }

  case STATE_EMERGENCY: {
    // State 3: Confirmed Fire -> Flashing Red, Fan OFF, Atomizer ON, Buzzer ON
    fanOut = false;
    atomOut = true;
    buzzOut = true;
    bool flashOn = ((now / 200) % 2) == 0;
    ledColor = flashOn ? strip.Color(255, 0, 0) : strip.Color(0, 0, 0);
    break;
  }

  case STATE_GLITCH:
    // State 4: Sensor Glitch -> Blue, all actuators OFF
    fanOut = false;
    atomOut = false;
    buzzOut = false;
    ledColor = strip.Color(0, 80, 255); // Blue
    break;
  }

  digitalWrite(PIN_RELAY_FAN, fanOut ? RELAY_ON_LEVEL : RELAY_OFF_LEVEL);
  digitalWrite(PIN_RELAY_ATOMIZER, atomOut ? RELAY_ON_LEVEL : RELAY_OFF_LEVEL);
  digitalWrite(PIN_BUZZER, buzzOut ? BUZZER_ON_LEVEL : BUZZER_OFF_LEVEL);

  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    strip.setPixelColor(i, ledColor);
  }
  strip.show();
}

void updateOledDisplay(const SensorSnapshot &snap) {
  if (!oledReady)
    return;
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);

  // Top Header Bar: State
  char line1[32];
  snprintf(line1, sizeof(line1), "ST%d: %s", (int)snap.state,
           STATE_LABELS[snap.state]);
  oled.drawStr(0, 10, line1);
  oled.drawHLine(0, 13, 128);

  // Line 2: Temp & Humidity
  char line2[32];
  snprintf(line2, sizeof(line2), "T:%.1fC  H:%.1f%%", snap.temp, snap.humidity);
  oled.drawStr(0, 26, line2);

  // Line 3: TVOC & eCO2
  char line3[32];
  snprintf(line3, sizeof(line3), "TVOC:%uppb CO2:%u", snap.tvoc, snap.eco2);
  oled.drawStr(0, 38, line3);

  // Line 4: Radar Presence & Distance
  char line4[32];
  snprintf(line4, sizeof(line4), "Radar:%s (%ucm)",
           snap.presence ? "YES" : "NO", snap.distance_cm);
  oled.drawStr(0, 50, line4);

  // Line 5: Flame & Wi-Fi IP
  char line5[32];
  String ip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString()
                                              : WiFi.softAPIP().toString();
  snprintf(line5, sizeof(line5), "Flm:%d IP:%s", snap.flame ? 1 : 0,
           ip.c_str());
  oled.drawStr(0, 62, line5);

  oled.sendBuffer();
}

void sensorAndSafetyTask(void *pv) {
  unsigned long lastPollMs = 0;
  unsigned long lastPublishMs = 0;
  float prevTemp = 25.0f;
  SafetyState currentState = STATE_NORMAL;

  // Running sum/count for Mode A (3-minute averaging in States 0 & 4)
  double sumTemp = 0.0;
  uint32_t sumTvoc = 0;
  uint32_t sampleCount = 0;

  for (;;) {
    unsigned long now = millis();

    // 1. Continuously parse HLK-LD2410C UART frames
    if (radarReady && radar.check() == MyLD2410::DATA) {
      bool pres = radar.presenceDetected();
      uint16_t dist = pres ? (uint16_t)radar.detectedDistance() : 0;
      if (sensorMutex && xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
        latestSnapshot.presence = pres;
        latestSnapshot.distance_cm = dist;
        xSemaphoreGive(sensorMutex);
      }
    }

    // 2. Check IR Flame Sensor immediately (ISR or direct digitalRead)
    bool flameActive = (digitalRead(PIN_FLAME) == FLAME_ACTIVE_LEVEL);

    // 3. Strictly 1Hz (1000ms) I2C Polling for BME280 & SGP30 + OLED update
    bool trigger1HzEval = (now - lastPollMs >= 1000UL);
    if (trigger1HzEval || flameIsrTriggered) {
      flameIsrTriggered = false;

      float t = latestSnapshot.temp;
      float h = latestSnapshot.humidity;
      float p = latestSnapshot.pressure;
      uint16_t tvoc = latestSnapshot.tvoc;
      uint16_t eco2 = latestSnapshot.eco2;

      if (trigger1HzEval) {
        lastPollMs = now;
        if (bmeReady) {
          float rt = bme.readTemperature();
          float rh = bme.readHumidity();
          float rp = bme.readPressure() / 100.0F;
          if (!isnan(rt))
            t = rt;
          if (!isnan(rh))
            h = rh;
          if (!isnan(rp))
            p = rp;
        }
        if (sgpReady && sgp.IAQmeasure()) {
          tvoc = sgp.TVOC;
          eco2 = sgp.eCO2;
        }
        sumTemp += t;
        sumTvoc += tvoc;
        sampleCount++;
      }

      // Evaluate Sensor Fusion State Machine (States 0 through 4)
      bool rapidTempRise = (t - prevTemp) >= 2.5f; // >=2.5C rise in 1 second
      if (trigger1HzEval)
        prevTemp = t;

      SafetyState nextState = STATE_NORMAL;
      if (flameActive) {
        if (t > 45.0f || rapidTempRise) {
          nextState = STATE_EMERGENCY; // State 3: Confirmed Fire
        } else if (t < 30.0f && tvoc < 250) {
          nextState = STATE_GLITCH; // State 4: Sensor Glitch / False Alarm
        } else {
          nextState = STATE_EMERGENCY; // Flame with elevated temp/gas -> Fire
        }
      } else {
        if (tvoc > 1500) {
          nextState = STATE_CRITICAL; // State 2: Severe Toxic Leak
        } else if (tvoc >= 500 && t < 32.0f) {
          nextState = STATE_WARNING; // State 1: Minor Chemical Leak
        } else {
          nextState = STATE_NORMAL; // State 0: Normal
        }
      }

      bool stateChanged = (nextState != currentState);
      currentState = nextState;

      bool fanOn = false, atomOn = false, buzzOn = false;
      updateActuatorsAndLed(currentState, fanOn, atomOn, buzzOn);

      // Update shared snapshot under Mutex
      SensorSnapshot snapCopy;
      if (sensorMutex &&
          xSemaphoreTake(sensorMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        latestSnapshot.temp = t;
        latestSnapshot.humidity = h;
        latestSnapshot.pressure = p;
        latestSnapshot.tvoc = tvoc;
        latestSnapshot.eco2 = eco2;
        latestSnapshot.flame = flameActive;
        latestSnapshot.state = currentState;
        latestSnapshot.fanOn = fanOn;
        latestSnapshot.atomizerOn = atomOn;
        latestSnapshot.buzzerOn = buzzOn;
        snapCopy = latestSnapshot;
        xSemaphoreGive(sensorMutex);
      } else {
        snapCopy = latestSnapshot;
      }

      if (trigger1HzEval) {
        updateOledDisplay(snapCopy);
      }

      // Dynamic Publishing Rules (Edge-Triggered + Mode A 3-min / Mode B 1-sec)
      bool isBurstMode = (currentState == STATE_WARNING ||
                          currentState == STATE_CRITICAL ||
                          currentState == STATE_EMERGENCY);
      unsigned long pubInterval = isBurstMode ? 1000UL : 180000UL;

      if (stateChanged || (now - lastPublishMs >= pubInterval)) {
        lastPublishMs = now;
        TelemetryPacket pkt;
        if (!isBurstMode && !stateChanged && sampleCount > 0) {
          pkt.temp = (float)(sumTemp / (double)sampleCount);
          pkt.tvoc = (uint16_t)(sumTvoc / sampleCount);
          sumTemp = 0.0;
          sumTvoc = 0;
          sampleCount = 0;
        } else {
          pkt.temp = snapCopy.temp;
          pkt.tvoc = snapCopy.tvoc;
        }
        pkt.humidity = snapCopy.humidity;
        pkt.pressure = snapCopy.pressure;
        pkt.eco2 = snapCopy.eco2;
        pkt.flame = snapCopy.flame;
        pkt.presence = snapCopy.presence;
        pkt.distance_cm = snapCopy.distance_cm;
        pkt.state = (uint8_t)currentState;
        pkt.isEdgeTrigger = stateChanged;
        pkt.uptime_s = now / 1000UL;

        if (telemetryQueue) {
          xQueueSend(telemetryQueue, &pkt, 0);
        }
      }
    } else {
      // Refresh blinking LEDs & buzzer cadence every 50ms between 1Hz polls
      bool fanOn = false, atomOn = false, buzzOn = false;
      updateActuatorsAndLed(currentState, fanOn, atomOn, buzzOn);
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// ============================================================================
//  MQTT & LORA FALLBACK PUBLISHING (Core 0)
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

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  if (strcmp(topic, "esp32/ota_trigger") == 0) {
    if (latestSnapshot.state == STATE_NORMAL)
      triggerOTA();
  }
}

void reconnectMQTT() {
  if (WiFi.status() != WL_CONNECTED || mqttCfg.host[0] == '\0')
    return;
  if (strcmp(mqttCfg.mode, "cloud") == 0 && !wsBridge.connected())
    return;

  bool connected = false;
  if (strlen(mqttCfg.user) > 0)
    connected = mqtt.connect(deviceName, mqttCfg.user, mqttCfg.pass);
  else
    connected = mqtt.connect(deviceName);

  if (connected) {
    mqtt.subscribe("esp32/ota_trigger");
  }
}

void publishTelemetryPacket(const TelemetryPacket &pkt) {
#if ARDUINOJSON_VERSION_MAJOR >= 7
  JsonDocument doc;
#else
  StaticJsonDocument<384> doc;
#endif
  doc["node"] = deviceName;
  doc["state"] = pkt.state;
  doc["temp"] = serialized(String(pkt.temp, 1));
  doc["humidity"] = serialized(String(pkt.humidity, 1));
  doc["pressure"] = serialized(String(pkt.pressure, 1));
  doc["tvoc"] = pkt.tvoc;
  doc["eco2"] = pkt.eco2;
  doc["flame"] = pkt.flame ? 1 : 0;
  doc["presence"] = pkt.presence;
  doc["distance_cm"] = pkt.distance_cm;
  doc["uptime_s"] = pkt.uptime_s;

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  String topic = String(deviceName) + "/telemetry";
  bool sentMqtt = false;

  if (WiFi.status() == WL_CONNECTED && mqtt.connected()) {
    sentMqtt = mqtt.publish(topic.c_str(), jsonPayload.c_str());
  }

  // Fallback to 433 MHz LoRa SX1278 if Wi-Fi/MQTT is down OR during Emergency
  if (loraReady && (!sentMqtt || pkt.state != STATE_NORMAL)) {
    LoRa.beginPacket();
    LoRa.print(jsonPayload);
    LoRa.endPacket(true); // async non-blocking TX
  }

  // Send immediate Telegram alert on State Change (Edge Trigger)
  if (pkt.isEdgeTrigger && pkt.state != STATE_NORMAL) {
    char alertBuf[350];
    snprintf(alertBuf, sizeof(alertBuf),
             "*LAB SAFETY ALERT*\n"
             "State: *State %u — %s*\n"
             "Temp: `%.1f C` | Hum: `%.1f %%`\n"
             "TVOC: `%u ppb` | eCO2: `%u ppm`\n"
             "Flame: *%s*\n"
             "Presence: *%s* (`%u cm`)",
             pkt.state, STATE_LABELS[pkt.state], pkt.temp, pkt.humidity,
             pkt.tvoc, pkt.eco2, pkt.flame ? "DETECTED" : "No",
             pkt.presence ? "Yes" : "No", pkt.distance_cm);
    tgSend(String(alertBuf));
  }
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
  doc["state"] = (uint8_t)latestSnapshot.state;

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
  tgSend("*Test OK!* Smart Laboratory Safety Monitoring Bot is online.\n\n" +
         buildSensorsMsg());
  server.send(200, "text/plain", "Queued — check Telegram in ~3 seconds");
}

void handleCheckUpdate() {
  if (!requireAuth())
    return;
  // Safety OTA Interlock per docs/firmware.md
  if (latestSnapshot.state != STATE_NORMAL) {
    server.send(403, "text/plain",
                "Safety Interlock Active: OTA updates are blocked while the "
                "laboratory is not in State 0 (Normal)!");
    return;
  }
  triggerOTA();
  server.send(200, "text/plain",
              "Checking remote repository for updates... Watch banner or "
              "Telegram for status.");
}

// ============================================================================
//  SETUP
// ============================================================================
void setup() {
  // 1. Initialize Actuator & Input Pins to Safe Defaults immediately
  pinMode(PIN_FLAME, INPUT_PULLUP);
  pinMode(PIN_RELAY_FAN, OUTPUT);
  pinMode(PIN_RELAY_ATOMIZER, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_RELAY_FAN, RELAY_OFF_LEVEL);
  digitalWrite(PIN_RELAY_ATOMIZER, RELAY_OFF_LEVEL);
  digitalWrite(PIN_BUZZER, BUZZER_OFF_LEVEL);

  strip.begin();
  strip.setBrightness(60);
  for (int i = 0; i < NEOPIXEL_COUNT; i++)
    strip.setPixelColor(i, strip.Color(0, 180, 0));
  strip.show();

  attachInterrupt(digitalPinToInterrupt(PIN_FLAME), onFlameISR, CHANGE);

  // 2. Mount LittleFS & Load Configurations
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

  // 3. Initialize I2C Bus (GPIO 8 SDA, GPIO 9 SCL) — Non-blocking probe
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

  // Probe 1.3" SH1106 OLED (0x3C)
  Wire.beginTransmission(0x3C);
  if (Wire.endTransmission() == 0) {
    oled.begin();
    oledReady = true;
    oled.clearBuffer();
    oled.setFont(u8g2_font_6x10_tf);
    oled.drawStr(0, 14, "Smart Lab Safety S3");
    oled.drawStr(0, 30, "Booting Hardware...");
    oled.sendBuffer();
  }

  // Probe BME280 (auto-check 0x76 then 0x77)
  if (bme.begin(0x76, &Wire) || bme.begin(0x77, &Wire)) {
    bmeReady = true;
  }

  // Probe SGP30 (0x58)
  if (sgp.begin(&Wire)) {
    sgpReady = true;
    loadSgpBaseline();
  }

  // 4. Probe HLK-LD2410C Radar on UART1 (RX=18, TX=17 @ 256000 baud)
  radarSerial.begin(RADAR_BAUD, SERIAL_8N1, PIN_RADAR_RX, PIN_RADAR_TX);
  radar.begin();
  radarReady = true;

  // 5. Probe LoRa SX1278 433 MHz on SPI (SCK=12, MISO=13, MOSI=11, CS=10)
  SPI.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI, PIN_LORA_CS);
  LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  if (LoRa.begin(LORA_FREQUENCY)) {
    loraReady = true;
  }

  // 6. Start Wi-Fi (AP + STA so portal is always reachable)
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

  // 7. Create FreeRTOS Queues, Mutex, and Core 1 / Core 0 Tasks
  sensorMutex = xSemaphoreCreateMutex();
  telemetryQueue = xQueueCreate(10, sizeof(TelemetryPacket));
  tgOutQueue = xQueueCreate(8, 384);

  initTelegramBot();
  xTaskCreatePinnedToCore(telegramTask, "TGTask", 12288, NULL, 1, &tgTaskHandle,
                          0);
  xTaskCreatePinnedToCore(sensorAndSafetyTask, "SafetyCore1", 8192, NULL, 3,
                          &sensorTaskHandle, 1);

  applyMqttTransport();

  // 8. Register HTTP Server Routes
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

  // REST APIs
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

  // Manual Firmware OTA Upload (with Safety Interlock)
  server.on(
      "/update/firmware", HTTP_POST,
      []() {
        if (!isAuthenticated()) {
          server.send(401, "text/plain", "Unauthorized");
          return;
        }
        if (latestSnapshot.state != STATE_NORMAL) {
          server.send(403, "text/plain",
                      "Safety Interlock: Cannot flash during hazard state!");
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
        if (!isAuthenticated() || latestSnapshot.state != STATE_NORMAL)
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

  // Manual LittleFS OTA Upload (with Safety Interlock + Config Backup/Restore)
  server.on(
      "/update/filesystem", HTTP_POST,
      []() {
        if (!isAuthenticated()) {
          server.send(401, "text/plain", "Unauthorized");
          return;
        }
        if (latestSnapshot.state != STATE_NORMAL) {
          server.send(403, "text/plain",
                      "Safety Interlock: Cannot flash during hazard state!");
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
        if (!isAuthenticated() || latestSnapshot.state != STATE_NORMAL)
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

  // Configure Arduino IDE Wireless OTA (ArduinoOTA)
  ArduinoOTA.setHostname(deviceName);
  ArduinoOTA.onStart([]() {
    detachInterrupt(digitalPinToInterrupt(PIN_FLAME));
    if (ArduinoOTA.getCommand() == U_SPIFFS) {
      backupConfigs();
    }
    if (oledReady) {
      oled.clearBuffer();
      oled.setFont(u8g2_font_6x10_tf);
      oled.drawStr(0, 16, "Arduino IDE OTA");
      oled.drawStr(0, 32,
                   ArduinoOTA.getCommand() == U_FLASH ? "Updating Firmware..."
                                                      : "Updating LittleFS...");
      oled.sendBuffer();
    }
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    unsigned int pct = (total > 0) ? (progress * 100U / total) : 0;
    if (oledReady) {
      char buf[32];
      snprintf(buf, sizeof(buf), "Progress: %u%%", pct);
      oled.clearBuffer();
      oled.setFont(u8g2_font_6x10_tf);
      oled.drawStr(0, 16, "Arduino IDE OTA");
      oled.drawStr(0, 32, buf);
      oled.drawFrame(0, 42, 128, 12);
      oled.drawBox(2, 44, (pct * 124U) / 100U, 8);
      oled.sendBuffer();
    }
  });
  ArduinoOTA.onEnd([]() {
    if (ArduinoOTA.getCommand() == U_SPIFFS) {
      restoreConfigs();
    }
    if (oledReady) {
      oled.clearBuffer();
      oled.setFont(u8g2_font_6x10_tf);
      oled.drawStr(0, 24, "OTA Complete!");
      oled.drawStr(0, 42, "Rebooting...");
      oled.sendBuffer();
    }
  });
  ArduinoOTA.onError([](ota_error_t error) {
    attachInterrupt(digitalPinToInterrupt(PIN_FLAME), onFlameISR, CHANGE);
  });
  ArduinoOTA.begin();
  server.begin();

  String ip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString()
                                              : WiFi.softAPIP().toString();
  tgSend("*Smart Lab Safety Node Online*\nIP: `" + ip + "`\nFW: `v" +
         String(FIRMWARE_VERSION, 1) + "` | FS: `v" +
         String(currentFsVersion, 1) + "`");
}

// ============================================================================
//  LOOP (Core 0: HTTP Web Server, MQTT, Telemetry Dequeue & Periodic Alerts)
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

  // Dequeue and publish telemetry packets from Core 1
  TelemetryPacket pkt;
  while (telemetryQueue && xQueueReceive(telemetryQueue, &pkt, 0) == pdTRUE) {
    publishTelemetryPacket(pkt);
  }

  // Periodic Telegram Report ("Alert Every" user-configured timer)
  uint32_t alertIntervalMs = getAlertIntervalMs();
  if (alertIntervalMs > 0 && tgInitialized && tgConfig.enabled) {
    if (now - lastPeriodicTgMs >= alertIntervalMs) {
      lastPeriodicTgMs = now;
      tgSend("*Periodic Lab Report*\n\n" + buildSensorsMsg());
    }
  }

  // Hourly SGP30 Baseline Save (every 3,600,000 ms)
  static unsigned long lastBaselineSaveMs = 0;
  if (sgpReady && (now - lastBaselineSaveMs >= 3600000UL)) {
    lastBaselineSaveMs = now;
    saveSgpBaseline();
  }
}
