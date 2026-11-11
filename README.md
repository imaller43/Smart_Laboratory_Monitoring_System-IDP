# Smart Laboratory Safety Monitoring System (IDP)

An IoT-based smart laboratory safety monitoring system built around an **ESP32-S3** main sensing node, **ESP32 Super Mini** LoRa emergency fallback gateway, **EMQX/Mosquitto MQTT + Node-RED + InfluxDB/MySQL** cloud backend, and a **React/Vue** web dashboard hosted on Netlify.

---

## 📁 Repository Structure

| Folder | Contents | Owner |
|---|---|---|
| [`docs/`](./docs) | Proposal tasks, blueprint, hardware/firmware architecture specs, survey data, and `.drawio` diagrams | All Team Members |
| [`firmware/`](./firmware) | ESP32-S3 C++/FreeRTOS sensor & fusion logic code + ESP32 Super Mini LoRa gateway | Hardware & Control Logic Engineers |
| [`backend/`](./backend) | Docker Compose, MQTT broker config, Node-RED flows, database setup, Telegram alerting | Cloud Backend Engineer |
| [`frontend/`](./frontend) | Web monitoring dashboard deployed to Netlify | Frontend / UI Developer |

Each folder also has its own tailored `.gitignore` (on top of the root `.gitignore`) so every role stays protected from committing build artifacts, `node_modules/`, or secret tokens/credentials.

---

## 🚀 Team Git Workflow (Single Branch: `main`)

1. **Always pull latest changes before starting work:**
   ```bash
   git pull origin main
   ```
2. **Only edit files inside your assigned folder** (`firmware/`, `backend/`, `frontend/`, or `docs/`).
3. **Pull with rebase before pushing** (in case another teammate pushed while you were working):
   ```bash
   git pull --rebase origin main
   git add .
   git commit -m "role: short description of changes"
   git push origin main
   ```
4. **Never commit secrets:** Keep Wi-Fi credentials, Telegram Bot tokens, and DB passwords inside `.env`, `secrets.h`, `config.json`, or `flows_cred.json` (all already ignored by `.gitignore`).
