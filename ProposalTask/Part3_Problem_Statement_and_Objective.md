# 📌 Part 3 — Problem Statement & Objective
**Project:** IoT-Based Smart Laboratory Safety Monitoring System
**Last Updated:** 2026-10-10

---

> **Assigned to:** *(fill in name)*

## 🎯 What You Need to Do
This section has **two sub-parts**. You must write both.

---

## Part A — Problem Statement

Explain the real-world problem that this project is trying to solve. This is your justification for why the project is necessary.

### Structure to Follow:

1. **Context:** Describe how laboratory environments are high-risk zones due to the presence of chemicals, volatile gases, flammable materials, and expensive equipment.
2. **Current Gaps:** Explain that most laboratories (especially in academic institutions) rely on manual inspection or basic smoke detectors, which are reactive (not proactive), have no remote monitoring capability, and cannot distinguish between different types of hazards (e.g., a gas leak vs. a broken sensor).
3. **Consequence:** If hazards are not detected early, they can lead to property damage, injury, or loss of life.
4. **The Need:** Therefore, there is a need for an intelligent, automated, real-time monitoring system with remote alerting capabilities.

**Length:** Aim for **3–4 paragraphs** or approximately 200–300 words.

---

## Part B — Objectives

List the **specific, measurable goals** of this project. Objectives must be concrete — do NOT write vague goals like "to improve safety."

### Suggested Objectives (expand each into a full sentence):

1. To design and develop a low-cost IoT-based environmental monitoring prototype using the ESP32 microcontroller integrated with temperature, humidity, air quality (TVOC/eCO2), and flame sensors.
2. To implement a sensor fusion logic matrix that can intelligently classify hazard states (Normal, Minor Leak, Severe Leak, Confirmed Fire, Sensor Fault) and trigger appropriate automated responses.
3. To establish a cloud-based data pipeline using MQTT, Node-RED, and a time-series database for real-time data logging and remote monitoring via a web dashboard.
4. To integrate a remote alerting system via Telegram Bot API and a LoRa radio fallback to ensure emergency notifications are delivered even during Wi-Fi outages.
5. To evaluate the system's performance in terms of sensor accuracy, alert response time, and reliability under simulated hazard conditions.

**Format:** Numbered list. Each objective starts with "To + verb."

---

*Refer to [BluePrint.md](../BluePrint.md) for full project technical details.*
