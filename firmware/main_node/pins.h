#pragma once

// ============================================================
//  ESP32-S3-N16R8 GPIO Pinout & Polarity Configuration
//  Reference: docs/BluePrint.md (Section 9) & docs/hardware.md
// ============================================================

// --- Shared I2C Bus (BME280, SGP30, 1.3" SH1106 OLED) ---
#define PIN_I2C_SDA         8
#define PIN_I2C_SCL         9

// --- HLK-LD2410C mmWave Radar (UART1 @ 256000 baud) ---
// Cross-wired: Radar TX -> ESP32 RX (GPIO 18), Radar RX -> ESP32 TX (GPIO 17)
#define PIN_RADAR_RX        18
#define PIN_RADAR_TX        17
#define RADAR_BAUD          256000

// --- LoRa SX1278 433 MHz Module (SPI) ---
#define PIN_LORA_SCK        12
#define PIN_LORA_MISO       13
#define PIN_LORA_MOSI       11
#define PIN_LORA_CS         10
#define PIN_LORA_RST        14
#define PIN_LORA_DIO0       15
#define LORA_FREQUENCY      433E6

// --- Digital Input: IR Flame Sensor ---
#define PIN_FLAME           4
// Most LM393 IR Flame modules pull DO LOW when flame is detected.
// Flip to HIGH if Phase 1C bench test shows Active-HIGH.
#define FLAME_ACTIVE_LEVEL  LOW

// --- Digital Outputs: Relays, Active Buzzer, WS2812B ARGB LED ---
#define PIN_RELAY_FAN       5   // Relay Channel 1: 5V DC Exhaust Fan
#define PIN_RELAY_ATOMIZER  6   // Relay Channel 2: 5V Ultrasonic Atomizer (Sprinkler)
#define PIN_BUZZER          7   // Active Buzzer module
#define PIN_NEOPIXEL        21  // WS2812B DIN (via 330 ohm resistor)
#define NEOPIXEL_COUNT      8   // Number of WS2812B LEDs on the module/strip

// Most opto-isolated 2-channel 5V relay modules are Active-LOW (LOW = ON).
// Flip RELAY_ON_LEVEL to HIGH and RELAY_OFF_LEVEL to LOW if your relay is Active-HIGH.
#define RELAY_ON_LEVEL      LOW
#define RELAY_OFF_LEVEL     HIGH

// Active Buzzer (usually Active-HIGH; flip if your buzzer module is Active-LOW)
#define BUZZER_ON_LEVEL     HIGH
#define BUZZER_OFF_LEVEL    LOW
