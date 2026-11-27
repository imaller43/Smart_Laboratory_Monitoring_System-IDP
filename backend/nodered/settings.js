/**
 * ============================================================================
 * Node-RED Settings — Smart Laboratory Safety Monitoring System (IDP)
 * ============================================================================
 * Key security & architecture features:
 *  - Flow Editor moved to `/admin` (isolated from public `/api/*` & `/ws/*`)
 *  - Admin auth configured via `.env` (supports pre-hashed bcrypt or plain password)
 *  - Dynamic CORS whitelist via `CORS_ORIGIN` env var (for Netlify + local dev)
 *  - Firebase Admin SDK (FCM HTTP v1) helper exposed cleanly in global context
 */

const fs = require("fs");
const bcrypt = require("bcryptjs");

// ---------------------------------------------------------------------------
// 1. Admin Auth Resolution
// ---------------------------------------------------------------------------
function buildAdminAuth() {
    const username = process.env.NODERED_ADMIN_USER || "admin";
    let passwordHash = (process.env.NODERED_ADMIN_PASSWORD_HASH || "").trim();

    if (!passwordHash && process.env.NODERED_ADMIN_PASSWORD) {
        passwordHash = bcrypt.hashSync(process.env.NODERED_ADMIN_PASSWORD, 8);
    }

    if (!passwordHash) {
        console.warn("[Node-RED] WARNING: Neither NODERED_ADMIN_PASSWORD_HASH nor NODERED_ADMIN_PASSWORD is set. Editor auth disabled!");
        return undefined;
    }

    return {
        type: "credentials",
        users: [
            {
                username,
                password: passwordHash,
                permissions: "*"
            }
        ]
    };
}

// ---------------------------------------------------------------------------
// 2. Dynamic CORS Configuration (Netlify Frontend + Local Dev)
// ---------------------------------------------------------------------------
function buildHttpNodeCors() {
    const raw = (process.env.CORS_ORIGIN || "*").trim();

    if (raw === "*") {
        return {
            origin: "*",
            methods: "GET,PUT,POST,DELETE,OPTIONS",
            allowedHeaders: "Content-Type,Authorization,X-API-Key"
        };
    }

    const allowedOrigins = raw
        .split(",")
        .map((s) => s.trim().replace(/\/+$/, ""))
        .filter(Boolean);

    return {
        origin: function (origin, callback) {
            // Allow non-browser clients (curl, healthchecks, server-to-server)
            if (!origin) {
                return callback(null, true);
            }
            const normalized = origin.replace(/\/+$/, "");
            if (allowedOrigins.includes(normalized)) {
                return callback(null, true);
            }
            console.warn(`[CORS] Blocked request from unauthorized origin: ${origin}`);
            return callback(new Error("Not allowed by CORS"));
        },
        methods: "GET,PUT,POST,DELETE,OPTIONS",
        allowedHeaders: "Content-Type,Authorization,X-API-Key",
        credentials: true
    };
}

// ---------------------------------------------------------------------------
// 3. Firebase Cloud Messaging (FCM HTTP v1) Helper
// ---------------------------------------------------------------------------
function createFcmHelper() {
    const serviceAccountPath =
        process.env.FCM_SERVICE_ACCOUNT_PATH || "/data/secrets/firebase-service-account.json";
    const defaultTopic = process.env.FCM_TOPIC || "lab-alerts";
    const defaultUrl = process.env.FRONTEND_DASHBOARD_URL || "https://lab-api.ikmaliman.my";

    let admin = null;
    let ready = false;
    let initError = null;

    function tryInit() {
        if (ready) return true;
        if (!fs.existsSync(serviceAccountPath)) {
            initError = `Service account file not found at ${serviceAccountPath}`;
            return false;
        }
        try {
            admin = require("firebase-admin");
            if (!admin.apps.length) {
                const serviceAccount = JSON.parse(fs.readFileSync(serviceAccountPath, "utf8"));
                admin.initializeApp({
                    credential: admin.credential.cert(serviceAccount)
                });
            }
            ready = true;
            initError = null;
            console.log(`[FCM] Firebase Admin SDK initialized cleanly (topic: ${defaultTopic}).`);
            return true;
        } catch (err) {
            initError = err.message;
            console.error(`[FCM] Failed to initialize Firebase Admin SDK: ${err.message}`);
            return false;
        }
    }

    // Attempt initialization once at startup (non-fatal if file not yet uploaded)
    if (!tryInit()) {
        console.log(`[FCM] Browser push disabled for now (${initError}). Drop your service account JSON at ${serviceAccountPath} and restart Node-RED when ready.`);
    }

    return {
        isReady: () => ready || tryInit(),
        getStatus: () => ({
            ready: ready || tryInit(),
            topic: defaultTopic,
            reason: ready ? null : initError
        }),
        async subscribeToken(token, topic = defaultTopic) {
            if (!this.isReady()) {
                return { ok: false, skipped: true, error: initError };
            }
            const res = await admin.messaging().subscribeToTopic([token], topic);
            if (res.failureCount > 0) {
                const firstErr = res.errors && res.errors[0] ? res.errors[0].error.message : "Subscription failed";
                return { ok: false, error: firstErr, topic };
            }
            return { ok: true, topic, successCount: res.successCount };
        },
        async unsubscribeToken(token, topic = defaultTopic) {
            if (!this.isReady()) {
                return { ok: false, skipped: true, error: initError };
            }
            const res = await admin.messaging().unsubscribeFromTopic([token], topic);
            if (res.failureCount > 0) {
                const firstErr = res.errors && res.errors[0] ? res.errors[0].error.message : "Unsubscription failed";
                return { ok: false, error: firstErr, topic };
            }
            return { ok: true, topic, successCount: res.successCount };
        },
        async sendAlert({ title, body, data = {}, url = defaultUrl, topic = defaultTopic }) {
            if (!this.isReady()) {
                return { ok: false, skipped: true, reason: initError };
            }
            // FCM data payload requires all values to be strings
            const stringData = {};
            for (const [k, v] of Object.entries(data)) {
                stringData[k] = v === null || v === undefined ? "" : String(v);
            }
            const message = {
                topic,
                notification: {
                    title,
                    body
                },
                data: stringData,
                webpush: {
                    headers: {
                        Urgency: "high",
                        TTL: "300"
                    },
                    notification: {
                        title,
                        body,
                        requireInteraction: Number(data.state) >= 2
                    },
                    fcmOptions: {
                        link: url
                    }
                }
            };
            const messageId = await admin.messaging().send(message);
            return { ok: true, messageId, topic };
        }
    };
}

// ---------------------------------------------------------------------------
// 4. Exported Node-RED Settings
// ---------------------------------------------------------------------------
module.exports = {
    flowFile: "flows.json",
    flowFilePretty: true,

    // Isolate the Flow Editor under `/admin` so public API routes stay strictly separated
    httpAdminRoot: "/admin",
    httpNodeRoot: "/",

    // Security & credentials
    adminAuth: buildAdminAuth(),
    credentialSecret: process.env.NODERED_CREDENTIAL_SECRET || false,

    // CORS for Netlify dashboard REST API calls
    httpNodeCors: buildHttpNodeCors(),

    // Global objects available inside Function nodes via global.get(...)
    functionGlobalContext: {
        fcmHelper: createFcmHelper()
    },

    // Enable both memory (default) and persistent filesystem context if needed later
    contextStorage: {
        default: {
            module: "memory"
        },
        file: {
            module: "localfilesystem"
        }
    },

    editorTheme: {
        page: {
            title: "Lab Safety — Node-RED Backend"
        },
        projects: {
            enabled: false
        }
    },

    logging: {
        console: {
            level: "info",
            metrics: false,
            audit: false
        }
    }
};
