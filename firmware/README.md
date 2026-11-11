# Firmware (`ESP32-S3` Main Node & `ESP32 Super Mini` LoRa Gateway)

**Owners:** Hardware & Embedded Engineer + Control Logic Developer

## Suggested Structure
To prevent file conflicts while working on the same branch (`main`), keep hardware drivers and safety logic cleanly separated:

```text
firmware/
├── main_node/        # ESP32-S3 firmware (FreeRTOS, I2C sensors, LittleFS, LoRa TX, MQTT)
├── gateway_node/     # ESP32 Super Mini LoRa-to-MQTT fallback receiver
└── .gitignore
```

## Notes
- Refer to [`docs/firmware.md`](../docs/firmware.md) and [`docs/hardware.md`](../docs/hardware.md) for FreeRTOS core allocation, pinouts, and state machine specs.
- **Never commit** Wi-Fi passwords or MQTT credentials (`secrets.h` and `data/config.json` are git-ignored). Commit `config.example.json` instead.
