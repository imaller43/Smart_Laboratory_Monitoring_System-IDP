# 📌 Part 4 — Scope of Work & Project Methodology
**Project:** IoT-Based Smart Laboratory Safety Monitoring System
**Last Updated:** 2026-10-10

---

> **Assigned to:** *(fill in name — recommended: Hardware/Control Logic person)*

## 🎯 What You Need to Do
This is one of the most detailed and technical sections. You must clearly describe **what** the project covers and **how** you will build it step by step.

---

## Part A — Scope of Work

Define the **boundaries** of the project — what IS included and what is NOT included.

### ✅ What is IN Scope:
- Hardware design and assembly of the ESP32-based monitoring unit with all listed sensors and actuators.
- Firmware development in C++ (sensor reading, sensor fusion logic, LittleFS configuration portal, MQTT publishing, LoRa fallback).
- Cloud backend deployment (Mosquitto MQTT broker on a VM, Node-RED middleware, InfluxDB/MySQL database, REST API).
- Frontend web dashboard development (React/Vue) hosted on Netlify.
- Telegram Bot integration for push notifications.
- System testing under simulated lab hazard conditions (gas release, heat source, flame).

### ❌ What is OUT of Scope (Limitations):
- The system is a prototype and is not designed for industrial-grade or legally certified safety compliance.
- The fire suppression (ultrasonic atomizer) is a simulation only — not a real sprinkler system.
- The LoRa gateway is a secondary unit and does not support full bidirectional communication.

---

## Part B — Project Methodology

Describe your development approach. Use the **hardware/software development lifecycle** as your framework.

Write a brief introduction stating that you will follow a structured prototyping methodology, broken into the following phases:

---

### Phase 1 — Research & Requirement Analysis (Week 1–2)
- Study existing laboratory safety systems and their limitations.
- Identify sensor types, microcontroller options, and IoT platforms.
- Finalise component list and prepare the hardware budget.
- Define all system states and alert conditions (Normal, Warning, Critical, Emergency, Fault).

### Phase 2 — Hardware Design & Assembly (Week 3–5)
- Design the circuit schematic (I2C bus layout for BME280, SGP30/CCS811; UART for mmWave radar; GPIO for flame sensor, relay, buzzer, ARGB LED).
- Assemble components on a breadboard for initial testing.
- Verify sensor readings using serial output before integration.
- Design and fabricate the physical enclosure/diorama.

### Phase 3 — Firmware Development (Week 5–8)
- Write ESP32 firmware in C++ using the Arduino framework.
- Implement the LittleFS Captive Web Portal for Wi-Fi/MQTT configuration.
- Code the sensor fusion state machine (States 0–4).
- Integrate MQTT publishing and LoRa radio fallback.
- Test all firmware modules independently (unit testing).

### Phase 4 — Cloud Backend Development (Week 7–10)
- Set up a Linux Virtual Machine (cloud server).
- Deploy Mosquitto MQTT broker (or EMQX) using Docker.
- Build Node-RED flows for: data routing, Telegram alert formatting, database write, and REST API endpoints.
- Set up InfluxDB or MySQL for time-series data storage.

### Phase 5 — Frontend Dashboard Development (Week 9–12)
- Design the UI layout and wireframes.
- Build the interactive dashboard using React or Vue.js.
- Connect the frontend to the Node-RED REST API.
- Deploy to Netlify.

### Phase 6 — Integration, Testing & Validation (Week 12–14)
- Perform full end-to-end integration testing.
- Simulate each hazard state and measure response time.
- Validate sensor accuracy against reference values.
- Test LoRa fallback by disabling primary Wi-Fi.
- Document test results.

### Phase 7 — Documentation & Final Presentation (Week 14–15)
- Finalise all technical documentation.
- Prepare the defend presentation slides.
- Rehearse the demonstration using the physical prototype.

> **Tip:** You can include a simple flowchart or diagram to visualise the methodology if desired. It adds clarity for the evaluators.

---

*Refer to [BluePrint.md](../BluePrint.md) for full project technical details.*
