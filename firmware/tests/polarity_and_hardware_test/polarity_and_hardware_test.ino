// ============================================================================
//  Phase 1 Bench Diagnostic: I2C Scanner + Flame & Relay Polarity Tester
//  Board: ESP32S3 Dev Module (ESP32-S3-N16R8)
// ============================================================================
//  How to use:
//  1. Upload this sketch and open Serial Monitor at 115200 baud.
//  2. It scans I2C (SDA=8, SCL=9) to verify BME280 (0x76/0x77), SGP30 (0x58),
//     and 1.3" SH1106 OLED (0x3C).
//  3. Every 500ms it prints the raw digitalRead(4) of the IR Flame Sensor:
//     - Hold a lighter or IR remote in front of the Flame Sensor and see
//       whether raw value changes to 0 (LOW) or 1 (HIGH).
//  4. Every 4 seconds it toggles Relay IN1 (GPIO 5) and Relay IN2 (GPIO 6)
//     between LOW and HIGH so you can see/hear which level turns the relay ON.
// ============================================================================

#include <Arduino.h>
#include <Wire.h>

#define PIN_I2C_SDA        8
#define PIN_I2C_SCL        9
#define PIN_FLAME          4
#define PIN_RELAY_FAN      5
#define PIN_RELAY_ATOMIZER 6
#define PIN_BUZZER         7

unsigned long lastFlamePrintMs = 0;
unsigned long lastRelayToggleMs = 0;
bool testLevelHigh = true;

void scanI2C() {
  Serial.println("\n--- Scanning I2C Bus (SDA=GPIO 8, SCL=GPIO 9) ---");
  int count = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  [FOUND] I2C Device at 0x%02X", addr);
      if (addr == 0x3C || addr == 0x3D)
        Serial.print(" -> 1.3\" SH1106 OLED Display");
      else if (addr == 0x58)
        Serial.print(" -> SGP30 Air Quality Sensor");
      else if (addr == 0x76 || addr == 0x77)
        Serial.print(" -> BME280 Climate Sensor");
      Serial.println();
      count++;
    }
  }
  if (count == 0) {
    Serial.println("  [WARN] No I2C devices found! Check 3.3V power, GND, SDA=8, SCL=9.");
  }
  Serial.println("--------------------------------------------------\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=======================================================");
  Serial.println("  ESP32-S3 Hardware & Polarity Bench Diagnostic Tool   ");
  Serial.println("=======================================================");

  pinMode(PIN_FLAME, INPUT_PULLUP);
  pinMode(PIN_RELAY_FAN, OUTPUT);
  pinMode(PIN_RELAY_ATOMIZER, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_RELAY_FAN, HIGH);
  digitalWrite(PIN_RELAY_ATOMIZER, HIGH);
  digitalWrite(PIN_BUZZER, LOW);

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  scanI2C();
}

void loop() {
  unsigned long now = millis();

  // Print Flame Sensor raw state every 500ms
  if (now - lastFlamePrintMs >= 500) {
    lastFlamePrintMs = now;
    int rawFlame = digitalRead(PIN_FLAME);
    Serial.printf("[FLAME GPIO 4] Raw digitalRead = %d (%s)\n", rawFlame,
                  rawFlame == LOW ? "LOW" : "HIGH");
  }

  // Toggle Relay pins between HIGH and LOW every 4 seconds
  if (now - lastRelayToggleMs >= 4000) {
    lastRelayToggleMs = now;
    testLevelHigh = !testLevelHigh;
    int level = testLevelHigh ? HIGH : LOW;
    digitalWrite(PIN_RELAY_FAN, level);
    digitalWrite(PIN_RELAY_ATOMIZER, level);
    Serial.printf("\n>>> [RELAY TEST] Setting GPIO 5 (Fan) & GPIO 6 (Atomizer) to %s (%d) <<<\n",
                  testLevelHigh ? "HIGH" : "LOW", level);
    Serial.println("    Observe your Relay LEDs/Click: If ON now, set RELAY_ON_LEVEL to this value in pins.h!\n");
  }
}
