# 📌 Part 7 — Project Organisation Chart
**Project:** IoT-Based Smart Laboratory Safety Monitoring System
**Last Updated:** 2026-10-10

---

> **Assigned to:** *(fill in name — recommended: Project Leader)*

## 🎯 What You Need to Do
Create a visual **organisation chart** (org chart) that shows the team structure and each member's role in the project. Export it as an image (PNG) and embed it into the final proposal document.

---

## 📝 Team Structure (Based on Blueprint)

The org chart should reflect these 4 roles and their hierarchy:

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

---

## 👥 Role Responsibilities

Below the org chart in the proposal, write a brief responsibility summary for each role:

| Role | Key Responsibilities |
|------|---------------------|
| **Project Leader** | Overall coordination, timeline management, documentation, and section reviews. |
| **Hardware & Embedded Engineer** | Circuit design, sensor wiring, ESP32 programming, LittleFS captive portal, LoRa module integration. |
| **Control Logic Developer** | Sensor fusion state machine design, hardcoded emergency rules (C++ fallback logic), edge-side testing. |
| **Cloud Backend Engineer** | VM setup, Docker deployment of MQTT & Node-RED, database configuration, REST API development, Telegram bot integration. |
| **Frontend / UI Developer** | Dashboard UI design, React/Vue implementation, Netlify deployment, API integration. |

---

## 🛠️ Recommended Tools to Create the Chart

| Tool | How to Use |
|------|-----------|
| **Microsoft Word** | Insert → SmartArt → Hierarchy |
| **draw.io (free)** | [app.diagrams.net](https://app.diagrams.net) — drag & drop org chart |
| **Canva** | Search "Org Chart" template — easy and visual |
| **PowerPoint** | Insert → SmartArt → Hierarchy |

> **Export as PNG** and paste it into the proposal Word document.

---

*Refer to [BluePrint.md](../BluePrint.md) for full project technical details.*
