# EasyEDA Standard PCB Design & JLCPCB Ordering Guide
**Project:** Smart Laboratory Safety Monitoring System (IDP) — **Main Lab Node Carrier PCB**
**Board Dimensions:** `98.0 mm × 85.0 mm` (2-Layer FR-4, 1.6 mm thickness — qualifies for JLCPCB's **\$2 / 5-board promo** under 100×100 mm)
**Target MCU:** 44-Pin `ESP32-S3-DevKitC-1 N16R8` (Dual-width compatible: fits both official `25.40 mm` and `22.86 mm` clone header spacing)

---

## 1. Generated Files in `docs/pcb/`

| File | Purpose |
| :--- | :--- |
| [`IDP_Main_Node_Carrier.sch`](./IDP_Main_Node_Carrier.sch) | Self-contained EAGLE v7.7 XML **Schematic** (all 27 parts + 39 verified nets matching [`firmware/main_node/pins.h`](../../firmware/main_node/pins.h) & [`docs/hardware.md`](../hardware.md)) |
| [`IDP_Main_Node_Carrier.brd`](./IDP_Main_Node_Carrier.brd) | Pre-placed `98 × 85 mm` **PCB Layout** with M3 mounting holes, silkscreen warnings, star-ground zoning, and complete ratsnest/airwires |
| [`IDP_Main_Node_Eagle_Import.zip`](./IDP_Main_Node_Eagle_Import.zip) | Zipped archive (if importing into EasyEDA Pro or KiCad later) |
| [`JLCPCB_LCSC_BOM.csv`](./JLCPCB_LCSC_BOM.csv) | Ready-to-order Bill of Materials with **LCSC C-codes** for through-hole female headers, screw terminals, decoupling capacitors, and 330Ω resistor |

---

## 2. Step-by-Step: Importing & Routing in EasyEDA Standard (5 Minutes)

### Step 1: Import the Schematic & Pre-Placed PCB
1. Open **EasyEDA Standard** in your browser (`https://easyeda.com/editor`) and sign in.
2. Create a new project: **File → New → Project...** (name it `IDP-Main-Node-PCB`).
3. Click **File → Import → EAGLE...** from the top menu bar.
4. Select **both** [`IDP_Main_Node_Carrier.sch`](./IDP_Main_Node_Carrier.sch) and [`IDP_Main_Node_Carrier.brd`](./IDP_Main_Node_Carrier.brd), ensure **"Import File and Extract Libs"** (or Schematic + PCB) is checked, and click **Import**.
5. EasyEDA Standard will open both the wired schematic and the pre-placed `98 × 85 mm` PCB with blue ratsnest airwires ready to route!

### Step 2: Set Track Width Rules (Critical for 5V High-Current & Star Ground)
Before auto-routing or manual routing in the PCB tab:
1. Go to **Design → Design Rule...** (or press `Ctrl+Shift+U` for Autorouter setup).
2. Use these trace widths:
   * **Signal Nets (`default` — I2C, SPI, UART, GPIO triggers):** Track Width = **`0.30 mm`** (`12 mil`), Clearance = **`0.254 mm`** (`10 mil`).
   * **Power Nets (`+12V_RAW`, `+12V_SW`, `+5V_MAIN`, `+3V3_LORA`, `+3V3_ESP`, `GND`):** Track Width = **`0.80 mm` to `1.00 mm`** (`32–40 mil`) so high-current 5V fan/atomizer/LED surges never cause voltage droop.

### Step 3: Keep Antenna Keep-Out & Route Traces
1. **Wi-Fi Antenna Keep-Out Zone (Top Center):** Keep copper pours and traces **out of the top-center strip (`X = 34..62 mm`, `Y = 71..85 mm`)** right above `U1_L`/`U1_R` Pin 1 so the ESP32-S3's 2.4 GHz Wi-Fi antenna radiates cleanly.
2. **Route Power First, Signals Second:**
   * Either route manually (`W` hotkey for Top Red Layer, `B` + `W` for Bottom Blue Layer) **or** click **Route → Auto Router... → Run** (using EasyEDA's built-in cloud/local router).
3. **Pour Bottom `GND` Plane Anchored at Star Ground (`J_BUCK_OUT` Pin 2):**
   * Select the **Copper Area** tool (`E` hotkey), choose **BottomLayer**, set Net to **`GND`**, draw around the `98 × 85 mm` board outline (avoiding the top-center Wi-Fi antenna strip), and press **`Shift+B`** to rebuild copper pours.

### Step 4: Run DRC & Click "Order at JLCPCB"
1. In the left Design Manager panel, click **DRC Errors → Refresh**. Confirm **0 errors**.
2. Click **View → 3D Viewer** to inspect your board visually.
3. Click **Fabrication → One-click Order PCB/SMT** (the **"Order at JLCPCB"** icon on the top toolbar):
   * **Layers:** `2` | **Dimensions:** `98 × 85 mm` | **PCB Qty:** `5` | **Thickness:** `1.6 mm`
   * Choose **Bare PCB Only** (~USD \$2 + shipping) and order the through-hole headers/terminals/caps from LCSC (using [`JLCPCB_LCSC_BOM.csv`](./JLCPCB_LCSC_BOM.csv)) or local electronics shops (Cytron / Shopee) to hand-solder in the lab!

---

## 3. Built-In Hardware Protections & Smart Footprints on This PCB

1. **Dual-Width 44-Pin ESP32-S3 Socket (`U1_L` + `U1_R` / `U1_R_ALT`):**
   * Official Espressif `ESP32-S3-DevKitC-1` boards measure **`25.40 mm`** (`1.00"`) between rows (`U1_L` to `U1_R`), whereas some `YD-ESP32-S3` clones measure **`22.86 mm`** (`0.90"`).
   * We included an inner auxiliary row `U1_R_ALT` at `22.86 mm`. **Before soldering**, plug the two `1x22P` female headers onto your physical ESP32-S3 legs first, drop the assembly into whichever PCB row aligns (`U1_R` or `U1_R_ALT`), and solder! *(Leave the unused row empty).*
2. **Polarity-Safe 1.3" SH1106 I2C OLED Headers (`J_OLED_VCC` vs `J_OLED_GND`):**
   * 1.3" SH1106 OLED modules exist in two pin orders (`VCC-GND-SCL-SDA` and `GND-VCC-SCL-SDA`).
   * Check the silkscreen on your OLED module and solder a single `1x4P` female header into **`J_OLED_VCC`** (if Pin 1 is `VCC`/`VDD`) **OR** **`J_OLED_GND`** (if Pin 1 is `GND`). Never populate both at once.
3. **Dual LoRa SX1278 433 MHz Footprint (`U_LORA_L`/`U_LORA_R` + `J_LORA_UNI`):**
   * Supports **both** standard `22.86 mm` dual-row `Ra-02` 2.54 mm breakout adapters (`U_LORA_L` + `U_LORA_R`) **and** any single-row `1x8P` SX1278 breakout or jumper harness (`J_LORA_UNI`), with `C4` (`100 nF`) placed right beside the `3.3V` RF pin.
4. **Cross-Wired Radar UART (`J_RADAR`):**
   * `J_RADAR` Pin 1 (`RADAR_TX`) routes directly on the PCB to `ESP32 GPIO 18` (`PIN_RADAR_RX`), and Pin 2 (`RADAR_RX`) routes to `ESP32 GPIO 17` (`PIN_RADAR_TX`) — use a straight-through 5-pin cable with no manual wire crossing!

---

## 4. Complete Pin-to-Pin Netlist Verification Table

| Subsystem | PCB Connector | Connector Pin | Net Name | ESP32-S3 Pin (`U1_L` / `U1_R`) | Notes |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **12V Power In** | `J_12V_IN` | Pin 1 / Pin 2 | `+12V_RAW` / `GND` | — | From 12V 2A/3A DC Adapter |
| **Master Switch** | `J_SW_MST` | Pin 1 / Pin 2 | `+12V_RAW` / `+12V_SW` | — | Series SPST Toggle Switch (`E-TEN1021`) |
| **LM2596S Buck** | `J_BUCK_IN` | Pin 1 / Pin 2 | `+12V_SW` / `GND` | — | Wire to LM2596S `IN+` / `IN-` |
| **5V Star Hub** | `J_BUCK_OUT` | Pin 1 / Pin 2 | `+5V_MAIN` / `GND` | `U1_L` Pin 21 (`5V0`) & `GND` | **Pre-tune LM2596S to `5.10V` BEFORE plugging in ESP32!** |
| **Fan/Atomizer Pwr** | `J_LOAD_5V` | Pin 1 / Pin 2 | `+5V_MAIN` / `GND` | — | Pin 1 (`+5V`) → Relay `COM1/COM2`; Pin 2 (`GND`) → Fan `-` & Atomizer `-` |
| **AMS1117-3.3V** | `U_AMS` | Pin 1 / 2 / 3 | `+5V_MAIN` / `GND` / `+3V3_LORA` | — | Decoupled by `C1` (`10µF`), `C2` (`100nF`), `C3` (`22µF`), `C4` (`100nF`) |
| **BME280 (I2C)** | `J_BME280` | 1 / 2 / 3 / 4 | `+3V3_ESP` / `GND` / `SCL` / `SDA` | `3V3`, `GND`, `GPIO 9`, `GPIO 8` | Address `0x76` / `0x77` |
| **SGP30 (I2C)** | `J_SGP30` | 1 / 2 / 3 / 4 | `+3V3_ESP` / `GND` / `SCL` / `SDA` | `3V3`, `GND`, `GPIO 9`, `GPIO 8` | Address `0x58` (1 Hz poll) |
| **1.3" OLED (I2C)** | `J_OLED_VCC` *or* `GND` | 1..4 | `+3V3_ESP` & `GND`, `SCL`, `SDA` | `3V3`, `GND`, `GPIO 9`, `GPIO 8` | Address `0x3C` (SH1106) — match Pin 1! |
| **HLK-LD2410C** | `J_RADAR` | 1 / 2 / 3 / 4 / 5 | `TX` / `RX` / `NC` / `GND` / `+5V_MAIN` | `GPIO 18` (`RX`), `GPIO 17` (`TX`) | Cross-wired on PCB (`256000` baud) |
| **IR Flame** | `J_FLAME` | 1 / 2 / 3 / 4 | `+3V3_ESP` / `GND` / `DO` / `NC` | `GPIO 4` (`U1_L` Pin 4) | 3.3V powered for safe 3.3V `DO` logic |
| **2-Ch 5V Relay** | `J_RELAY` | 1 / 2 / 3 / 4 | `GND` / `IN1` / `IN2` / `+5V_MAIN` | `GPIO 5` (Fan), `GPIO 6` (Atomizer) | Active-LOW (`pins.h`) |
| **Active Buzzer** | `J_BUZZER` | 1 / 2 / 3 | `+3V3_ESP` / `IO` / `GND` | `GPIO 7` (`U1_L` Pin 7) | **Strictly 3.3V VCC** so PNP turns OFF cleanly |
| **WS2812B LED** | `J_LED` (+ `R1`, `C5`) | 1 / 2 / 3 | `+5V_MAIN` / `DIN` / `GND` | `GPIO 21` (`U1_R` Pin 18) via `330Ω` | Protected by `C5` (`1000µF`) + `R1` (`330Ω`) |
| **LoRa SX1278** | `J_LORA_UNI` / `U_LORA` | `3V3, GND, SPI` | `+3V3_LORA` + SPI Bus | `SCK=12, MISO=13, MOSI=11, CS=10, RST=14, DIO0=15` | Powered strictly by isolated `AMS1117-3.3V` |
