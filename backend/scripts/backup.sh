#!/usr/bin/env bash
# ============================================================================
# backup.sh — Backup InfluxDB v2 data + Node-RED flows + Mosquitto configs
# Usage:
#   cd backend
#   bash scripts/backup.sh
#
# Suggested weekly cron (Sundays 03:00 MYT):
#   0 3 * * 0 cd ~/IDP-Research/backend && bash scripts/backup.sh >> backups/backup.log 2>&1
# ============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
ENV_FILE="${BACKEND_DIR}/.env"
STAMP="$(date +%Y-%m-%d_%H%M%S)"
BACKUP_ROOT="${BACKEND_DIR}/backups"
WORK_DIR="${BACKUP_ROOT}/tmp_${STAMP}"
ARCHIVE="${BACKUP_ROOT}/lab_backend_backup_${STAMP}.tar.gz"

if [[ ! -f "${ENV_FILE}" ]]; then
  echo "ERROR: ${ENV_FILE} not found."
  exit 1
fi

set -a
# shellcheck disable=SC1090
source "${ENV_FILE}"
set +a

mkdir -p "${WORK_DIR}/influxdb" "${WORK_DIR}/config"

echo "[${STAMP}] Backing up InfluxDB v2 bucket '${INFLUXDB_BUCKET:-lab_safety}'..."
docker exec lab_influxdb rm -rf /tmp/influx_backup
docker exec lab_influxdb influx backup /tmp/influx_backup \
  --org "${INFLUXDB_ORG:-idp-lab}" \
  --bucket "${INFLUXDB_BUCKET:-lab_safety}" \
  --token "${INFLUXDB_ADMIN_TOKEN}"
docker cp lab_influxdb:/tmp/influx_backup/. "${WORK_DIR}/influxdb/"
docker exec lab_influxdb rm -rf /tmp/influx_backup

echo "[${STAMP}] Backing up Node-RED flows & Mosquitto configuration..."
cp -f "${BACKEND_DIR}/nodered/flows.json" "${WORK_DIR}/config/"
cp -f "${BACKEND_DIR}/nodered/settings.js" "${WORK_DIR}/config/"
cp -f "${BACKEND_DIR}/mosquitto/mosquitto.conf" "${WORK_DIR}/config/"
cp -f "${BACKEND_DIR}/mosquitto/acl.conf" "${WORK_DIR}/config/"

tar -czf "${ARCHIVE}" -C "${WORK_DIR}" .
rm -rf "${WORK_DIR}"
chmod 600 "${ARCHIVE}"

# Keep only the most recent 14 days of archives
find "${BACKUP_ROOT}" -maxdepth 1 -name "lab_backend_backup_*.tar.gz" -mtime +14 -delete

echo "[${STAMP}] Backup saved: ${ARCHIVE} ($(du -h "${ARCHIVE}" | cut -f1))"
