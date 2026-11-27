# ============================================================================
# Custom Node-RED 4.0 Image (ARM64 & AMD64 compatible)
# Pre-installs InfluxDB v2, Telegram palette, Firebase Admin SDK (FCM HTTP v1),
# and bcryptjs into /usr/src/node-red so mounting ./nodered:/data stays clean.
# ============================================================================
FROM nodered/node-red:4.0

WORKDIR /usr/src/node-red

RUN npm install --no-audit --no-fund \
    node-red-contrib-influxdb@^0.7.0 \
    node-red-contrib-telegrambot@^16.0.0 \
    firebase-admin@^12.7.0 \
    bcryptjs@^2.4.3
