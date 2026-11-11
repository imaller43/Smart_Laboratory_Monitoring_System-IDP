# Cloud Backend (MQTT, Node-RED, Database, Telegram Bot)

**Owner:** Cloud Backend Engineer

## Suggested Structure
```text
backend/
├── docker-compose.yml   # EMQX/Mosquitto MQTT, Node-RED, InfluxDB/MySQL
├── nodered/             # Exported Node-RED flows (flows.json)
├── mosquitto/           # Broker config (mosquitto.conf)
├── .env.example         # Template for required environment variables
└── .gitignore
```

## Notes
- Refer to [`docs/BluePrint.md`](../docs/BluePrint.md) (Section 6) and [`docs/NodeRed_Flowchart.drawio`](../docs/NodeRed_Flowchart.drawio) for the cloud architecture and alert routing.
- **Never commit** `.env` or `flows_cred.json` (containing Telegram Bot tokens, Chat IDs, or database passwords).
