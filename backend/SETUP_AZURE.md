# Microsoft Azure Cloud Backend Deployment Runbook (`ikmaliman.my`)

Step-by-step guide to deploy **Eclipse Mosquitto 2.0**, **InfluxDB v2.7**, **Node-RED 4.0**, and **Cloudflare Zero Trust Tunnel** on a **Microsoft Azure `Standard B2ats v2` (2 vCPUs, 1 GiB RAM)** Linux VM.

> **Expected RAM Usage:** ~540–620 MB active out of 1,024 MiB physical RAM (~400+ MB free headroom) + 2 GB swapfile as OOM safety net.

---

## Step 1 — Provision the Azure VM (`Standard B2ats v2`)

1. Open [Azure Portal](https://portal.azure.com) → **Virtual machines → Create → Azure virtual machine**.
2. **Basics tab:**
   - **Resource group:** Create new (e.g., `rg-idp-lab-safety`)
   - **Virtual machine name:** `lab-safety-backend`
   - **Region:** **`Southeast Asia` (Singapore)** or **`Malaysia West`** *(lowest latency for Malaysia)*
   - **Availability options:** `No infrastructure redundancy required`
   - **Security type:** `Standard` *(or `Trusted launch` default)*
   - **Image:** **`Ubuntu Server 24.04 LTS - x64 Gen2`**
   - **Size:** **`Standard_B2ats_v2`** *(2 vCPUs, 1 GiB memory — Free Services eligible)*
   - **Authentication type:** **SSH public key** (Username: `azureuser`, Generate new key pair → **Download private key** when prompted!)
   - **Inbound port rules:** Allow selected ports → check **ONLY `SSH (22)`** *(do NOT open HTTP 80, HTTPS 443, 1883, 8083, 1880, or 8086 — Cloudflare Tunnel connects outbound!)*.
3. **Disks tab:**
   - **OS disk size:** **64 GiB** (`P6` / `E6` Standard SSD LRS — Azure Free Tier includes up to 2 × 64 GiB Standard SSDs).
   - **OS disk type:** **Standard SSD (locally-redundant storage)** *(recommended over Standard HDD so the 2 GB swapfile and InfluxDB writes stay snappy)*.
4. **Networking tab:** Keep defaults (creates VNet, Subnet, Public IP, and NSG with SSH 22 only).
5. Click **Review + create → Create** and save the `.pem` SSH private key.

---

## Step 2 — Initial Ubuntu Server Setup (Timezone, 2 GB Swap, Docker & Firewall)

SSH into your Azure VM:
```bash
chmod 600 ~/Downloads/lab-safety-backend_key.pem
ssh -i ~/Downloads/lab-safety-backend_key.pem azureuser@<YOUR_AZURE_PUBLIC_IP>
```

### 2.1 Update OS & Set Malaysia Timezone
```bash
sudo apt update && sudo apt upgrade -y
sudo timedatectl set-timezone Asia/Kuala_Lumpur
```

### 2.2 Create a 2 GB Swapfile on the OS Disk (Important on 1 GiB RAM)
*(Note: `B2ats v2` has no temporary ephemeral disk, so placing the swapfile at `/swapfile` on the Standard SSD OS disk is persistent across reboots.)*
```bash
sudo fallocate -l 2G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
echo '/swapfile none swap sw 0 0' | sudo tee -a /etc/fstab
sudo tee /etc/sysctl.d/99-memory-tuning.conf > /dev/null <<'EOF'
vm.swappiness=10
vm.vfs_cache_pressure=50
EOF
sudo sysctl --system
free -h
```

### 2.3 Install Docker Engine + Docker Compose Plugin
```bash
curl -fsSL https://get.docker.com | sudo sh
sudo usermod -aG docker $USER
newgrp docker
docker --version && docker compose version
```

### 2.4 Enable Global Docker Log Rotation
```bash
sudo tee /etc/docker/daemon.json > /dev/null <<'EOF'
{
  "log-driver": "json-file",
  "log-opts": {
    "max-size": "10m",
    "max-file": "3"
  }
}
EOF
sudo systemctl restart docker
```

### 2.5 Enable Host Firewall (`ufw`) — SSH Only
```bash
sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw allow 22/tcp
sudo ufw --force enable
sudo ufw status
```

---

## Step 3 — Configure Cloudflare Zero Trust Tunnel (`ikmaliman.my`)

Create a **new dedicated tunnel** (separate from your Raspberry Pi 4 tunnel):

1. Open **Cloudflare Zero Trust Dashboard → Networks → Tunnels → Create a tunnel**.
2. Choose **Cloudflared** and name it **`azure-lab-safety`**.
3. Copy the **Docker Tunnel Token** (the long string starting with `eyJh...` after `--token `) — you will paste this into `backend/.env` in Step 4.
4. Add these **Public Hostname** routes **in this exact order** (order matters for `lab-api.ikmaliman.my` path matching!):

| # | Subdomain | Domain | Path | Service Type | URL |
| :-: | :--- | :--- | :--- | :--- | :--- |
| 1 | `lab-mqtt` | `ikmaliman.my` | `mqtt` | `HTTP` | `mosquitto:8083` |
| 2 | `lab-api` | `ikmaliman.my` | `api` | `HTTP` | `nodered:1880` |
| 3 | `lab-api` | `ikmaliman.my` | `ws` | `HTTP` | `nodered:1880` |
| 4 | `lab-api` | `ikmaliman.my` | *(leave blank)* | `HTTP_STATUS` | `404` |
| 5 | `lab-nodered` | `ikmaliman.my` | *(leave blank)* | `HTTP` | `nodered:1880` |
| 6 | `lab-influx` | `ikmaliman.my` | *(leave blank)* | `HTTP` | `influxdb:8086` |

> **Why Route #4 (`HTTP_STATUS 404`) matters:**
> It ensures Node-RED's `/admin` Flow Editor can **never** be reached through `lab-api.ikmaliman.my`, while `/api/*` and `/ws/*` remain accessible for your Netlify dashboard.

### Lock Down Admin UIs with Cloudflare Zero Trust Access
Go to **Zero Trust → Access → Applications → Add an application → Self-hosted** and create Access policies (e.g., **Allow → Emails: your team emails**, using One-Time PIN) for these **two admin subdomains ONLY**:
- `lab-nodered.ikmaliman.my` *(open at `https://lab-nodered.ikmaliman.my/admin`)*
- `lab-influx.ikmaliman.my`

> **CRITICAL:** Do **NOT** put Cloudflare Access login on `lab-mqtt.ikmaliman.my` (ESP32 cannot complete browser OTP login — Mosquitto username/password + ACL protects it) or on `lab-api.ikmaliman.my` (Netlify browser `fetch()`/`WebSocket` must reach it directly).

---

## Step 4 — Clone Project, Configure `.env` & Launch Stack

On your Azure VM:
```bash
git clone <YOUR_REPO_URL> ~/IDP-Research
cd ~/IDP-Research/backend
cp .env.example .env
```

Generate strong random secrets (copy these into `.env`):
```bash
echo "INFLUXDB_ADMIN_TOKEN=$(openssl rand -hex 32)"
echo "NODERED_CREDENTIAL_SECRET=$(openssl rand -hex 24)"
echo "API_ADMIN_KEY=$(openssl rand -hex 20)"
```

Edit `.env`:
```bash
nano .env
```
Fill in:
- `CLOUDFLARE_TUNNEL_TOKEN` (from Step 3)
- Strong passwords for Mosquitto (`MQTT_NODERED_PASS`, `MQTT_ESP32_MAIN_PASS`, `MQTT_ESP32_GATEWAY_PASS`)
- InfluxDB password & token (`INFLUXDB_ADMIN_PASSWORD`, `INFLUXDB_ADMIN_TOKEN`)
- Node-RED admin password & secret (`NODERED_ADMIN_PASSWORD`, `NODERED_CREDENTIAL_SECRET`)
- `CORS_ORIGIN` (add your Netlify domain, e.g., `https://your-site.netlify.app,http://localhost:5173`)
- `FRONTEND_DASHBOARD_URL` (your Netlify domain)
- `TELEGRAM_BOT_TOKEN` & `TELEGRAM_CHAT_ID` (from Step 5A — or leave blank initially)

Generate hashed Mosquitto passwords + Node-RED credentials and launch all 4 containers:
```bash
bash scripts/init-secrets.sh
docker compose up -d --build
docker compose ps
```

Wait ~20–30 seconds until `lab_mosquitto`, `lab_influxdb`, and `lab_nodered` show `(healthy)` and `lab_cloudflared` shows `Up`.

---

## Step 5 — Configure Telegram & Firebase Cloud Messaging (FCM)

### 5A. Telegram Bot Alerts (Handled Exclusively by Node-RED)
1. Message `@BotFather` on Telegram → `/newbot` → copy the **Bot Token**.
2. Start a chat with your bot (or add it to your lab safety group) and retrieve your **Chat ID** via `https://api.telegram.org/bot<TOKEN>/getUpdates` (or `@userinfobot`).
3. Set `TELEGRAM_BOT_TOKEN` and `TELEGRAM_CHAT_ID` in `backend/.env`, then recreate Node-RED:
   ```bash
   docker compose up -d nodered
   ```

### 5B. Firebase Cloud Messaging (FCM HTTP v1 Browser Push)
1. In [Firebase Console](https://console.firebase.google.com/) → Select/create your project → **Project Settings → Service accounts → Generate new private key**.
2. Upload the downloaded `.json` file from your PC to the Azure VM:
   ```bash
   scp -i ~/Downloads/lab-safety-backend_key.pem ~/Downloads/*firebase-adminsdk*.json \
     azureuser@<YOUR_AZURE_PUBLIC_IP>:~/IDP-Research/backend/nodered/secrets/firebase-service-account.json
   ```
3. Lock permissions and restart Node-RED:
   ```bash
   chmod 600 ~/IDP-Research/backend/nodered/secrets/firebase-service-account.json
   cd ~/IDP-Research/backend && docker compose restart nodered
   docker logs lab_nodered --tail 30
   ```
   Look for: `[FCM] Firebase Admin SDK initialized cleanly (topic: lab-alerts).`

---

## Step 6 — Configure ESP32-S3 Main Node & LoRa Gateway

Open each ESP32's LittleFS web portal (`http://<esp32-ip>/iot`), select **Cloud Broker**, and enter:

| Field | Main Node (`lab_main_s3`) | LoRa Gateway (`lab_gateway_mini`) |
| :--- | :--- | :--- |
| **Connection Mode** | `Cloud Broker` | `Cloud Broker` |
| **Protocol** | `WSS (WebSocket over TLS)` | `WSS (WebSocket over TLS)` |
| **Host** | `lab-mqtt.ikmaliman.my` | `lab-mqtt.ikmaliman.my` |
| **Port** | `443` | `443` |
| **WS Path** | `/mqtt` | `/mqtt` |
| **Username** | `lab_main_s3` | `lab_gateway_mini` |
| **Password** | `<MQTT_ESP32_MAIN_PASS>` | `<MQTT_ESP32_GATEWAY_PASS>` |

Click **Save MQTT Config** — within 5 seconds the connection badge turns green. Watch live broker connections anytime with:
```bash
docker logs -f lab_mosquitto
```

---

## Step 7 — End-to-End Verification Checklist

1. **Container Health & RAM Check:**
   ```bash
   docker compose ps
   docker stats --no-stream
   free -h
   ```
   Confirm all 4 containers are healthy and total container memory sits comfortably around ~350–500 MB.
2. **Public API Health Check:**
   ```bash
   curl -s https://lab-api.ikmaliman.my/api/health
   ```
   Confirm `"status": "ok"` and `"mqttConnected": true`.
3. **Editor Isolation Security Check (MUST return 404):**
   ```bash
   curl -I https://lab-api.ikmaliman.my/admin
   ```
   Expected: `HTTP/2 404`.
4. **Simulate Hazard States (No Hardware Needed):**
   - Open `https://lab-nodered.ikmaliman.my/admin` (sign in via Cloudflare Zero Trust OTP + Node-RED admin login).
   - Switch to Tab **`3. Test & Simulation`** and click **`Simulate State 3 (Emergency - Fire)`**.
   - Verify:
     - Telegram alert arrives immediately.
     - FCM browser push fires (if service account JSON is installed & browser registered).
     - `curl -s https://lab-api.ikmaliman.my/api/latest` returns the State 3 snapshot.
     - `curl -s "https://lab-api.ikmaliman.my/api/alerts?range=1h"` returns the logged fire event from InfluxDB v2.
   - Click **`Simulate State 0 (Normal)`** to confirm the "All Clear / Back to Normal" recovery alert fires.
5. **Schedule Weekly Automated Backups:**
   ```bash
   crontab -e
   # Add this line (runs every Sunday at 03:00 MYT):
   0 3 * * 0 cd /home/azureuser/IDP-Research/backend && bash scripts/backup.sh >> /home/azureuser/IDP-Research/backend/backups/backup.log 2>&1
   ```

---

## Troubleshooting Quick Reference

| Issue | Fix |
| :--- | :--- |
| Changed MQTT passwords in `.env` | Re-run `bash scripts/init-secrets.sh --force && docker compose restart mosquitto nodered`. |
| Node-RED shows `InfluxDB unauthorized` | Run `bash scripts/init-secrets.sh --force && docker compose restart nodered`, or paste `INFLUXDB_ADMIN_TOKEN` into the `InfluxDB v2 (idp-lab)` config node in Node-RED. |
| Want to monitor live MQTT topics visually | Connect **MQTTX** desktop app to `wss://lab-mqtt.ikmaliman.my:443/mqtt` using `nodered_backend` credentials, or run `docker logs -f lab_mosquitto`. |
| Netlify frontend gets CORS error | Add exact Netlify URL (no trailing slash, e.g., `https://myapp.netlify.app`) to `CORS_ORIGIN` in `.env` and run `docker compose up -d nodered`. |
| Emergency local access if Cloudflare is down | SSH tunnel from your PC: `ssh -i key.pem -L 1880:127.0.0.1:1880 -L 8086:127.0.0.1:8086 azureuser@<AZURE_IP>` |
