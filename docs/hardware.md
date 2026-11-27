# Lab Safety Monitoring System - Hardware Architecture
This document outlines the finalized hardware architecture, power distribution tree, and pin mappings for the industrial-grade Lab Safety Monitoring System.

## 1. Power Distribution Tree (Star Topology)
The system utilizes a strict star-ground topology to isolate noisy components (relays, PWM LEDs) from sensitive analog and RF modules. All component grounds must tie back to the `OUT-` pin of the LM2596S.

* **Main Supply:** 12V 3A DC Power Adapter.

* **Master Switch:** Latching SPST Toggle Switch (e.g., E-TEN1021 15A 250VAC) wired in series on the 12V positive line before the step-down converter.

* **Primary Step-Down:** LM2596S Buck Converter.

  * **IN+ / IN-:** 12V from the switched main supply.

  * **OUT+ / OUT-:** Outputs stable 5V and establishes the Common Star Ground.

* **Isolated RF Power:** Dedicated AMS1117-3.3V Linear Regulator.

  * Fed from the 5V line, dedicated strictly to powering the LoRa SX1278 module on the Main Node to prevent voltage droop during 120mA RF transmit bursts.

## 2. Main Node Hardware (ESP32-S3)
The primary sensor and actuator hub for the laboratory.

### I2C Bus (Powered by ESP32 3.3V)
* **Pins:** SDA (GPIO 8), SCL (GPIO 9)

* **BME280 (Climate):** Built-in decoupling capacitors and 10kΩ pull-ups.

* **SGP30 (Air Quality):** Built-in 10kΩ pull-ups (parallel with BME280 yields an optimal 5kΩ effective pull-up).

* **OLED Display:** 4-pin I2C module.

### UART & Digital Sensors
* **HLK-LD2410C Radar:** RX (GPIO 18), TX (GPIO 17). 5V powered (standard 2.54mm header pins).

* **IR Flame Sensor:** DO connected to GPIO 4. Powered by 3.3V (via AMS1117) to ensure safe 3.3V logic output to the ESP32.

### SPI Bus (LoRa RF)
* **LoRa SX1278:** Standard SPI pins to ESP32-S3. Powered exclusively by the isolated AMS1117-3.3V regulator.

### Actuators & Alerts
* **Exhaust Fan:** 5V Relay IN1 (GPIO 5).

* **Atomizer:** 5V Relay IN2 (GPIO 6).

* **Active Buzzer Module:** I/O Pin to GPIO 7. Powered by **3.3V** (critical for Low-Level Trigger PNP modules so ESP32 3.3V HIGH turns the transistor completely OFF).

* **WS2812B LED (ARGB):** Data input to GPIO 21. Powered directly from the 5V rail.

## 3. Gateway Node (Fallback Receiver)
Acts as an independent 24/7 failsafe bridge to receive emergency LoRa packets from the Main Node and publish them to the cloud EMQX MQTT Broker if the primary laboratory Wi-Fi goes down.

* **Controller:** **Seeed Studio XIAO ESP32C3** (32-bit RISC-V, metal RF-shielded industrial module).
  * Chosen over generic Super Mini clones for superior 24/7 thermal stability, genuine USB-C CC termination resistors (compatible with both USB-A-to-C and USB-C-to-C power adapters), and an external antenna interface.

* **External Wi-Fi Antenna (Critical Requirement):**
  * The XIAO ESP32C3 has **no onboard ceramic antenna**; its RF path routes exclusively to the onboard **U.FL (IPEX) socket** located between the `R` and `B` buttons.
  * The included **2.4 GHz external FPC antenna must be snapped on** prior to powering the board to ensure reliable long-range Wi-Fi reception and prevent RF transceiver damage.

* **Main Power & Isolated RF Rail:**
  * **Primary Supply:** 5V 1A/2A USB-C Wall Adapter plugged into the XIAO ESP32C3 USB-C port.
  * **Isolated RF Power (Dedicated AMS1117-3.3V):** The XIAO's **`5V` pin** outputs USB VBUS (~4.8V via its internal Schottky diode) to feed a second, dedicated **AMS1117-3.3V linear regulator**.
  * This isolated 3.3V rail powers the **LoRa SX1278 (`VCC`)** exclusively, keeping the XIAO's onboard 3.3V regulator cool during 24/7 Wi-Fi operation and handling 120mA RF spikes safely if two-way LoRa ACK packets are enabled.
  * **Common Ground:** XIAO **`GND`**, AMS1117 **`GND`**, capacitor grounds, and LoRa **`GND`** must all tie together.

* **LoRa SX1278 Strapping-Safe Custom SPI Mapping (`SPI.begin(4, 5, 6, 7)`):**
  * Intentionally skips ESP32-C3 boot strapping pins (`D0 / GPIO 2`, `D8 / GPIO 8`, and `D9 / GPIO 9`) so the Gateway is guaranteed to boot cleanly after any power interruption, even if LoRa SPI lines fluctuate at startup:
  * **SCK (Clock):** **`D2`** (`GPIO 4`)
  * **MISO (Data In):** **`D3`** (`GPIO 5`)
  * **MOSI (Data Out):** **`D4`** (`GPIO 6`)
  * **CS / NSS (Chip Select):** **`D5`** (`GPIO 7`)
  * **RST (Reset):** **`D1`** (`GPIO 3`)
  * **DIO0 (Interrupt):** **`D10`** (`GPIO 10`)

## 4. Passive Components & Hardware Protections
Crucial discrete components required to maintain signal integrity and prevent brownouts.

### AMS1117-3.3V Regulator Capacitors (Applies to BOTH Nodes)
Both the Main Node and Gateway Node require these capacitors on their respective AMS1117 regulators:

* **Input (5V):** 10µF and 0.1µF (100nF) ceramic capacitors wired in parallel between `VIN` and `GND`.

* **Output (3.3V):** 22µF (Tantalum) and 0.1µF (100nF) ceramic capacitors wired in parallel between `VOUT` and `GND`.

  * **Critical:** The 0.1µF output capacitor must be placed as physically close to the LoRa SX1278 VCC pin as possible.

### WS2812B LED Protections (Main Node)

* **Bulk Decoupling Capacitor:** 1000µF Electrolytic Capacitor.
  * Wired in parallel at the LED strip's power input pads.
  * **Polarity:** Unmarked positive leg to 5V; striped negative leg to the Common Star Ground. Acts as a local reservoir to handle high-frequency PWM switching spikes.

* **Data Line Resistor:** 330Ω resistor wired in series between the ESP32-S3 (GPIO 21) and the WS2812B DIN pad to prevent impedance mismatch and GPIO damage.