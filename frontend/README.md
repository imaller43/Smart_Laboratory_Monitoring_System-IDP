# Frontend Dashboard (React / Vue on Netlify)

**Owner:** Frontend / UI Developer

## Setup & Deployment Notes
- **Netlify Monorepo Config:** Set Netlify **Base directory** to `frontend` and **Production branch** to `main`.
- **API Integration:** Connects to Node-RED HTTP REST API endpoints and WebSockets (refer to [`docs/BluePrint.md`](../docs/BluePrint.md) and [`docs/firmware.md`](../docs/firmware.md)).
- **Never commit** `node_modules/`, `dist/`, or `.env.local` files.
