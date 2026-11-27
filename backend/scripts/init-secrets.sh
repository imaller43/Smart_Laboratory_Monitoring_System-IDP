#!/usr/bin/env bash
# ============================================================================
# init-secrets.sh — Bootstrap gitignored Mosquitto & Node-RED secrets from .env
# Usage:
#   cd backend
#   cp .env.example .env && nano .env
#   bash scripts/init-secrets.sh          # safe: won't overwrite flows_cred.json
#   bash scripts/init-secrets.sh --force  # regenerates flows_cred.json from .env
# ============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
ENV_FILE="${BACKEND_DIR}/.env"
FORCE=false

if [[ "${1:-}" == "--force" ]]; then
  FORCE=true
fi

if [[ ! -f "${ENV_FILE}" ]]; then
  echo "ERROR: ${ENV_FILE} not found."
  echo "Run: cp .env.example .env && nano .env"
  exit 1
fi

if ! command -v docker >/dev/null 2>&1; then
  echo "ERROR: docker is not installed or not in PATH."
  exit 1
fi

# Lock down .env file permissions
chmod 600 "${ENV_FILE}"

# Load .env safely
set -a
# shellcheck disable=SC1090
source "${ENV_FILE}"
set +a

# Basic placeholder check
if grep -q "replace_with_" "${ENV_FILE}"; then
  echo "WARNING: .env still contains 'replace_with_' placeholder values!"
  echo "Please replace all placeholders before deploying to production."
fi

# Create required runtime directories
mkdir -p \
  "${BACKEND_DIR}/data/mosquitto/data" \
  "${BACKEND_DIR}/data/influxdb/data" \
  "${BACKEND_DIR}/data/influxdb/config" \
  "${BACKEND_DIR}/nodered/secrets" \
  "${BACKEND_DIR}/backups"

chmod 700 "${BACKEND_DIR}/nodered/secrets"

# 1. Generate hashed Mosquitto password file (gitignored) using the official image
#    Passes credentials via env vars so special characters in passwords stay intact.
echo "[INFO] Generating Mosquitto PBKDF2 password file via eclipse-mosquitto:2.0..."
docker run --rm \
  --user 0:0 \
  -e "U_NR=${MQTT_NODERED_USER:-nodered_backend}" \
  -e "P_NR=${MQTT_NODERED_PASS:?Set MQTT_NODERED_PASS in .env}" \
  -e "U_MAIN=${MQTT_ESP32_MAIN_USER:-lab_main_s3}" \
  -e "P_MAIN=${MQTT_ESP32_MAIN_PASS:?Set MQTT_ESP32_MAIN_PASS in .env}" \
  -e "U_GW=${MQTT_ESP32_GATEWAY_USER:-lab_gateway_mini}" \
  -e "P_GW=${MQTT_ESP32_GATEWAY_PASS:?Set MQTT_ESP32_GATEWAY_PASS in .env}" \
  -v "${BACKEND_DIR}/mosquitto:/mosquitto/config" \
  -v "${BACKEND_DIR}/data/mosquitto/data:/mosquitto/data" \
  eclipse-mosquitto:2.0 sh -eu -c '
    rm -f /mosquitto/config/passwd
    touch /mosquitto/config/passwd
    mosquitto_passwd -b /mosquitto/config/passwd "$U_NR" "$P_NR"
    mosquitto_passwd -b /mosquitto/config/passwd "$U_MAIN" "$P_MAIN"
    mosquitto_passwd -b /mosquitto/config/passwd "$U_GW" "$P_GW"
    chown 1883:1883 /mosquitto/config/passwd /mosquitto/data
    chmod 0600 /mosquitto/config/passwd
  '
echo "[OK] Generated ${BACKEND_DIR}/mosquitto/passwd (UID 1883, mode 0600)"

# 2. Seed Node-RED initial credentials file (gitignored)
#    Node-RED reads plaintext flows_cred.json on first boot and encrypts it
#    with NODERED_CREDENTIAL_SECRET on the next deploy.
CRED_FILE="${BACKEND_DIR}/nodered/flows_cred.json"
if [[ ! -f "${CRED_FILE}" || "${FORCE}" == "true" ]]; then
  python3 - "${CRED_FILE}" \
    "${MQTT_NODERED_USER:-nodered_backend}" \
    "${MQTT_NODERED_PASS}" \
    "${INFLUXDB_ADMIN_TOKEN:?Set INFLUXDB_ADMIN_TOKEN in .env}" <<'PYEOF'
import json, sys

cred_path, mqtt_user, mqtt_pass, influx_token = sys.argv[1:5]
creds = {
    "cfg_mqtt_broker": {
        "user": mqtt_user,
        "password": mqtt_pass,
    },
    "cfg_influx_v2": {
        "token": influx_token,
    },
}
with open(cred_path, "w", encoding="utf-8") as f:
    json.dump(creds, f, indent=2)
    f.write("\n")
PYEOF
  chmod 600 "${CRED_FILE}"
  echo "[OK] Seeded ${CRED_FILE} (Mosquitto + InfluxDB v2 credentials)"
else
  echo "[SKIP] ${CRED_FILE} already exists (use --force to overwrite)"
fi

echo ""
echo "Initialization complete! Next steps:"
echo "  1. (Optional) Place Firebase service account JSON at: backend/nodered/secrets/firebase-service-account.json"
echo "  2. Start the stack: docker compose up -d --build"
echo "  3. Verify health:   docker compose ps && docker stats --no-stream"
