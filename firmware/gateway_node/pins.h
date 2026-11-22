#pragma once

// ============================================================
//  ESP32 Super Mini (Gateway Node) GPIO Pinout Configuration
//  Reference: docs/BluePrint.md (Section 9) & docs/hardware.md (Section 3)
// ============================================================

// --- LoRa SX1278 433 MHz Module (Custom SPI Mapping) ---
#define PIN_LORA_SCK        4   // Standard SPI Clock
#define PIN_LORA_MISO       5   // Standard SPI Data In
#define PIN_LORA_MOSI       6   // Standard SPI Data Out
#define PIN_LORA_CS         7   // Chip Select (NSS)
#define PIN_LORA_RST        3   // Reset
#define PIN_LORA_DIO0       10  // Interrupt (DIO0)
#define LORA_FREQUENCY      433E6

// --- Onboard Status LED (ESP32 Super Mini Blue LED on GPIO 8, Active-LOW) ---
#define PIN_STATUS_LED      8
#define LED_ON_LEVEL        LOW
#define LED_OFF_LEVEL       HIGH
