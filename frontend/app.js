/**
 * Sensor Value - Real-Time IoT Dashboard
 * Core Application Controller
 * Supports: WebSocket (Real-Time), REST API (Historical via Node-RED/InfluxDB), & Simulation Fallback
 */

// Backend Middleware Configuration (Node-RED + InfluxDB + MQTT)
const BACKEND_CONFIG = {
  // REST API provided by Node-RED (queries InfluxDB)
  apiBaseUrl: 'http://localhost:1880/api',
  // WebSocket streams provided by Node-RED
  wsSensorsUrl: 'ws://localhost:1880/ws/sensors',
  wsGpioUrl: 'ws://localhost:1880/ws/gpio',
  reconnectInterval: 3000
};

// Global State
const appState = {
  currentRoom: 'design',
  timeRange: '1h',
  theme: 'dark',
  mode: 'simulated', // 'simulated' | 'backend'
  wsSensors: null,
  wsGpio: null,
  wsConnected: false,
  mqttConnected: false,
  dbConnected: false,
  
  // Real-time sensor current readings
  sensors: {
    humidity: 50.00,
    temperature: 20.20,
    light: 275.56
  },

  // GPIO States
  gpio: {
    fan: false,
    interrupt: false,
    light: false
  },

  // Historical buffer for logs
  history: {
    humidity: [
      { time: '24/09/2026 17:56:38', val: 50.00 },
      { time: '24/09/2026 17:56:00', val: 50.20 },
      { time: '24/09/2026 17:55:00', val: 51.00 },
      { time: '24/09/2026 17:54:00', val: 51.20 },
      { time: '24/09/2026 17:53:00', val: 52.00 }
    ],
    temperature: [
      { time: '24/09/2026 17:56:38', val: 20.20 },
      { time: '24/09/2026 17:56:00', val: 20.20 },
      { time: '24/09/2026 17:55:00', val: 20.20 },
      { time: '24/09/2026 17:54:00', val: 20.20 },
      { time: '24/09/2026 17:53:00', val: 20.33 }
    ],
    light: [
      { time: '24/09/2026 17:56:38', val: 342.00 },
      { time: '24/09/2026 17:56:00', val: 323.46 },
      { time: '24/09/2026 17:55:00', val: 343.78 },
      { time: '24/09/2026 17:54:00', val: 338.63 },
      { time: '24/09/2026 17:53:00', val: 343.78 }
    ]
  }
};

// References to Charts
let dualSensorChart = null;
let lightDensityChart = null;
let meanHourlyChart = null;

// Real-time Simulation Interval
let simTicker = null;

/* ==========================================================================
   Initialization
   ========================================================================== */
document.addEventListener('DOMContentLoaded', () => {
  initClock();
  initThemeToggle();
  initRoomTabs();
  initGpioSwitches();
  initTimeRangeDropdown();
  initSettingsModal();
  initGauges();
  renderInitialTables();
  initCharts();
  startSimulationEngine();

  // Try auto-connecting to backend (Node-RED WebSocket & REST API)
  fetchHistoricalData();
  connectSensorsWebSocket();
  connectGpioWebSocket();
});

/* ==========================================================================
   Clock & Date Display
   ========================================================================== */
function initClock() {
  const timeElem = document.getElementById('liveTimestamp');

  function update() {
    const now = new Date();
    const options = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' };
    const datePart = now.toLocaleDateString('en-US', options);
    const timePart = now.toLocaleTimeString('en-US', { hour12: true });
    timeElem.textContent = `${datePart} | ${timePart}`;
  }

  update();
  setInterval(update, 1000);
}

function getFormattedTimestamp() {
  const now = new Date();
  const day = String(now.getDate()).padStart(2, '0');
  const month = String(now.getMonth() + 1).padStart(2, '0');
  const year = now.getFullYear();
  const hours = String(now.getHours()).padStart(2, '0');
  const minutes = String(now.getMinutes()).padStart(2, '0');
  const seconds = String(now.getSeconds()).padStart(2, '0');
  return `${day}/${month}/${year} ${hours}:${minutes}:${seconds}`;
}

function getTimeOnly() {
  const now = new Date();
  const hours = String(now.getHours()).padStart(2, '0');
  const minutes = String(now.getMinutes()).padStart(2, '0');
  const seconds = String(now.getSeconds()).padStart(2, '0');
  return `${hours}:${minutes}:${seconds}`;
}

/* ==========================================================================
   SVG Semi-Circular Gauges
   ========================================================================== */
const gaugeConfigs = {
  humidity: {
    min: 0,
    max: 100,
    unit: '%',
    ticks: [0, 20, 50, 80, 100],
    gradient: [
      { offset: '0%', color: '#f59e0b' },
      { offset: '50%', color: '#38bdf8' },
      { offset: '100%', color: '#2563eb' }
    ]
  },
  temperature: {
    min: 0,
    max: 40,
    unit: '°C',
    ticks: [0, 10, 20, 30, 40],
    gradient: [
      { offset: '0%', color: '#06b6d4' },
      { offset: '50%', color: '#22c55e' },
      { offset: '85%', color: '#eab308' },
      { offset: '100%', color: '#ef4444' }
    ]
  },
  light: {
    min: 0,
    max: 3000,
    unit: 'LPD',
    ticks: [0, 600, 1500, 2400, 3000],
    gradient: [
      { offset: '0%', color: '#334155' },
      { offset: '30%', color: '#d97706' },
      { offset: '100%', color: '#fbbf24' }
    ]
  }
};

function createGaugeSVG(containerId, type, initialValue) {
  const container = document.getElementById(containerId);
  const config = gaugeConfigs[type];
  const radius = 80;
  const strokeWidth = 14;
  const cx = 130;
  const cy = 100;

  // Semi-circle Arc path from 180° to 0°
  const arcPath = `M ${cx - radius} ${cy} A ${radius} ${radius} 0 0 1 ${cx + radius} ${cy}`;

  // Ticks calculation
  let ticksSvg = '';
  config.ticks.forEach(tickVal => {
    const fraction = (tickVal - config.min) / (config.max - config.min);
    const angleRad = Math.PI - (fraction * Math.PI); // 180 to 0
    const innerR = radius - 16;
    const outerR = radius - 9;
    const tx1 = cx + innerR * Math.cos(angleRad);
    const ty1 = cy - innerR * Math.sin(angleRad);
    const tx2 = cx + outerR * Math.cos(angleRad);
    const ty2 = cy - outerR * Math.sin(angleRad);
    const textR = radius - 26;
    const textX = cx + textR * Math.cos(angleRad);
    const textY = cy - textR * Math.sin(angleRad) + 3;

    ticksSvg += `
      <line x1="${tx1}" y1="${ty1}" x2="${tx2}" y2="${ty2}" stroke="rgba(255,255,255,0.25)" stroke-width="1.5" />
      <text x="${textX}" y="${textY}" fill="#8492a6" font-size="8.5" font-family="'JetBrains Mono', monospace" font-weight="500" text-anchor="middle">${tickVal}</text>
    `;
  });

  // Gradient Definition
  let gradStops = config.gradient.map(s => `<stop offset="${s.offset}" stop-color="${s.color}" />`).join('');

  const svg = `
    <svg viewBox="0 0 260 120" xmlns="http://www.w3.org/2000/svg">
      <defs>
        <linearGradient id="grad-${type}" x1="0%" y1="0%" x2="100%" y2="0%">
          ${gradStops}
        </linearGradient>
      </defs>

      <!-- Background Arc Track -->
      <path d="${arcPath}" fill="none" stroke="rgba(255, 255, 255, 0.08)" stroke-width="${strokeWidth}" stroke-linecap="round" />

      <!-- Gradient Arc Track -->
      <path d="${arcPath}" fill="none" stroke="url(#grad-${type})" stroke-width="${strokeWidth - 2}" stroke-linecap="round" />

      <!-- Tick Lines & Labels -->
      ${ticksSvg}

      <!-- Needle Anchor Group positioned at Pivot Point -->
      <g transform="translate(${cx}, ${cy})">
        <g id="needle-${type}" style="transform-origin: 0px 0px; transition: transform 0.6s cubic-bezier(0.34, 1.56, 0.64, 1);">
          <!-- Needle Pointer pointing upward towards 12 o'clock -->
          <polygon points="-3.5,0 0,${-(radius - 12)} 3.5,0" fill="#f8fafc" />
          <!-- Pivot Cap -->
          <circle cx="0" cy="0" r="6" fill="#1e293b" stroke="#cbd5e1" stroke-width="2" />
          <circle cx="0" cy="0" r="2.5" fill="#38bdf8" />
        </g>
      </g>
    </svg>
  `;

  container.innerHTML = svg;
  updateGaugeNeedle(type, initialValue);
}

function updateGaugeNeedle(type, value) {
  const config = gaugeConfigs[type];
  const clamped = Math.max(config.min, Math.min(config.max, value));
  const fraction = (clamped - config.min) / (config.max - config.min);
  // Angle: 0 fraction = -90 deg (pointing left), 1 fraction = 90 deg (pointing right)
  const angleDeg = -90 + (fraction * 180);

  const needle = document.getElementById(`needle-${type}`);
  if (needle) {
    needle.style.transform = `rotate(${angleDeg}deg)`;
  }
}

function initGauges() {
  createGaugeSVG('humidityGaugeContainer', 'humidity', appState.sensors.humidity);
  createGaugeSVG('temperatureGaugeContainer', 'temperature', appState.sensors.temperature);
  createGaugeSVG('lightGaugeContainer', 'light', appState.sensors.light);
}

/* ==========================================================================
   Data Tables Rendering
   ========================================================================== */
function renderInitialTables() {
  renderTableBody('humidityTableBody', appState.history.humidity, '%');
  renderTableBody('temperatureTableBody', appState.history.temperature, '');
  renderTableBody('lightTableBody', appState.history.light, '');
}

function renderTableBody(elementId, items) {
  const tbody = document.getElementById(elementId);
  tbody.innerHTML = '';
  items.forEach(item => {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td>${item.time}</td>
      <td class="col-val">${Number(item.val).toFixed(2)}</td>
    `;
    tbody.appendChild(tr);
  });
}

function addTableRow(type, time, val) {
  const historyList = appState.history[type];
  historyList.unshift({ time, val });
  if (historyList.length > 8) historyList.pop();

  const tbodyId = `${type}TableBody`;
  renderTableBody(tbodyId, historyList);
}

/* ==========================================================================
   Chart.js Configurations
   ========================================================================== */
function initCharts() {
  const isDark = appState.theme === 'dark';
  const gridColor = isDark ? 'rgba(255, 255, 255, 0.05)' : 'rgba(0, 0, 0, 0.05)';
  const textColor = isDark ? '#8492a6' : '#64748b';

  // 1. Dual Sensor Chart (Humidity & Temperature)
  const ctxDual = document.getElementById('dualSensorChart').getContext('2d');
  const dualLabels = ['16:57:00', '17:02:00', '17:07:00', '17:12:00', '17:17:00', '17:22:00', '17:27:00', '17:32:00', '17:37:00', '17:42:00', '17:47:00', '17:52:00', '17:56:38'];
  const humidityPoints = [51, 62, 59, 56, 54, 53, 62, 58, 55, 54, 63, 58, 50];
  const tempPoints = [19.8, 20.2, 20.3, 20.1, 20.0, 20.1, 20.2, 20.1, 20.0, 20.1, 20.3, 20.2, 20.2];

  // Soft gradients
  const humGrad = ctxDual.createLinearGradient(0, 0, 0, 200);
  humGrad.addColorStop(0, 'rgba(56, 189, 248, 0.25)');
  humGrad.addColorStop(1, 'rgba(56, 189, 248, 0.0)');

  dualSensorChart = new Chart(ctxDual, {
    type: 'line',
    data: {
      labels: dualLabels,
      datasets: [
        {
          label: 'Humidity (%)',
          data: humidityPoints,
          borderColor: '#38bdf8',
          backgroundColor: humGrad,
          fill: true,
          tension: 0.35,
          borderWidth: 2,
          pointRadius: 0,
          pointHoverRadius: 4,
          yAxisID: 'yHumidity'
        },
        {
          label: 'Temperature (°C)',
          data: tempPoints,
          borderColor: '#22c55e',
          backgroundColor: 'transparent',
          fill: false,
          tension: 0.3,
          borderWidth: 2,
          pointRadius: 0,
          pointHoverRadius: 4,
          yAxisID: 'yTemp'
        }
      ]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      interaction: { mode: 'index', intersect: false },
      plugins: {
        legend: {
          position: 'bottom',
          labels: {
            boxWidth: 8,
            boxHeight: 8,
            usePointStyle: true,
            color: textColor,
            font: { family: "'Inter', sans-serif", size: 11 }
          }
        },
        tooltip: {
          backgroundColor: '#161c28',
          titleColor: '#f1f5f9',
          bodyColor: '#cbd5e1',
          borderColor: '#1e2638',
          borderWidth: 1
        }
      },
      scales: {
        x: {
          grid: { color: gridColor },
          ticks: { color: textColor, font: { family: "'JetBrains Mono', monospace", size: 9 }, maxRotation: 0 }
        },
        yHumidity: {
          type: 'linear',
          position: 'left',
          min: 0,
          max: 80,
          grid: { color: gridColor },
          ticks: { color: textColor, font: { size: 10 } }
        },
        yTemp: {
          type: 'linear',
          position: 'right',
          min: 10,
          max: 40,
          grid: { drawOnChartArea: false },
          ticks: { color: textColor, font: { size: 10 } }
        }
      }
    }
  });

  // 2. Light Density Chart
  const ctxLight = document.getElementById('lightDensityChart').getContext('2d');
  const lightLabels = ['17:00:00', '17:05:00', '17:10:00', '17:15:00', '17:20:00', '17:25:00', '17:30:00', '17:35:00', '17:40:00', '17:45:00', '17:50:00', '17:56:38'];
  const lightPoints = [290, 310, 315, 330, 345, 350, 340, 348, 335, 352, 340, 325];

  const lightGrad = ctxLight.createLinearGradient(0, 0, 0, 200);
  lightGrad.addColorStop(0, 'rgba(251, 191, 36, 0.35)');
  lightGrad.addColorStop(1, 'rgba(251, 191, 36, 0.02)');

  lightDensityChart = new Chart(ctxLight, {
    type: 'line',
    data: {
      labels: lightLabels,
      datasets: [{
        label: 'Light (LPD)',
        data: lightPoints,
        borderColor: '#fbbf24',
        backgroundColor: lightGrad,
        fill: true,
        tension: 0.35,
        borderWidth: 2,
        pointRadius: 0,
        pointHoverRadius: 4
      }]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      interaction: { mode: 'index', intersect: false },
      plugins: {
        legend: {
          position: 'bottom',
          labels: {
            boxWidth: 8,
            boxHeight: 8,
            usePointStyle: true,
            color: textColor,
            font: { family: "'Inter', sans-serif", size: 11 }
          }
        },
        tooltip: {
          backgroundColor: '#161c28',
          titleColor: '#f1f5f9',
          bodyColor: '#cbd5e1',
          borderColor: '#1e2638',
          borderWidth: 1
        }
      },
      scales: {
        x: {
          grid: { color: gridColor },
          ticks: { color: textColor, font: { family: "'JetBrains Mono', monospace", size: 9 }, maxRotation: 0 }
        },
        y: {
          min: 0,
          max: 400,
          grid: { color: gridColor },
          ticks: { color: textColor, font: { size: 10 } }
        }
      }
    }
  });

  // 3. Mean Hourly Data Chart (Diurnal Curves)
  const ctxMean = document.getElementById('meanHourlyChart').getContext('2d');
  const meanLabels = [
    '20:00:00', '23:00:00', '02:00:00', '05:00:00', '08:00:00', '11:00:00', '14:00:00', '17:00:00',
    '20:00:00', '23:00:00', '02:00:00', '05:00:00', '08:00:00', '11:00:00', '14:00:00', '17:00:00',
    '20:00:00', '23:00:00', '02:00:00', '05:00:00', '08:00:00', '11:00:00', '14:00:00', '17:56:38'
  ];

  const meanLightCycle = [
    600, 2950, 2950, 2950, 300, 100, 150, 400,
    2950, 2950, 2950, 2950, 250, 120, 180, 450,
    2950, 2950, 2950, 2950, 300, 100, 160, 340
  ];

  const meanLightGrad = ctxMean.createLinearGradient(0, 0, 0, 240);
  meanLightGrad.addColorStop(0, 'rgba(251, 191, 36, 0.28)');
  meanLightGrad.addColorStop(1, 'rgba(251, 191, 36, 0.01)');

  meanHourlyChart = new Chart(ctxMean, {
    type: 'line',
    data: {
      labels: meanLabels,
      datasets: [
        {
          label: 'Mean Humidity (%)',
          data: [],
          borderColor: '#38bdf8',
          backgroundColor: 'transparent',
          borderWidth: 1.5,
          pointRadius: 0
        },
        {
          label: 'Mean Temp (°C)',
          data: [],
          borderColor: '#22c55e',
          backgroundColor: 'transparent',
          borderWidth: 1.5,
          pointRadius: 0
        },
        {
          label: 'Mean Light (LPD)',
          data: meanLightCycle,
          borderColor: '#d97706',
          backgroundColor: meanLightGrad,
          fill: true,
          tension: 0.35,
          borderWidth: 2,
          pointRadius: 0,
          pointHoverRadius: 4
        }
      ]
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      interaction: { mode: 'index', intersect: false },
      plugins: {
        legend: {
          position: 'bottom',
          labels: {
            boxWidth: 8,
            boxHeight: 8,
            usePointStyle: true,
            color: textColor,
            font: { family: "'Inter', sans-serif", size: 11 }
          }
        },
        tooltip: {
          backgroundColor: '#161c28',
          titleColor: '#f1f5f9',
          bodyColor: '#cbd5e1',
          borderColor: '#1e2638',
          borderWidth: 1
        }
      },
      scales: {
        x: {
          grid: { color: gridColor },
          ticks: { color: textColor, font: { family: "'JetBrains Mono', monospace", size: 9 }, maxRotation: 0 }
        },
        y: {
          min: 0,
          max: 3200,
          ticks: {
            stepSize: 800,
            color: textColor,
            font: { size: 10 }
          },
          grid: {
            color: gridColor,
            borderDash: [3, 3]
          }
        }
      }
    }
  });
}

/* ==========================================================================
   Backend Integration: WebSocket (Real-Time) & REST API (Historical)
   ========================================================================== */

/**
 * 1. Fetch Historical Time-Series from Node-RED / InfluxDB via REST API
 */
async function fetchHistoricalData(room = appState.currentRoom, range = appState.timeRange) {
  try {
    const url = `${BACKEND_CONFIG.apiBaseUrl}/history?room=${encodeURIComponent(room)}&range=${encodeURIComponent(range)}`;
    const response = await fetch(url, { headers: { 'Accept': 'application/json' } });
    
    if (!response.ok) {
      throw new Error(`HTTP ${response.status}: ${response.statusText}`);
    }

    const data = await response.json();
    setDbStatus(true);
    showToast(`Loaded ${range} historical data from InfluxDB`);

    // 1. Update Dual Sensor Chart (Humidity & Temperature)
    if (data.timestamps && data.humidity && data.temperature && dualSensorChart) {
      dualSensorChart.data.labels = data.timestamps;
      dualSensorChart.data.datasets[0].data = data.humidity;
      dualSensorChart.data.datasets[1].data = data.temperature;
      dualSensorChart.update();
    }

    // 2. Update Light Density Chart
    if (data.timestamps && data.light && lightDensityChart) {
      lightDensityChart.data.labels = data.timestamps;
      lightDensityChart.data.datasets[0].data = data.light;
      lightDensityChart.update();
    }

    // 3. Update Mean Hourly Data Chart (Diurnal aggregations)
    if (data.meanHourly && meanHourlyChart) {
      if (data.meanHourly.labels) meanHourlyChart.data.labels = data.meanHourly.labels;
      if (data.meanHourly.light) meanHourlyChart.data.datasets[2].data = data.meanHourly.light;
      meanHourlyChart.update();
    }

    // 4. Update Logs table if historical records are supplied
    if (data.logs) {
      if (data.logs.humidity) {
        appState.history.humidity = data.logs.humidity;
        renderTableBody('humidityTableBody', appState.history.humidity);
      }
      if (data.logs.temperature) {
        appState.history.temperature = data.logs.temperature;
        renderTableBody('temperatureTableBody', appState.history.temperature);
      }
      if (data.logs.light) {
        appState.history.light = data.logs.light;
        renderTableBody('lightTableBody', appState.history.light);
      }
    }
  } catch (err) {
    console.warn('[REST API] Backend not reachable or error:', err.message);
    setDbStatus(false);
  }
}

/**
 * 2. Connect to Node-RED WebSocket Stream for Real-Time Telemetry
 */
function connectSensorsWebSocket() {
  if (appState.wsSensors) {
    try { appState.wsSensors.close(); } catch(e) {}
  }

  showToast(`Connecting to Node-RED WebSocket: ${BACKEND_CONFIG.wsSensorsUrl}`);
  
  try {
    const ws = new WebSocket(BACKEND_CONFIG.wsSensorsUrl);
    appState.wsSensors = ws;

    ws.onopen = () => {
      appState.wsConnected = true;
      appState.mode = 'backend';
      document.getElementById('modeLabel').textContent = 'LIVE WS';
      setMqttStatus(true); // Node-RED connected to MQTT
      showToast('Connected to Node-RED Real-Time WebSocket stream');
      // Stop local simulation ticker
      if (simTicker) clearInterval(simTicker);
    };

    ws.onmessage = (event) => {
      try {
        const payload = JSON.parse(event.data);
        // Only update if matches current room (or no room filter specified)
        if (!payload.room || payload.room === appState.currentRoom) {
          updateAllSensors(payload);
        }
      } catch (err) {
        console.warn('Error parsing incoming WebSocket JSON:', err);
      }
    };

    ws.onerror = (err) => {
      console.warn('WebSocket error:', err);
    };

    ws.onclose = () => {
      appState.wsConnected = false;
      setMqttStatus(false);
      console.log(`WebSocket closed. Reconnecting in ${BACKEND_CONFIG.reconnectInterval}ms...`);
      // Fallback to simulation if backend is down
      if (appState.mode === 'backend') {
        appState.mode = 'simulated';
        document.getElementById('modeLabel').textContent = 'SIMULATED';
        startSimulationEngine();
      }
      setTimeout(connectSensorsWebSocket, BACKEND_CONFIG.reconnectInterval);
    };
  } catch (e) {
    console.error('Failed to instantiate WebSocket:', e);
  }
}

/**
 * 3. Connect to GPIO WebSocket for bidirectional remote control
 */
function connectGpioWebSocket() {
  if (appState.wsGpio) {
    try { appState.wsGpio.close(); } catch(e) {}
  }

  try {
    const ws = new WebSocket(BACKEND_CONFIG.wsGpioUrl);
    appState.wsGpio = ws;

    ws.onmessage = (event) => {
      try {
        const cmd = JSON.parse(event.data);
        // Update local switch UI if hardware sends state feedback
        if (cmd.pin) {
          const pinKey = cmd.pin.toLowerCase();
          const switchElem = document.getElementById(`switch${cmd.pin.charAt(0).toUpperCase() + cmd.pin.slice(1).toLowerCase()}`);
          if (switchElem) {
            switchElem.checked = (cmd.state === 1 || cmd.state === true);
            const statusText = switchElem.nextElementSibling.querySelector('.toggle-status-text');
            if (statusText) statusText.textContent = switchElem.checked ? 'ON' : 'OFF';
            appState.gpio[pinKey] = switchElem.checked;
          }
        }
      } catch (e) {
        console.warn('Failed to parse GPIO WebSocket message:', e);
      }
    };

    ws.onclose = () => {
      setTimeout(connectGpioWebSocket, BACKEND_CONFIG.reconnectInterval);
    };
  } catch (e) {
    console.warn('GPIO WebSocket not available:', e);
  }
}

/**
 * 4. Send GPIO Command (via WebSocket, falling back to REST POST)
 */
function dispatchGpioCommand(pinName, state) {
  const payload = {
    command: 'GPIO_SET',
    pin: pinName,
    state: state ? 1 : 0,
    room: appState.currentRoom,
    timestamp: new Date().toISOString()
  };

  // Try GPIO WebSocket first
  if (appState.wsGpio && appState.wsGpio.readyState === WebSocket.OPEN) {
    appState.wsGpio.send(JSON.stringify(payload));
    return;
  }

  // Fallback to REST API POST /api/gpio
  fetch(`${BACKEND_CONFIG.apiBaseUrl}/gpio`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(payload)
  }).catch(() => {
    // If backend offline, action is simulated locally
  });
}

function setDbStatus(connected) {
  appState.dbConnected = connected;
  const badge = document.getElementById('dbStatusBadge');
  if (!badge) return;
  const text = badge.querySelector('.status-text');
  if (connected) {
    badge.classList.remove('disconnected');
    text.textContent = 'DB Connected';
  } else {
    badge.classList.add('disconnected');
    text.textContent = 'DB Offline';
  }
}

/* ==========================================================================
   Real-Time IoT Simulation Engine
   ========================================================================== */
function startSimulationEngine() {
  if (simTicker) clearInterval(simTicker);

  simTicker = setInterval(() => {
    if (!appState.isSimulated) return;

    // Realistic random walk around baseline
    const dHumidity = (Math.random() - 0.5) * 0.4;
    const dTemp = (Math.random() - 0.5) * 0.1;
    const dLight = (Math.random() - 0.5) * 8.0;

    let newHum = +(appState.sensors.humidity + dHumidity).toFixed(2);
    let newTemp = +(appState.sensors.temperature + dTemp).toFixed(2);
    let newLight = +(appState.sensors.light + dLight).toFixed(2);

    // Clamp within bounds
    newHum = Math.max(30, Math.min(85, newHum));
    newTemp = Math.max(15, Math.min(35, newTemp));
    newLight = Math.max(50, Math.min(1200, newLight));

    updateAllSensors({
      humidity: newHum,
      temperature: newTemp,
      light: newLight
    });
  }, 2500);
}

function updateAllSensors(data) {
  const timestamp = getFormattedTimestamp();
  const timeOnly = getTimeOnly();

  // Update State
  if (data.humidity !== undefined) {
    appState.sensors.humidity = Number(data.humidity);
    document.getElementById('humidityValue').textContent = `${appState.sensors.humidity.toFixed(2)} %`;
    updateGaugeNeedle('humidity', appState.sensors.humidity);
    addTableRow('humidity', timestamp, appState.sensors.humidity);
  }

  if (data.temperature !== undefined) {
    appState.sensors.temperature = Number(data.temperature);
    document.getElementById('temperatureValue').textContent = `${appState.sensors.temperature.toFixed(2)} °C`;
    updateGaugeNeedle('temperature', appState.sensors.temperature);
    addTableRow('temperature', timestamp, appState.sensors.temperature);
  }

  if (data.light !== undefined) {
    appState.sensors.light = Number(data.light);
    document.getElementById('lightValue').textContent = `${appState.sensors.light.toFixed(2)} LPD`;
    updateGaugeNeedle('light', appState.sensors.light);
    addTableRow('light', timestamp, appState.sensors.light);
  }

  // Append to Charts
  if (dualSensorChart) {
    const labels = dualSensorChart.data.labels;
    labels.push(timeOnly);
    dualSensorChart.data.datasets[0].data.push(appState.sensors.humidity);
    dualSensorChart.data.datasets[1].data.push(appState.sensors.temperature);

    if (labels.length > 15) {
      labels.shift();
      dualSensorChart.data.datasets[0].data.shift();
      dualSensorChart.data.datasets[1].data.shift();
    }
    dualSensorChart.update('none'); // Smoother update without full animation
  }

  if (lightDensityChart) {
    const labels = lightDensityChart.data.labels;
    labels.push(timeOnly);
    lightDensityChart.data.datasets[0].data.push(appState.sensors.light);

    if (labels.length > 15) {
      labels.shift();
      lightDensityChart.data.datasets[0].data.shift();
    }
    lightDensityChart.update('none');
  }
}

/* ==========================================================================
   GPIO Controls & Event Handlers
   ========================================================================== */
function initGpioSwitches() {
  const switches = [
    { id: 'switchFan', key: 'fan', name: 'FAN' },
    { id: 'switchInterrupt', key: 'interrupt', name: 'INTERRUPT' },
    { id: 'switchLight', key: 'light', name: 'LIGHT' }
  ];

  switches.forEach(sw => {
    const el = document.getElementById(sw.id);
    const statusText = el.nextElementSibling.querySelector('.toggle-status-text');

    el.addEventListener('change', (e) => {
      const isChecked = e.target.checked;
      appState.gpio[sw.key] = isChecked;
      statusText.textContent = isChecked ? 'ON' : 'OFF';

      showToast(`GPIO Command: ${sw.name} turned ${isChecked ? 'ON' : 'OFF'}`);

      // Dispatch via Node-RED WebSocket or REST API
      dispatchGpioCommand(sw.name, isChecked);

      // Publish over MQTT if connected
      if (appState.mqttClient && appState.mqttConnected) {
        const topic = document.getElementById('gpioTopic').value || 'iot/room/gpio';
        const payload = JSON.stringify({
          command: 'GPIO_SET',
          pin: sw.name,
          state: isChecked ? 1 : 0,
          room: appState.currentRoom,
          timestamp: new Date().toISOString()
        });
        const message = new Paho.MQTT.Message(payload);
        message.destinationName = topic;
        appState.mqttClient.send(message);
      }
    });
  });
}

/* ==========================================================================
   Room Tabs Navigation
   ========================================================================== */
function initRoomTabs() {
  const tabs = document.querySelectorAll('.room-tab');
  tabs.forEach(tab => {
    tab.addEventListener('click', () => {
      tabs.forEach(t => t.classList.remove('active'));
      tab.classList.add('active');
      const room = tab.dataset.room;
      appState.currentRoom = room;

      showToast(`Switched view to ${tab.textContent}`);

      // Request historical data from backend for this room
      fetchHistoricalData(room, appState.timeRange);

      // Adjust sensor profile slightly by room in simulation mode
      if (appState.isSimulated) {
        if (room === 'room2') {
          appState.sensors.humidity = 45.3;
          appState.sensors.temperature = 23.5;
          appState.sensors.light = 410.0;
        } else if (room === 'room3') {
          appState.sensors.humidity = 58.1;
          appState.sensors.temperature = 18.9;
          appState.sensors.light = 150.0;
        } else {
          appState.sensors.humidity = 50.0;
          appState.sensors.temperature = 20.2;
          appState.sensors.light = 275.56;
        }
        updateAllSensors(appState.sensors);
      }
    });
  });
}

/* ==========================================================================
   Time Range Dropdown
   ========================================================================== */
function initTimeRangeDropdown() {
  const btn = document.getElementById('timeRangeBtn');
  const menu = document.getElementById('timeRangeMenu');
  const label = document.getElementById('selectedRangeText');

  btn.addEventListener('click', (e) => {
    e.stopPropagation();
    menu.classList.toggle('show');
  });

  document.addEventListener('click', () => {
    menu.classList.remove('show');
  });

  menu.querySelectorAll('.dropdown-item').forEach(item => {
    item.addEventListener('click', (e) => {
      menu.querySelectorAll('.dropdown-item').forEach(i => i.classList.remove('active'));
      item.classList.add('active');
      label.textContent = item.textContent;
      appState.timeRange = item.dataset.range;
      menu.classList.remove('show');
      showToast(`Filter range set to ${item.textContent}`);

      // Query InfluxDB for selected time range
      fetchHistoricalData(appState.currentRoom, appState.timeRange);
    });
  });
}

/* ==========================================================================
   Theme Switcher (Dark / Light)
   ========================================================================== */
function initThemeToggle() {
  const btn = document.getElementById('themeToggleBtn');
  const icon = document.getElementById('themeIcon');

  btn.addEventListener('click', () => {
    const isDark = document.body.classList.contains('dark-theme');
    if (isDark) {
      document.body.classList.remove('dark-theme');
      document.body.classList.add('light-theme');
      icon.className = 'fa-regular fa-moon';
      appState.theme = 'light';
    } else {
      document.body.classList.remove('light-theme');
      document.body.classList.add('dark-theme');
      icon.className = 'fa-regular fa-sun';
      appState.theme = 'dark';
    }
    updateChartTheme();
  });
}

function updateChartTheme() {
  const isDark = appState.theme === 'dark';
  const gridColor = isDark ? 'rgba(255, 255, 255, 0.05)' : 'rgba(0, 0, 0, 0.05)';
  const textColor = isDark ? '#8492a6' : '#64748b';

  [dualSensorChart, lightDensityChart, meanHourlyChart].forEach(chart => {
    if (!chart) return;
    if (chart.options.scales.x) {
      chart.options.scales.x.grid.color = gridColor;
      chart.options.scales.x.ticks.color = textColor;
    }
    if (chart.options.scales.y) {
      chart.options.scales.y.grid.color = gridColor;
      chart.options.scales.y.ticks.color = textColor;
    }
    if (chart.options.scales.yHumidity) {
      chart.options.scales.yHumidity.grid.color = gridColor;
      chart.options.scales.yHumidity.ticks.color = textColor;
    }
    if (chart.options.plugins.legend) {
      chart.options.plugins.legend.labels.color = textColor;
    }
    chart.update();
  });
}

/* ==========================================================================
   Settings Modal & Live MQTT Client Connection
   ========================================================================== */
function initSettingsModal() {
  const modal = document.getElementById('settingsModal');
  const openBtn = document.getElementById('settingsBtn');
  const closeBtn = document.getElementById('closeSettingsBtn');
  const modeBtn = document.getElementById('modeToggleBtn');
  const modeLabel = document.getElementById('modeLabel');
  const connectBtn = document.getElementById('connectMqttBtn');
  const saveSimBtn = document.getElementById('saveSimBtn');

  openBtn.addEventListener('click', () => modal.classList.add('show'));
  closeBtn.addEventListener('click', () => modal.classList.remove('show'));

  modal.addEventListener('click', (e) => {
    if (e.target === modal) modal.classList.remove('show');
  });

  modeBtn.addEventListener('click', () => {
    if (appState.isSimulated) {
      modal.classList.add('show');
    } else {
      appState.isSimulated = true;
      modeLabel.textContent = 'SIMULATED';
      showToast('Switched to Internal Simulated Telemetry');
      setMqttStatus(false);
      startSimulationEngine();
    }
  });

  saveSimBtn.addEventListener('click', () => {
    appState.isSimulated = true;
    modeLabel.textContent = 'SIMULATED';
    modal.classList.remove('show');
    showToast('Simulation engine active');
    startSimulationEngine();
  });

  connectBtn.addEventListener('click', () => {
    const host = document.getElementById('brokerHost').value.trim();
    const port = parseInt(document.getElementById('brokerPort').value.trim(), 10);
    const path = document.getElementById('brokerPath').value.trim();
    const useSsl = document.getElementById('useSsl').checked;
    const topic = document.getElementById('brokerTopic').value.trim();

    connectToMqttBroker(host, port, path, useSsl, topic);
    modal.classList.remove('show');
  });
}

function connectToMqttBroker(host, port, path, useSsl, topic) {
  if (typeof Paho === 'undefined') {
    showToast('Paho MQTT library not loaded. Check internet connection.');
    return;
  }

  showToast(`Connecting to MQTT broker: ${host}:${port}...`);
  const clientId = 'web_dashboard_' + Math.random().toString(16).substr(2, 8);

  try {
    const client = new Paho.MQTT.Client(host, port, path, clientId);
    appState.mqttClient = client;

    client.onConnectionLost = (responseObject) => {
      setMqttStatus(false);
      if (responseObject.errorCode !== 0) {
        showToast(`MQTT Connection Lost: ${responseObject.errorMessage}`);
      }
      // Revert to simulation
      appState.isSimulated = true;
      document.getElementById('modeLabel').textContent = 'SIMULATED';
      startSimulationEngine();
    };

    client.onMessageArrived = (message) => {
      try {
        const payload = JSON.parse(message.payloadString);
        // Expecting JSON format: { humidity: 52.1, temperature: 21.0, light: 310 }
        updateAllSensors(payload);
      } catch (err) {
        console.warn('Failed to parse incoming MQTT payload:', message.payloadString);
      }
    };

    client.connect({
      useSSL: useSsl,
      timeout: 5,
      onSuccess: () => {
        setMqttStatus(true);
        appState.isSimulated = false;
        document.getElementById('modeLabel').textContent = 'LIVE BROKER';
        showToast('Connected to MQTT Broker successfully!');

        // Subscribe to sensor topics
        client.subscribe(topic + '/#', { qos: 0 });
      },
      onFailure: (err) => {
        setMqttStatus(false);
        showToast(`MQTT Connection Failed: ${err.errorMessage || 'Timeout'}`);
        appState.isSimulated = true;
        document.getElementById('modeLabel').textContent = 'SIMULATED';
      }
    });
  } catch (ex) {
    showToast(`Error initializing MQTT: ${ex.message}`);
    setMqttStatus(false);
  }
}

function setMqttStatus(connected) {
  appState.mqttConnected = connected;
  const badge = document.getElementById('mqttStatusBadge');
  const text = badge.querySelector('.status-text');

  if (connected) {
    badge.classList.remove('disconnected');
    text.textContent = 'MQTT Connected';
  } else {
    badge.classList.add('disconnected');
    text.textContent = 'MQTT Disconnected';
  }
}

/* ==========================================================================
   Toast Notifications System
   ========================================================================== */
function showToast(message) {
  const container = document.getElementById('toastContainer');
  const toast = document.createElement('div');
  toast.className = 'toast';
  toast.innerHTML = `
    <i class="fa-solid fa-circle-info" style="color: #38bdf8;"></i>
    <span>${message}</span>
  `;
  container.appendChild(toast);

  setTimeout(() => {
    toast.style.opacity = '0';
    toast.style.transform = 'translateX(100%)';
    toast.style.transition = 'all 0.3s ease-out';
    setTimeout(() => toast.remove(), 300);
  }, 3200);
}
