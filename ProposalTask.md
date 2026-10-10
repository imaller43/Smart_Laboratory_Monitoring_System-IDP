# 📋 IDP Defend Proposal — Task Assignment Guide
**Project:** IoT-Based Smart Laboratory Safety Monitoring System
**Prepared by:** Project Leader
**Last Updated:** 2026-10-10

> This document breaks down every section of the defend proposal into clear, actionable tasks. Each teammate should read their assigned section carefully, understand what is expected, and complete it before the deadline.

---

## ✅ Section Ownership Overview

| # | Section | Owner | Status |
|---|---------|-------|--------|
| 1 | Project Proposal (Title, Details, Course) | All Members | ✅ Managed by leader |
| 2 | Project Summary | TBD | ⬜ Pending |
| 3 | Problem Statement & Objective | TBD | ⬜ Pending |
| 4 | Scope of Work & Project Methodology | TBD | ⬜ Pending |
| 5 | Expected Outcome | TBD | ⬜ Pending |
| 6 | References | TBD | ⬜ Pending |
| 7 | Project Organisation Chart | TBD | ⬜ Pending |
| 8 | Project Planning (Gantt Chart) | TBD | ⬜ Pending |

---

## 📌 Section 1 — Project Proposal
> **Status: ✅ Managed by the project leader. No action needed from teammates.**

This covers the cover page / front matter of the proposal document, including:
- **Project Title:** IoT-Based Smart Laboratory Safety Monitoring System
- **Student Details:** Names, student IDs, programme
- **Course Information:** Course code, supervisor name, semester

---

## 📌 Section 2 — Project Summary
> **Word Limit: Maximum 200 words**
> **Assigned to:** *(fill in name)*

### 🎯 What You Need to Do
Write a concise **standalone paragraph** (no bullet points) that gives any reader — including someone unfamiliar with this project — a clear picture of what the project is about. Think of it as an "elevator pitch" for your proposal.

### 📝 What to Cover (in order)
1. **The Problem (1–2 sentences):** Briefly mention why laboratory safety monitoring is critical and what is currently lacking (manual monitoring, delayed alerts, no remote access).
2. **The Solution (2–3 sentences):** Introduce your system — an ESP32-based IoT monitoring device that reads temperature, humidity, air quality (TVOC/eCO2), and flame sensors in real time.
3. **Key Features (2–3 sentences):** Highlight the most impressive aspects — sensor fusion for intelligent alert logic, LoRa radio fallback for Wi-Fi outages, Telegram push notifications, and a cloud dashboard.
4. **How It Will Be Validated (1–2 sentences):** State that the prototype will be tested under simulated hazard conditions (gas leak, fire) to evaluate response time, accuracy, and reliability.

### 💡 Tips
- Do NOT use bullet points — write it as flowing prose.
- Avoid technical jargon that won't be understood by a general reader; keep it accessible.
- Stay within **200 words**. Use a word counter tool such as [wordcounter.net](https://wordcounter.net/).
- Read the `BluePrint.md` Sections 1–3 to understand the full project context before writing.

---

## 📌 Section 3 — Problem Statement & Objective
> **Assigned to:** *(fill in name)*

### 🎯 What You Need to Do
This section has **two sub-parts**. You must write both.

---

### Part A — Problem Statement
Explain the real-world problem that this project is trying to solve. This is your justification for why the project is necessary.

**Structure to follow:**
1. **Context:** Describe how laboratory environments are high-risk zones due to the presence of chemicals, volatile gases, flammable materials, and expensive equipment.
2. **Current Gaps:** Explain that most laboratories (especially in academic institutions) rely on manual inspection or basic smoke detectors, which are reactive (not proactive), have no remote monitoring capability, and cannot distinguish between different types of hazards (e.g., a gas leak vs. a broken sensor).
3. **Consequence:** If hazards are not detected early, they can lead to property damage, injury, or loss of life.
4. **The Need:** Therefore, there is a need for an intelligent, automated, real-time monitoring system with remote alerting capabilities.

**Length:** Aim for **3–4 paragraphs** or approximately 200–300 words.

---

### Part B — Objectives
List the **specific, measurable goals** of this project. Objectives must be concrete — do NOT write vague goals like "to improve safety."

**Suggested objectives (expand each into a sentence):**
1. To design and develop a low-cost IoT-based environmental monitoring prototype using the ESP32 microcontroller integrated with temperature, humidity, air quality (TVOC/eCO2), and flame sensors.
2. To implement a sensor fusion logic matrix that can intelligently classify hazard states (Normal, Minor Leak, Severe Leak, Confirmed Fire, Sensor Fault) and trigger appropriate automated responses.
3. To establish a cloud-based data pipeline using MQTT, Node-RED, and a time-series database for real-time data logging and remote monitoring via a web dashboard.
4. To integrate a remote alerting system via Telegram Bot API and a LoRa radio fallback to ensure emergency notifications are delivered even during Wi-Fi outages.
5. To evaluate the system's performance in terms of sensor accuracy, alert response time, and reliability under simulated hazard conditions.

**Format:** Numbered list. Each objective starts with "To + verb."

---

## 📌 Section 4 — Scope of Work & Project Methodology
> **Assigned to:** *(fill in name — recommended: Hardware/Control Logic person)*

### 🎯 What You Need to Do
This is one of the most detailed and technical sections. You must clearly describe **what** the project covers and **how** you will build it step by step.

---

### Part A — Scope of Work
Define the **boundaries** of the project — what IS included and what is NOT included.

**What to include in scope:**
- Hardware design and assembly of the ESP32-based monitoring unit with all listed sensors and actuators.
- Firmware development in C++ (sensor reading, sensor fusion logic, LittleFS configuration portal, MQTT publishing, LoRa fallback).
- Cloud backend deployment (Mosquitto MQTT broker on a VM, Node-RED middleware, InfluxDB/MySQL database, REST API).
- Frontend web dashboard development (React/Vue) hosted on Netlify.
- Telegram Bot integration for push notifications.
- System testing under simulated lab hazard conditions (gas release, heat source, flame).

**What is OUT of scope (limitations):**
- The system is a prototype and is not designed for industrial-grade or legally certified safety compliance.
- The fire suppression (ultrasonic atomizer) is a simulation only — not a real sprinkler system.
- The LoRa gateway is a secondary unit and does not support full bidirectional communication.

---

### Part B — Project Methodology
Describe your development approach. Use the **hardware/software development lifecycle** as your framework.

**Write a brief introduction** saying you will follow a structured prototyping methodology, broken into phases:

#### Phase 1 — Research & Requirement Analysis (Week 1–2)
- Study existing laboratory safety systems and their limitations.
- Identify sensor types, microcontroller options, and IoT platforms.
- Finalise component list and prepare the hardware budget.
- Define all system states and alert conditions (Normal, Warning, Critical, Emergency, Fault).

#### Phase 2 — Hardware Design & Assembly (Week 3–5)
- Design the circuit schematic (I2C bus layout for BME280, SGP30/CCS811; UART for mmWave radar; GPIO for flame sensor, relay, buzzer, ARGB LED).
- Assemble components on a breadboard for initial testing.
- Verify sensor readings using serial output before integration.
- Design and fabricate the physical enclosure/diorama.

#### Phase 3 — Firmware Development (Week 5–8)
- Write ESP32 firmware in C++ using the Arduino framework.
- Implement the LittleFS Captive Web Portal for Wi-Fi/MQTT configuration.
- Code the sensor fusion state machine (States 0–4).
- Integrate MQTT publishing and LoRa radio fallback.
- Test all firmware modules independently (unit testing).

#### Phase 4 — Cloud Backend Development (Week 7–10)
- Set up a Linux Virtual Machine (cloud server).
- Deploy Mosquitto MQTT broker (or EMQX) using Docker.
- Build Node-RED flows for: data routing, Telegram alert formatting, database write, and REST API endpoints.
- Set up InfluxDB or MySQL for time-series data storage.

#### Phase 5 — Frontend Dashboard Development (Week 9–12)
- Design the UI layout and wireframes.
- Build the interactive dashboard using React or Vue.js.
- Connect the frontend to the Node-RED REST API.
- Deploy to Netlify.

#### Phase 6 — Integration, Testing & Validation (Week 12–14)
- Perform full end-to-end integration testing.
- Simulate each hazard state and measure response time.
- Validate sensor accuracy against reference values.
- Test LoRa fallback by disabling primary Wi-Fi.
- Document test results.

#### Phase 7 — Documentation & Final Presentation (Week 14–15)
- Finalise all technical documentation.
- Prepare the defend presentation slides.
- Rehearse the demonstration using the physical prototype.

> **Tip:** You can include a simple flowchart or diagram to visualise the methodology if desired. It adds clarity for the evaluators.

---

## 📌 Section 5 — Expected Outcome
> **Assigned to:** *(fill in name)*

### 🎯 What You Need to Do
Describe what you **expect to have achieved** by the end of the project. This is forward-looking — write about what the final product and its capabilities will look like if everything goes to plan.

### 📝 What to Cover

#### 1. Physical Deliverable
Describe the physical prototype: a fully assembled IoT monitoring unit housed in a professional casing, mounted in a lab diorama, with sensors, actuators, OLED display, and LED indicators all working together.

#### 2. Functional Deliverables
Describe what each part of the system will be able to do:
- The ESP32 unit can continuously monitor environmental conditions and classify hazard states in real time using sensor fusion.
- The system will autonomously activate exhaust fans, atomizer sprinklers, and buzzers based on detected conditions — without human intervention.
- Administrators will receive instant Telegram push notifications during emergencies, including the hazard type and severity level.
- In case of Wi-Fi failure, the LoRa radio fallback will ensure emergency alerts are still transmitted to the cloud via an alternate gateway.

#### 3. Cloud & Dashboard Deliverable
Describe the web dashboard: a live, interactive dashboard accessible from any browser showing real-time sensor readings, historical trend graphs, and a system status overview.

#### 4. Performance Targets (Quantify where possible)
- Alert response time: **< 5 seconds** from hazard detection to Telegram notification.
- Sensor reading interval: **every 5 seconds** (or configurable).
- System uptime target: **> 95%** during testing period.
- Dashboard data refresh rate: **< 10 seconds** latency.

#### 5. Broader Impact
Briefly state the potential for this project to be adopted in real academic laboratories to improve safety culture, reduce manual monitoring workload, and serve as a foundation for future AI-enhanced safety systems.

**Length:** Aim for **2–4 paragraphs** or approximately 200–300 words.

---

## 📌 Section 6 — References
> **Assigned to:** All members (each member submits references relevant to their section)

### 🎯 What You Need to Do
Compile all academic and technical references used throughout the proposal. References must be properly formatted.

### 📝 Formatting Standard
Use **IEEE citation format** (commonly used in engineering proposals):

```
[1] A. Author, "Title of Paper," Journal Name, vol. X, no. Y, pp. ZZ–ZZ, Month Year.
[2] B. Author, Title of Book, Edition. City, Country: Publisher, Year.
[3] C. Author. "Title of Webpage." Website Name. [Online]. Available: https://url. [Accessed: DD Mon. YYYY].
```

### 📚 Suggested Reference Topics to Look Up
Each teammate should find **at least 2 references** relevant to their section:

| Topic to Search | Relevant Section |
|-----------------|-----------------|
| IoT-based laboratory monitoring systems | Summary, Problem Statement |
| ESP32 microcontroller in IoT applications | Methodology, Hardware |
| Air quality / TVOC sensor accuracy (SGP30, CCS811) | Methodology |
| MQTT protocol for IoT data transmission | Methodology, Cloud |
| Machine learning for anomaly detection in sensor data | Expected Outcome |
| LoRa/LoRaWAN for emergency communication systems | Methodology |
| Node-RED as an IoT middleware platform | Cloud Backend |
| Smart building/lab safety systems — literature review | Problem Statement |

> **Tip:** Use [Google Scholar](https://scholar.google.com), [IEEE Xplore](https://ieeexplore.ieee.org), and [ScienceDirect](https://www.sciencedirect.com) to find peer-reviewed papers. Aim for references from **2018–2025** to ensure they are recent and relevant.

---

## 📌 Section 7 — Project Organisation Chart
> **Assigned to:** *(fill in name — recommended: Project Leader)*

### 🎯 What You Need to Do
Create a visual **organisation chart** (org chart) that shows the team structure and each member's role in the project.

### 📝 What to Include

#### Team Roles (Based on Blueprint)
The org chart should reflect these 4 roles:

```
                    [Project Leader]
                          |
    ┌─────────────────────┼─────────────────────┐
    |                     |                     |
[Hardware &        [Control Logic        [Cloud Backend
Embedded Eng.]      Developer]            Engineer]
                                               |
                                       [Frontend / UI
                                         Developer]
```

> If your team has only 4 members and some handle multiple roles, show that in the chart (e.g., one box can list 2 roles).

#### For Each Role, State Responsibilities
Below the chart, write a **brief responsibility list** for each role:

- **Project Leader:** Overall coordination, timeline management, documentation, and section reviews.
- **Hardware & Embedded Engineer:** Circuit design, sensor wiring, ESP32 programming, LittleFS captive portal, LoRa module integration.
- **Control Logic Developer:** Sensor fusion state machine design, hardcoded emergency rules (C++ fallback logic), edge-side testing.
- **Cloud Backend Engineer:** VM setup, Docker deployment of MQTT & Node-RED, database configuration, REST API development, Telegram bot integration.
- **Frontend / UI Developer:** Dashboard UI design, React/Vue implementation, Netlify deployment, API integration.

### 🛠️ Tools to Create the Chart
You can create the org chart using:
- **Microsoft Word** — Insert → SmartArt → Hierarchy
- **draw.io (free)** — [app.diagrams.net](https://app.diagrams.net)
- **Canva** — Search "Org Chart" template
- **PowerPoint** — SmartArt → Hierarchy

Export as an image (PNG) and embed it in the proposal document.

---

## 📌 Section 8 — Project Planning (Gantt Chart, Week 1–15)
> **Assigned to:** *(fill in name — recommended: Project Leader or Methodology writer)*

### 🎯 What You Need to Do
Create a **Gantt Chart** that maps out all project activities across **Week 1 to Week 15**. This shows the evaluator that the team has a realistic and organised plan.

### 📝 Suggested Activity Timeline

| # | Activity | Week 1 | Week 2 | Week 3 | Week 4 | Week 5 | Week 6 | Week 7 | Week 8 | Week 9 | Week 10 | Week 11 | Week 12 | Week 13 | Week 14 | Week 15 |
|---|----------|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 | Research & Literature Review | ███ | ███ | | | | | | | | | | | | | |
| 2 | Requirement & Component Finalisation | ███ | ███ | | | | | | | | | | | | | |
| 3 | Circuit Design & Schematic | | | ███ | ███ | | | | | | | | | | | |
| 4 | Hardware Assembly & Sensor Testing | | | | ███ | ███ | ███ | | | | | | | | | |
| 5 | ESP32 Firmware Development | | | | | ███ | ███ | ███ | ███ | | | | | | | |
| 6 | Sensor Fusion Logic Implementation | | | | | | ███ | ███ | ███ | | | | | | | |
| 7 | Cloud Backend Setup (VM, MQTT, Node-RED) | | | | | | | ███ | ███ | ███ | ███ | | | | | |
| 8 | Database & REST API Development | | | | | | | | ███ | ███ | ███ | | | | | |
| 9 | Frontend Dashboard Development | | | | | | | | | ███ | ███ | ███ | ███ | | | |
| 10 | System Integration | | | | | | | | | | | ███ | ███ | ███ | | |
| 11 | Testing & Validation | | | | | | | | | | | | ███ | ███ | ███ | |
| 12 | Documentation & Report Writing | | | | ███ | | | | | | | | | ███ | ███ | |
| 13 | Final Presentation Preparation | | | | | | | | | | | | | | ███ | ███ |

> **Legend:** ███ = Active period for that task

### 🛠️ Tools to Create the Gantt Chart
- **Microsoft Excel** — Use a stacked bar chart to create a proper Gantt chart.
- **Microsoft Project** — If available.
- **GanttProject (free)** — [ganttproject.biz](https://www.ganttproject.biz)
- **Notion / ClickUp / Trello** — Gantt view plugins available.
- Alternatively, create a simple **table in Word** using the template above and shade the cells manually.

### ⚠️ Important Notes
- Ensure that **no critical task is isolated to a single week** — builds should have overlapping phases.
- Add a row for **Proposal Submission / Milestones** if required by your supervisor.
- The Gantt chart should be **clearly readable** when printed — use landscape orientation if needed.

---

## 🗓️ Suggested Submission Deadlines (To Be Confirmed by Leader)

| Section | Assigned To | Draft Deadline | Review Deadline |
|---------|-------------|---------------|-----------------|
| Project Summary | TBD | TBD | TBD |
| Problem Statement & Objective | TBD | TBD | TBD |
| Scope of Work & Methodology | TBD | TBD | TBD |
| Expected Outcome | TBD | TBD | TBD |
| References | All | TBD | TBD |
| Organisation Chart | TBD | TBD | TBD |
| Gantt Chart | TBD | TBD | TBD |

> **Leader's Note:** Once all sections are drafted, please send them to the project leader for a final review and compilation into the unified proposal document.

---

*This task guide was generated based on the project blueprint. Refer to [BluePrint.md](./BluePrint.md) for full technical details.*
