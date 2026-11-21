// ============================================================================
//  Phase 1 Bench Diagnostic: Continuous I2C Scanner + Flame & Relay Tester
//  Board: ESP32S3 Dev Module (ESP32-S3-N16R8)
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
unsigned long lastCycleMs = 0;
bool testLevelHigh = true;

void scanI2C() {
  Serial.println("\n--- Scanning I2C Bus (SDA=GPIO 8, SCL=GPIO 9 @ 100kHz) ---");

  int count = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
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
    Serial.println("  [WARN] No I2C devices found! Check:");
    Serial.println("         1) Try plugging ONLY ONE I2C module at a time (OLED -> BME280 -> SGP30)");
    Serial.println("         2) Check 3.3V and GND on the module (watch for breadboard middle rail split!)");
    Serial.println("         3) Verify SDA -> GPIO 8 and SCL -> GPIO 9 (not swapped)");
  }
  Serial.println("----------------------------------------------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1500);
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

  // Check if SDA or SCL is stuck LOW before starting Wire
  pinMode(PIN_I2C_SDA, INPUT_PULLUP);
  pinMode(PIN_I2C_SCL, INPUT_PULLUP);
  delay(10);
  Serial.printf("[I2C LINE CHECK] Idle SDA (GPIO 8) = %s | Idle SCL (GPIO 9) = %s\n",
                digitalRead(PIN_I2C_SDA) ? "HIGH (OK)" : "LOW (STUCK/SHORTED!)",
                digitalRead(PIN_I2C_SCL) ? "HIGH (OK)" : "LOW (STUCK/SHORTED!)");

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 100000); // 100 kHz for breadboard stability
  scanI2C();
}

void loop() {
  unsigned long now = millis();

  // Print Flame Sensor raw state every 1000ms
  if (now - lastFlamePrintMs >= 1000) {
    lastFlamePrintMs = now;
    int rawFlame = digitalRead(PIN_FLAME);
    Serial.printf("[FLAME GPIO 4] Raw digitalRead = %d (%s)\n", rawFlame,
                  rawFlame == LOW ? "LOW (FLAME DETECTED)" : "HIGH (Clear)");
  }

  // Re-scan I2C and toggle Relay pins every 4 seconds
  if (now - lastCycleMs >= 4000) {
    lastCycleMs = now;

    // Rescan I2C continuously so you can plug in 1 module at a time and see it live
    scanI2C();

    testLevelHigh = !testLevelHigh;
    int level = testLevelHigh ? HIGH : LOW;
    digitalWrite(PIN_RELAY_FAN, level);
    digitalWrite(PIN_RELAY_ATOMIZER, level);
    Serial.printf(">>> [RELAY TEST] GPIO 5 (Fan) & GPIO 6 (Atomizer) = %s (%d) <<<\n\n",
                  testLevelHigh ? "HIGH (OFF)" : "LOW (ON)", level);
  }
}
