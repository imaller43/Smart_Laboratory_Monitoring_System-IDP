# Cloud Backend (Eclipse Mosquitto 2.0 + Node-RED 4.0 + InfluxDB v2.7 + Cloudflare Tunnel)

**Owner:** Cloud Backend Engineer  
**Domain:** `ikmaliman.my` (Cloudflare Zero Trust Tunnel)  
**Host:** Microsoft Azure `Standard B2ats v2` (2 vCPUs, 1 GiB RAM + 2 GB Swap, `Ubuntu 24.04 LTS`)

---

## Folder Structure

```text
backend/
├── docker-compose.yml       # Mosquitto 2.0, InfluxDB 2.7, Node-RED 4.0, cloudflared
├── .env.example             # Environment & secrets template (copy to .env)
├── SETUP_AZURE.md           # Full step-by-step Azure VM + Cloudflare deployment runbook
├── mosquitto/
│   ├── mosquitto.conf       # TCP :1883 + WebSocket :8083 (/mqtt), auth & ACL enabled
│   └── acl.conf             # Least-privilege MQTT topic ACL rules
├── nodered/
│   ├── Dockerfile           # Node-RED 4.0 + InfluxDB v2, Telegram & firebase-admin (FCM)
│   ├── settings.js          # /admin isolation, adminAuth, dynamic CORS, FCM helper
│   └── flows.json           # 3-tab flow: Telemetry/Alerts, REST API, State Simulator
├── scripts/
│   ├── init-secrets.sh      # Generates gitignored Mosquitto passwd & Node-RED creds from .env
│   └── backup.sh            # Exports InfluxDB v2 bucket + configs into backups/
└── .gitignore
```

---

## Quick Start (Azure VM or Local Docker)

See **[SETUP_AZURE.md](./SETUP_AZURE.md)** for full Azure `Standard B2ats v2`, 2 GB swap, Docker, Cloudflare Zero Trust, Telegram, and Firebase FCM instructions.

```bash
cd backend
cp .env.example .env
nano .env                       # Fill in Cloudflare token, passwords, tokens, CORS_ORIGIN
bash scripts/init-secrets.sh    # Generates gitignored Mosquitto passwd & Node-RED credentials
docker compose up -d --build
docker compose ps
```

> **Never commit** `.env`, `mosquitto/passwd`, `nodered/flows_cred.json`, or `nodered/secrets/firebase-service-account.json` (all already blocked by `.gitignore`).

---

## Subdomain Routing (`ikmaliman.my`)

| Subdomain | Target | Access Control |
| :--- | :--- | :--- |
| `wss://lab-mqtt.ikmaliman.my/mqtt` | `mosquitto:8083` | Mosquitto Username/Password + Topic ACL (ESP32 Main & Gateway) |
| `https://lab-api.ikmaliman.my/api/*` | `nodered:1880` | Dynamic CORS (`CORS_ORIGIN` in `.env`) for Netlify Frontend |
| `wss://lab-api.ikmaliman.my/ws/telemetry` | `nodered:1880` | Real-time 1 Hz live telemetry WebSocket for Netlify Frontend |
| `https://lab-nodered.ikmaliman.my/admin` | `nodered:1880` | Cloudflare Zero Trust Access (OTP) + Node-RED `adminAuth` |
| `https://lab-influx.ikmaliman.my` | `influxdb:8086` | Cloudflare Zero Trust Access (OTP) + InfluxDB v2 Login |

---

## Frontend API & WebSocket Contract (for Netlify Dashboard)

Base URL: **`https://lab-api.ikmaliman.my`**

| Endpoint | Method | Description |
| :--- | :--- | :--- |
| `/ws/telemetry` | `WSS` | Real-time JSON telemetry stream (1 Hz during hazards, 3 min during normal state) |
| `/api/health` | `GET` | Backend health, MQTT connection state, FCM readiness, active nodes |
| `/api/latest` | `GET` | Latest cached sensor snapshot (optional `?node=lab_main_s3`) |
| `/api/history?range=1h` | `GET` | Aggregated InfluxDB v2 time-series (`range`: `15m`, `1h`, `6h`, `24h`, `7d`, `30d`; optional `&node=lab_main_s3`) |
| `/api/alerts?range=7d&limit=50` | `GET` | Historical non-normal safety events (`state > 0`) from InfluxDB v2 |
| `/api/fcm/register` | `POST` | Subscribe a browser FCM token to push alerts (`{ "token": "<fcm_token>" }`) |
| `/api/fcm/unregister` | `POST` | Unsubscribe a browser FCM token (`{ "token": "<fcm_token>" }`) |
| `/api/ota/trigger` | `POST` | Publish `esp32/ota_trigger` (requires `X-API-Key` header + `STATE_NORMAL` safety interlock) |

### Telemetry JSON Payload Format
```json
{
  "node": "lab_main_s3",
  "state": 0,
  "state_label": "NORMAL",
  "temp": 25.4,
  "humidity": 56.8,
  "pressure": 1011.2,
  "tvoc": 42,
  "eco2": 415,
  "flame": 0,
  "presence": true,
  "distance_cm": 118,
  "uptime_s": 3600,
  "received_at": "2026-11-27T08:55:00.000Z"
}
```
