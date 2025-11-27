#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <esp_http_server.h>
#include <ArduinoJson.h>

const char* WIFI_SSID = "QuangVinhPC";
const char* WIFI_PASS = "12345678";

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define LED_FLASH_PIN 4

#define RXD2_PIN 15
#define TXD2_PIN 14

httpd_handle_t http_server = NULL;
WebSocketsServer webSocket = WebSocketsServer(81);
StaticJsonDocument<512> doc;
String lastJsonPayload = "";

static const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8"/>
    <meta name="viewport" content="width=device-width, initial-scale=1"/>
    <title>Sorting Machine Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        :root {
            --bg: #f5f5f5;
            --card: #ffffff;
            --text-dark: #212121;
            --text-muted: #757575;
            --main-accent: #FF7043;
            --stat-border: #FFAB91;
            
            --stop-btn: #E53935;
            --reset-btn: #1E88E5;
            --running: #4CAF50;
            --stopped: #E53935;
            --warning: #FFB300;
            
            --type-td: #EF5350;
            --type-vd: #C62828;
            --type-tv: #FFCA28;
            --type-vv: #FF8F00;
            --type-tx: #42A5F5;
            --type-vx: #1565C0;
            --type-other: #9E9E9E;
        }
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Roboto', 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: var(--bg);
            color: var(--text-dark);
            min-height: 100vh;
            padding: 10px;
        }
        
        .moon, .stars, .star { display: none; }
        
        .main-grid-container {
            display: grid;
            grid-template-columns: 4fr 6fr; 
            gap: 15px;
            max-width: 1400px;
            margin: 0 auto;
        }

        .camera-control-wrapper {
            grid-column: 1 / 2;
            display: flex;
            flex-direction: column;
            gap: 15px;
        }

        .dashboard-column {
            grid-column: 2 / 3;
            display: flex;
            flex-direction: column;
            gap: 15px;
        }

        .card {
            background: var(--card);
            border-radius: 8px;
            padding: 15px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            border: 1px solid #e0e0e0;
            width: 100%;
        }
        
        .section-title {
            font-size: 16px;
            font-weight: 700;
            margin-bottom: 12px;
            color: var(--main-accent);
            text-transform: uppercase;
            letter-spacing: 0.8px;
            border-bottom: 2px solid var(--main-accent);
            padding-bottom: 5px;
        }

        #view {
            width: 100%;
            border-radius: 6px;
            background: #000;
            box-shadow: 0 3px 8px rgba(0, 0, 0, 0.2);
        }
        
        .top-stats-row {
            display: grid;
            grid-template-columns: 1.5fr 1fr;
            gap: 15px;
        }
        
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
            margin-bottom: 15px;
        }

        .stat-item {
            padding: 12px;
            border-radius: 6px;
            text-align: center;
            border: 1px solid #e0e0e0;
            background: #fafafa;
            transition: all 0.2s;
        }
        
        .stat-value {
            font-size: 24px;
            font-weight: 700;
            margin-bottom: 4px;
            color: var(--text-dark);
        }
        
        .stat-label {
            font-size: 10px;
            color: var(--text-muted);
            text-transform: uppercase;
            letter-spacing: 0.5px;
        }
        
        .realtime-status-combined {
            display: grid;
            grid-template-columns: 2fr 1fr;
            gap: 15px;
        }

        .real-time-detail {
            margin-top: 5px;
            font-size: 14px;
            color: var(--text-dark);
            line-height: 1.6;
            padding-left: 10px;
            border-left: 3px solid var(--stat-border);
            background-color: #fcfcfc;
            padding: 10px;
            border-radius: 4px;
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 8px 20px;
        }
        .rt-label { font-weight: 500; color: var(--main-accent); }
        .rt-value { font-weight: 400; }
        
        .counters-summary-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 8px;
            margin-top: 10px;
        }
        .counter-item {
            background: var(--bg);
            padding: 8px;
            border-radius: 4px;
            box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
            border-left: 3px solid var(--stat-border);
        }
        .counter-label {
            font-size: 10px;
            color: var(--text-muted);
            text-transform: uppercase;
        }
        .counter-value {
            font-size: 20px;
            font-weight: 700;
            line-height: 1.2;
        }

        .total-sorted {
            text-align: center;
            padding: 20px 0;
            border: 1px dashed #e0e0e0;
            border-radius: 6px;
            margin-bottom: 15px;
        }
        .total-value {
            font-size: 48px;
            font-weight: 900;
            color: var(--main-accent);
        }
        .total-label {
            font-size: 14px;
            color: var(--text-muted);
        }

        .chart-container {
            height: 250px;
            width: 100%;
            margin-top: 15px;
        }
        
        .control-buttons {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            margin-top: 15px;
        }
        .btn {
            padding: 12px;
            border: none;
            border-radius: 5px;
            font-weight: 600;
            cursor: pointer;
            text-transform: uppercase;
            font-size: 13px;
            transition: background-color 0.2s;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }
        .btn-stop {
            background: var(--stop-btn);
            color: white;
        }
        .btn-reset {
            background: var(--reset-btn);
            color: white;
        }
        .alert {
            background: #FFEBEE;
            color: var(--stop-btn);
            padding: 12px;
            border-radius: 5px;
            text-align: center;
            font-weight: 600;
            font-size: 13px;
            border: 1px solid var(--stop-btn);
            display: none; /* Changed from inline style to initial hidden state */
            margin-top: 15px;
            animation: pulse-light 1.5s infinite;
        }
        @keyframes pulse-light {
            0%, 100% { background: #FFEBEE; }
            50% { background: #FFCDD2; }
        }
        
        .pan-controls {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-top: 10px;
        }
        .pan-button {
            width: 40px;
            height: 40px;
            background: #f0f0f0;
            border: 1px solid #ccc;
            color: var(--text-dark);
            font-size: 18px;
            line-height: 1;
            border-radius: 4px;
        }
        input[type=range] {
            flex-grow: 1;
            margin: 0 10px;
            -webkit-appearance: none;
            height: 10px;
            background: #ddd;
            border-radius: 5px;
        }
        input[type=range]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: var(--main-accent);
            cursor: pointer;
        }

    </style>
</head>
<body>
    <div class="main-grid-container">
        
        <div class="camera-control-wrapper">
            <div class="card">
                <div class="section-title">LIVE VIDEO FEED</div>
                <img id="view" src="/stream" alt="Live Stream"/>
            </div>

            <div class="card">
                <div class="section-title">CAMERA PAN CONTROL & SYSTEM CONTROL</div>
                <div class="pan-controls">
                    <button class="btn pan-button" id="panLeft" onclick="sendServoCommand(panSlider.value - 5)">◀</button>
                    <input type="range" min="0" max="180" value="90" class="slider" id="panSlider" 
                           oninput="updateServoDisplay(this.value)" onchange="sendServoCommand(this.value)">
                    <button class="btn pan-button" id="panRight" onclick="sendServoCommand(parseInt(panSlider.value) + 5)">▶</button>
                </div>
                <div style="text-align: center; margin-top: 5px; font-weight: 500;">Angle: <span id="servo-angle-display">90°</span></div>

                <div class="control-buttons">
                    <button class="btn btn-stop" id="emergencyStop" onclick="sendCommand('EMERGENCY_STOP')">EMERGENCY STOP</button>
                    <button class="btn btn-reset" id="resetCounters" onclick="sendCommand('RESET_COUNTERS')">RESET COUNTERS</button>
                </div>
                <div class="alert" id="alert">
                    ALERT: BIN A IS FULL! ACTION REQUIRED
                </div>
            </div>
        </div>

        <div class="dashboard-column">
            
            <div class="top-stats-row">
                <div class="card">
                    <div class="section-title">SYSTEM STATUS</div>
                    <div class="stats-grid">
                        <div class="stat-item">
                            <div class="stat-value" id="accuracy">0.0%</div>
                            <div class="stat-label">ACCURACY</div>
                        </div>
                        <div class="stat-item">
                            <div class="stat-value" id="speed">0</div>
                            <div class="stat-label">SORTING SPEED ()</div>
                        </div>
                        <div class="stat-item">
                            <div class="stat-value" id="status" style="color: var(--stopped);">STOPPED</div>
                            <div class="stat-label">SYSTEM STATE</div>
                        </div>
                    </div>
                </div>
                
                <div class="card">
                    <div class="section-title">COUNTER SUMMARY</div>
                    <div class="counters-summary-grid">
                        <div class="counter-item" style="border-left-color: var(--type-td);">
                            <div class="counter-label">TOTAL RED</div>
                            <div class="counter-value" id="countRed">0</div>
                        </div>
                        <div class="counter-item" style="border-left-color: var(--type-tv);">
                            <div class="counter-label">TOTAL YELLOW</div>
                            <div class="counter-value" id="countYellow">0</div>
                        </div>
                        <div class="counter-item" style="border-left-color: var(--type-tx);">
                            <div class="counter-label">TOTAL BLUE</div>
                            <div class="counter-value" id="countBlue">0</div>
                        </div>
                        <div class="counter-item" style="border-left-color: var(--type-other);">
                            <div class="counter-label">SKIPPED/FAILED</div>
                            <div class="counter-value" id="countSkipped">0</div>
                        </div>
                    </div>
                </div>
            </div>
            
            <div class="realtime-status-combined">
                <div class="card">
                    <div class="section-title">REAL-TIME CLASSIFICATION & COLOR</div>
                    <div class="stats-grid" style="grid-template-columns: 1fr 1fr;">
                        <div class="stat-item" style="border-left: 4px solid var(--main-accent);">
                            <div class="stat-value" id="currentCode">N/A</div>
                            <div class="stat-label">CLASSIFIED CODE</div>
                        </div>
                        <div class="stat-item" style="border-left: 4px solid var(--running);">
                            <div class="stat-value" id="cycleTime">N/A</div>
                            <div class="stat-label">LAST CYCLE TIME (s)</div>
                        </div>
                    </div>
                    <div class="real-time-detail">
                        <div><span class="rt-label">Shape:</span> <span class="rt-value" id="currentShape">N/A</span></div>
                        <div><span class="rt-label">Color Name:</span> <span class="rt-value" id="currentColor">N/A</span></div>
                        <div><span class="rt-label">R:</span> <span class="rt-value" id="valR">0</span></div>
                        <div><span class="rt-label">G:</span> <span class="rt-value" id="valG">0</span></div>
                        <div><span class="rt-label">B:</span> <span class="rt-value" id="valB">0</span></div>
                        <div><span class="rt-label">Retries:</span> <span class="rt-value" id="retryCount">0</span></div>
                        <div><span class="rt-label">Stage:</span> <span class="rt-value" id="currentStageText">0</span></div>
                        <div><span class="rt-label">Timestamp:</span> <span class="rt-value" id="timestamp">N/A</span></div>
                    </div>
                </div>

                <div class="card">
                    <div class="section-title">MACHINE STATUS FLAGS & TOTAL</div>
                    <div class="total-sorted">
                        <div class="total-value" id="totalSorted">0</div>
                        <div class="total-label">TOTAL OBJECTS PROCESSED</div>
                    </div>
                    <div class="stats-grid" style="grid-template-columns: 1fr 1fr 1fr; gap: 8px;">
                        <div class="stat-item" id="statusArm" style="border-left: 4px solid var(--warning);">
                            <div class="stat-value" id="valArm" style="font-size: 18px;">IDLE</div>
                            <div class="stat-label">ARM STATE</div>
                        </div>
                        <div class="stat-item" id="statusConveyor" style="border-left: 4px solid var(--running);">
                            <div class="stat-value" id="valConveyor" style="font-size: 18px;">STOP</div>
                            <div class="stat-label">CONVEYOR</div>
                        </div>
                        <div class="stat-item" id="statusIR" style="border-left: 4px solid var(--stopped);">
                            <div class="stat-value" id="valIR" style="font-size: 18px;">CLEAR</div>
                            <div class="stat-label">IR SENSOR</div>
                        </div>
                    </div>
                </div>
            </div>
            
            <div class="card">
                <div class="section-title">PRODUCT DISTRIBUTION </div>
                <div class="chart-container" style="height: 300px;">
                    <canvas id="productDistributionChart"></canvas>
                </div>
            </div>

            <div class="card">
                <div class="section-title">HOURLY THROUGHPUT () - MOCK CHART</div>
                <div class="chart-container" style="height: 180px;">
                    <canvas id="lineChart"></canvas>
                </div>
            </div>
            
        </div>
    </div>
<script>
    const ws = new WebSocket(`ws://${window.location.host}:81`);
    let productDistributionChart;
    let lineChart; 
    
    let globalStats = {
        total: 95,      
        skipped: 5,     
        TD: 20,          
        VD: 15,          
        TV: 25,          
        VV: 10,          
        TX: 15,          
        VX: 10           
    };
    let lastCycleTimeMs = 3000; 
    
    let panSlider = document.getElementById('panSlider');

    function initCharts() {
        const chartColors = [
            'var(--type-td)', 'var(--type-vd)', 'var(--type-tv)', 
            'var(--type-vv)', 'var(--type-tx)', 'var(--type-vx)',
            'var(--type-other)'
        ];
        
        const ctxBar = document.getElementById('productDistributionChart').getContext('2d');
        productDistributionChart = new Chart(ctxBar, {
            type: 'bar',
            data: {
                labels: ['R-Round (TD)', 'R-Square (VD)', 'Y-Round (TV)', 'Y-Square (VV)', 'B-Round (TX)', 'B-Square (VX)', 'Skipped'],
                datasets: [{
                    label: 'Quantity',
                    data: [0, 0, 0, 0, 0, 0, 0],
                    backgroundColor: chartColors,
                    borderColor: '#ccc',
                    borderWidth: 1
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: { beginAtZero: true },
                },
                plugins: { legend: { display: false } }
            }
        });

        const ctxLine = document.getElementById('lineChart').getContext('2d');
        lineChart = new Chart(ctxLine, {
            type: 'line',
            data: {
                labels: ['08:00', '09:00', '10:00', '11:00', '12:00', '13:00'],
                datasets: [{
                    label: '',
                    data: [10, 12, 15, 14, 11, 13], 
                    borderColor: 'var(--main-accent)',
                    backgroundColor: 'rgba(255, 112, 67, 0.2)',
                    tension: 0.3,
                    borderWidth: 2,
                    fill: true
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: { y: { beginAtZero: true, max: 20 } },
                plugins: { legend: { display: false } }
            }
        });
    }
    
    function updateDashboardMetrics(stats, cycleTimeMs) {
        const totalProcessed = stats.total + stats.skipped;
        
        const cycleTimeSec = cycleTimeMs / 1000.0;
        const speed = cycleTimeSec > 0 ? Math.round(60 / cycleTimeSec) : 0;
        
        const accuracy = totalProcessed > 0 
            ? ((stats.total / totalProcessed) * 100).toFixed(1) 
            : 100.0;

        document.getElementById('totalSorted').textContent = totalProcessed; 
        document.getElementById('accuracy').textContent = accuracy + '%';
        document.getElementById('speed').textContent = speed;

        document.getElementById('countRed').textContent = stats.TD + stats.VD;
        document.getElementById('countYellow').textContent = stats.TV + stats.VV;
        document.getElementById('countBlue').textContent = stats.TX + stats.VX;
        document.getElementById('countSkipped').textContent = stats.skipped;
        
        document.getElementById('cycleTime').textContent = cycleTimeSec > 0 
            ? cycleTimeSec.toFixed(2) + ' s' 
            : 'N/A';

        if (productDistributionChart) {
            productDistributionChart.data.datasets[0].data = [
                stats.TD, stats.VD, stats.TV,  
                stats.VV, stats.TX, stats.VX,
                stats.skipped 
            ];
            productDistributionChart.update();
        }
    }

    function sendCommand(cmd) {
        ws.send("CMD:" + cmd);
        console.log(`Sent command: ${cmd}`);
        if (cmd === 'RESET_COUNTERS') {
            resetLocalStats();
        }
    }
    
    function sendServoCommand(angle) {
        angle = Math.max(0, Math.min(180, angle));
        panSlider.value = angle;
        updateServoDisplay(angle);
        ws.send("CMD:PAN=" + angle);
        console.log(`Sent Servo Angle: ${angle}`);
    }
    
    function updateServoDisplay(angle) {
        document.getElementById('servo-angle-display').textContent = angle + '°';
    }
    
    function resetLocalStats() {
        globalStats = { total: 0, skipped: 0, TD: 0, VD: 0, TV: 0, VV: 0, TX: 0, VX: 0 };
        lastCycleTimeMs = 0;
        
        updateDashboardMetrics(globalStats, lastCycleTimeMs);
    }

    function updateRealtimeData(data) {
        document.getElementById('currentCode').textContent = data.code || 'N/A';
        document.getElementById('currentShape').textContent = data.shape || 'N/A';
        document.getElementById('currentColor').textContent = data.colorName || 'N/A';
        document.getElementById('valR').textContent = data.R !== undefined ? data.R : 0;
        document.getElementById('valG').textContent = data.G !== undefined ? data.G : 0;
        document.getElementById('valB').textContent = data.B !== undefined ? data.B : 0;
        document.getElementById('retryCount').textContent = data.retryCount !== undefined ? data.retryCount : 0;
        document.getElementById('currentStageText').textContent = data.currentStage !== undefined ? data.currentStage : 0;
    }

    function updateStatusFlags(data) {
        const isRunning = data.armActive || data.conveyorActive;
        const statusElement = document.getElementById('status');
        
        statusElement.textContent = isRunning ? 'RUNNING' : 'STOPPED';
        statusElement.style.color = isRunning ? 'var(--running)' : 'var(--stopped)';
        
        const armState = data.armActive ? 'ACTIVE' : 'IDLE';
        const convState = data.conveyorActive ? 'ACTIVE' : 'STOP';
        const irState = data.irBlocked ? 'BLOCKED' : 'CLEAR';
        
        document.getElementById('valArm').textContent = armState;
        document.getElementById('valConveyor').textContent = convState;
        document.getElementById('valIR').textContent = irState;
        
        document.getElementById('statusArm').style.borderLeftColor = data.armActive ? 'var(--warning)' : 'var(--running)';
        document.getElementById('statusConveyor').style.borderLeftColor = data.conveyorActive ? 'var(--running)' : 'var(--stopped)';
        document.getElementById('statusIR').style.borderLeftColor = data.irBlocked ? 'var(--stopped)' : 'var(--running)';
        
        document.getElementById('alert').style.display = data.alertStatus ? 'block' : 'none';
    }
    
    function processCompletedEvent(data) {
        if (!data.stats) return;

        globalStats.total = data.stats.total || 0;
        globalStats.skipped = data.stats.skipped || 0;
        globalStats.TD = data.stats.TD || 0;
        globalStats.VD = data.stats.VD || 0;
        globalStats.TV = data.stats.TV || 0;
        globalStats.VV = data.stats.VV || 0;
        globalStats.TX = data.stats.TX || 0;
        globalStats.VX = data.stats.VX || 0;
        
        lastCycleTimeMs = data.cycleTimeMs || 0;
        
        updateDashboardMetrics(globalStats, lastCycleTimeMs);
    }

    ws.onmessage = function(event) {
        try {
            const data = JSON.parse(event.data);
            
            switch(data.type) {
                case "REALTIME":
                    updateRealtimeData(data);
                    break;
                case "STATUS":
                    updateStatusFlags(data);
                    break;
                case "COMPLETED":
                    processCompletedEvent(data);
                    break;
                default:
                    console.warn("Unknown payload type:", data.type);
                    break;
            }
        } catch (e) {
            console.error('Error parsing JSON:', e);
        }
    };

    document.addEventListener('DOMContentLoaded', function() {
        initCharts();
        updateServoDisplay(panSlider.value);
        
        updateDashboardMetrics(globalStats, lastCycleTimeMs);
        
        updateStatusFlags({ armActive: false, conveyorActive: false, irBlocked: false, alertStatus: false });
    });

</script>
</body>
</html>
)HTML";


static esp_err_t stream_handler(httpd_req_t* req) {
  camera_fb_t* fb = NULL;
  char part_buf[64];
  static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=frame";
  static const char* _STREAM_BOUNDARY = "\r\n--frame\r\n";
  static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";
  httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }
    size_t hlen = snprintf(part_buf, 64, _STREAM_PART, fb->len);
    httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    httpd_resp_send_chunk(req, part_buf, hlen);
    httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
  return ESP_OK;
}

void sendCommandToUno(const String& command) {
  if (command.startsWith("CMD:PAN=")) {
    String panValue = command.substring(8);
    Serial2.print("SERVO_POS:");
    Serial2.println(panValue);
    Serial.println("Gửi lệnh Servo: " + panValue);
  } else if (command.endsWith("EMERGENCY_STOP")) {
    Serial2.println("STOP");
    Serial.println("Gửi lệnh: STOP");
  } else if (command.endsWith("RESET_COUNTERS")) {
    Serial2.println("RESET");
    Serial.println("Gửi lệnh: RESET");
  }
}

static esp_err_t index_handler(httpd_req_t* req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, INDEX_HTML, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t emergency_stop_handler(httpd_req_t* req) {
  sendCommandToUno("CMD:EMERGENCY_STOP");
  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, "{\"status\":\"stopped\", \"command\":\"STOP\"}");
  return ESP_OK;
}

static esp_err_t reset_counters_handler(httpd_req_t* req) {
  sendCommandToUno("CMD:RESET_COUNTERS");
  httpd_resp_set_type(req, "application/json");
  httpd_resp_sendstr(req, "{\"status\":\"reset\", \"command\":\"RESET\"}");
  return ESP_OK;
}

static httpd_handle_t startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  config.stack_size = 8192;

  if (httpd_start(&http_server, &config) == ESP_OK) {
    httpd_uri_t index = { "/", HTTP_GET, index_handler, NULL };
    httpd_uri_t stream = { "/stream", HTTP_GET, stream_handler, NULL };
    httpd_uri_t emergency_stop = { "/emergency_stop", HTTP_GET, emergency_stop_handler, NULL };
    httpd_uri_t reset_counters = { "/reset_counters", HTTP_GET, reset_counters_handler, NULL };

    httpd_register_uri_handler(http_server, &index);
    httpd_register_uri_handler(http_server, &stream);
    httpd_register_uri_handler(http_server, &emergency_stop);
    httpd_register_uri_handler(http_server, &reset_counters);
  }
  return http_server;
}

void sendStatsToClients(String& payload) {
  lastJsonPayload = payload;

  String mutablePayload = payload;

  webSocket.broadcastTXT(mutablePayload);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Client disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);

        if (lastJsonPayload.length() > 0) {
          webSocket.sendTXT(num, lastJsonPayload);
        }
        break;
      }
    case WStype_TEXT:
      {
        String command = String((char*)payload);
        Serial.printf("[%u] Received command: %s\n", num, command.c_str());

        if (command.startsWith("CMD:")) {
          sendCommandToUno(command);
        }
      }

      break;
    default:
      break;
  }
}

void handleUnoData() {
  if (Serial2.available()) {
    String jsonString = Serial2.readStringUntil('\n');
    jsonString.trim();

    if (jsonString.length() > 0) {
      DeserializationError error = deserializeJson(doc, jsonString);

      if (error == DeserializationError::Ok) {
        sendStatsToClients(jsonString);
      } else {
        Serial.printf("Error parsing JSON from Uno: %s\n", error.c_str());
      }
    }
  }
}

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 24000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed 0x%x\n", err);
    return false;
  }
  return true;
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Connecting to %s", WIFI_SSID);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n Wi-Fi connected! IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\n FAILED TO CONNECT WI-FI! Check SSID/PASS.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n Initializing system...");

  Serial2.begin(9600, SERIAL_8N1, RXD2_PIN, TXD2_PIN);

  if (!initCamera()) {
    Serial.println(" Camera initialization failed");
    while (true)
      ;
  }

  pinMode(LED_FLASH_PIN, OUTPUT);
  digitalWrite(LED_FLASH_PIN, LOW);

  connectWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    startCameraServer();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println(" DASHBOARD IS READY!");
    Serial.println(" Access at: http://" + WiFi.localIP().toString());
  } else {
    Serial.println(" Cannot start server due to Wi-Fi error");
  }
}

void loop() {
  webSocket.loop();
  handleUnoData();
  delay(10);
}