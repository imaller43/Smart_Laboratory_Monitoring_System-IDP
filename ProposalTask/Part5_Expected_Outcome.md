# 📌 Part 5 — Expected Outcome
**Project:** IoT-Based Smart Laboratory Safety Monitoring System
**Last Updated:** 2026-10-10

---

> **Assigned to:** *(fill in name)*

## 🎯 What You Need to Do
Describe what you **expect to have achieved** by the end of the project. This is forward-looking — write about what the final product and its capabilities will look like if everything goes to plan.

**Length:** Aim for **2–4 paragraphs** or approximately 200–300 words.

---

## 📝 What to Cover

### 1. Physical Deliverable
Describe the physical prototype: a fully assembled IoT monitoring unit housed in a professional casing, mounted in a lab diorama, with sensors, actuators, OLED display, and LED indicators all working together.

### 2. Functional Deliverables
Describe what each part of the system will be able to do:
- The ESP32 unit can continuously monitor environmental conditions and classify hazard states in real time using sensor fusion.
- The system will autonomously activate exhaust fans, atomizer sprinklers, and buzzers based on detected conditions — without human intervention.
- Administrators will receive instant Telegram push notifications during emergencies, including the hazard type and severity level.
- In case of Wi-Fi failure, the LoRa radio fallback will ensure emergency alerts are still transmitted to the cloud via an alternate gateway.

### 3. Cloud & Dashboard Deliverable
Describe the web dashboard: a live, interactive dashboard accessible from any browser showing real-time sensor readings, historical trend graphs, and a system status overview.

### 4. Performance Targets (Quantify where possible)
| Metric | Target |
|--------|--------|
| Alert response time | **< 5 seconds** from hazard detection to Telegram notification |
| Sensor reading interval | **Every 5 seconds** (or configurable) |
| System uptime | **> 95%** during testing period |
| Dashboard data refresh rate | **< 10 seconds** latency |

### 5. Broader Impact
Briefly state the potential for this project to be adopted in real academic laboratories to improve safety culture, reduce manual monitoring workload, and serve as a foundation for future AI-enhanced safety systems.

---

*Refer to [BluePrint.md](../BluePrint.md) for full project technical details.*
