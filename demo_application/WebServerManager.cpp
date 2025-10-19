#include "WebServerManager.h"

void WebServerManager::begin() {
    server.begin();
    Serial.println(" HTTP server started on port " + String(WEB_SERVER_PORT));
}

void WebServerManager::handleClient() {
    WiFiClient client = server.available();
    if (!client) return;
    
    // Wait for client to send data
    unsigned long timeout = millis() + 5000;
    while (!client.available() && millis() < timeout) {
        delay(10);
    }
    
    if (!client.available()) {
        client.stop();
        return;
    }
    
    String request = client.readStringUntil('\r');
    client.flush();
    
    Serial.println("HTTP Request: " + request);
    
    // Read request body for POST requests
    String body;
    if (request.startsWith("POST")) {
        while (client.available()) {
            body += client.readString();
        }
    }
    
    // Route handling
    if (request.indexOf("GET / ") != -1 || request.indexOf("GET /index") != -1) {
        sendMainPage(client);
    }
    else if (request.indexOf("GET /api/status") != -1) {
        sendJSONStatus(client);
    }
    else if (request.indexOf("POST /api/light") != -1) {
        handleLightControl(client, body);
    }
    else if (request.indexOf("POST /api/ac") != -1) {
        handleACControl(client, body);
    }
    else if (request.indexOf("POST /api/temperature") != -1) {
        handleTemperatureControl(client, body);
    }
    else {
        // Send 404 for unknown routes
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("404 - Page Not Found");
    }
    
    delay(10);
    client.stop();
}

void WebServerManager::sendMainPage(WiFiClient& client) {
    String html = getHTMLContent();
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(html);
}

void WebServerManager::sendJSONStatus(WiFiClient& client) {
    DeviceState state = deviceManager->getState();
    StaticJsonDocument<512> doc;
    
    doc["lightState"] = state.lightState;
    doc["lightBrightness"] = state.lightBrightness;
    doc["temperature"] = state.temperature;
    doc["acState"] = state.acState;
    doc["acMode"] = state.acMode;
    doc["fanSpeed"] = state.fanSpeed;
    doc["ipAddress"] = WiFi.localIP().toString();
    doc["bacnetDeviceId"] = bacnetController->getDeviceInstance();
    doc["uptime"] = millis() / 1000;
    
    String response;
    serializeJson(doc, response);
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println(response);
}

void WebServerManager::handleLightControl(WiFiClient& client, const String& body) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);
    
    if (!error) {
        if (doc.containsKey("state")) {
            deviceManager->setLightState(doc["state"]);
        }
        if (doc.containsKey("brightness")) {
            deviceManager->setLightBrightness(doc["brightness"]);
        }
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println("{\"status\":\"success\",\"message\":\"Light control updated\"}");
    } else {
        client.println("HTTP/1.1 400 Bad Request");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println("{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
    }
}

void WebServerManager::handleACControl(WiFiClient& client, const String& body) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);
    
    if (!error) {
        if (doc.containsKey("state")) {
            deviceManager->setACState(doc["state"]);
        }
        if (doc.containsKey("mode")) {
            deviceManager->setACMode(doc["mode"].as<String>());
        }
        if (doc.containsKey("fanSpeed")) {
            deviceManager->setFanSpeed(doc["fanSpeed"]);
        }
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println("{\"status\":\"success\",\"message\":\"AC control updated\"}");
    } else {
        client.println("HTTP/1.1 400 Bad Request");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println("{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
    }
}

void WebServerManager::handleTemperatureControl(WiFiClient& client, const String& body) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);
    
    if (!error) {
        if (doc.containsKey("temperature")) {
            deviceManager->setTemperature(doc["temperature"]);
        }
        
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println("{\"status\":\"success\",\"message\":\"Temperature updated\"}");
    } else {
        client.println("HTTP/1.1 400 Bad Request");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println("{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
    }
}

String WebServerManager::getHTMLContent() {
    return R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>BACnet ESP8266 Building Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; }
        .container { max-width: 800px; margin: 0 auto; }
        .header { text-align: center; color: white; margin-bottom: 30px; }
        .header h1 { font-size: 2.5em; margin-bottom: 10px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }
        .header p { font-size: 1.2em; opacity: 0.9; }
        .card { background: rgba(255, 255, 255, 0.95); padding: 25px; margin: 20px 0; border-radius: 15px; box-shadow: 0 8px 32px rgba(0,0,0,0.1); backdrop-filter: blur(10px); border: 1px solid rgba(255,255,255,0.2); }
        .card h2 { color: #333; margin-bottom: 20px; font-size: 1.5em; border-bottom: 2px solid #667eea; padding-bottom: 10px; }
        .status { padding: 15px; background: #e8f5e8; border-radius: 10px; margin: 15px 0; border-left: 4px solid #4CAF50; font-weight: 500; }
        .control-group { margin: 15px 0; }
        .control-label { display: block; margin-bottom: 8px; font-weight: 600; color: #555; }
        .switch { position: relative; display: inline-block; width: 70px; height: 38px; }
        .switch input { opacity: 0; width: 0; height: 0; }
        .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }
        .slider:before { position: absolute; content: ""; height: 30px; width: 30px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
        input:checked + .slider { background-color: #2196F3; }
        input:checked + .slider:before { transform: translateX(32px); }
        .slider-text { margin-left: 15px; font-weight: 600; color: #666; }
        button { background: #667eea; color: white; border: none; padding: 12px 24px; border-radius: 8px; cursor: pointer; margin: 5px; font-size: 14px; font-weight: 600; transition: all 0.3s ease; box-shadow: 0 4px 15px rgba(102, 126, 234, 0.3); }
        button:hover { background: #5a6fd8; transform: translateY(-2px); box-shadow: 0 6px 20px rgba(102, 126, 234, 0.4); }
        button.active { background: #4CAF50; box-shadow: 0 4px 15px rgba(76, 175, 80, 0.3); }
        input[type="range"] { width: 100%; margin: 10px 0; }
        .value-display { display: inline-block; min-width: 60px; text-align: center; font-weight: 600; color: #667eea; }
        .system-info { background: #f8f9fa; padding: 15px; border-radius: 10px; margin-top: 10px; font-family: 'Courier New', monospace; font-size: 0.9em; }
        .loading { color: #666; font-style: italic; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1> BACnet ESP8266 Controller</h1>
            <p>Mobile-Integrated Building Management System</p>
        </div>
        
        <div class="card">
            <h2> Smart Lighting Control</h2>
            <div class="status" id="lightStatus">Loading...</div>
            <div class="control-group">
                <label class="control-label">Light Switch:</label>
                <label class="switch">
                    <input type="checkbox" id="lightSwitch" onchange="toggleLight()">
                    <span class="slider"></span>
                </label>
                <span class="slider-text" id="lightSwitchText">OFF</span>
            </div>
            <div class="control-group">
                <label class="control-label">Brightness Control:</label>
                <input type="range" id="brightnessSlider" min="0" max="100" value="0" 
                       oninput="updateBrightnessDisplay(this.value)" 
                       onchange="setBrightness(this.value)">
                <span class="value-display" id="brightnessValue">0%</span>
            </div>
        </div>
        
        <div class="card">
            <h2> Air Conditioning Control</h2>
            <div class="status" id="acStatus">Loading...</div>
            <div class="control-group">
                <label class="control-label">AC Power:</label>
                <label class="switch">
                    <input type="checkbox" id="acSwitch" onchange="toggleAC()">
                    <span class="slider"></span>
                </label>
                <span class="slider-text" id="acSwitchText">OFF</span>
            </div>
            <div class="control-group">
                <label class="control-label">Temperature Setpoint:</label>
                <input type="range" id="tempSlider" min="16" max="30" value="22" 
                       oninput="updateTempDisplay(this.value)" 
                       onchange="setTemperature(this.value)">
                <span class="value-display" id="tempValue">22°C</span>
            </div>
            <div class="control-group">
                <label class="control-label">Operation Mode:</label>
                <button id="btnCool" onclick="setACMode('cool')" class="active">Cool</button>
                <button id="btnHeat" onclick="setACMode('heat')">Heat</button>
                <button id="btnAuto" onclick="setACMode('auto')">Auto</button>
            </div>
            <div class="control-group">
                <label class="control-label">Fan Speed:</label>
                <button id="btnFanLow" onclick="setFanSpeed(1)">Low</button>
                <button id="btnFanMed" onclick="setFanSpeed(2)" class="active">Medium</button>
                <button id="btnFanHigh" onclick="setFanSpeed(3)">High</button>
                <button id="btnFanAuto" onclick="setFanSpeed(0)">Auto</button>
            </div>
        </div>
        
        <div class="card">
            <h2> System Information</h2>
            <div class="system-info">
                <div id="systemInfo" class="loading">Loading system information...</div>
            </div>
        </div>
    </div>

    <script>
        // JavaScript from previous implementation
        function updateStatus() {
            fetch('/api/status')
                .then(response => {
                    if (!response.ok) throw new Error('Network response was not ok');
                    return response.json();
                })
                .then(data => {
                    document.getElementById('lightStatus').textContent = 
                        ` Light: ${data.lightState ? 'ON' : 'OFF'} | Brightness: ${data.lightBrightness}%`;
                    document.getElementById('acStatus').textContent = 
                        ` AC: ${data.acState ? 'ON' : 'OFF'} | Mode: ${data.acMode.toUpperCase()} | Temperature: ${data.temperature}°C | Fan: ${getFanSpeedText(data.fanSpeed)}`;
                    document.getElementById('systemInfo').innerHTML = 
                        ` IP Address: <strong>${data.ipAddress}</strong><br> BACnet Device ID: <strong>${data.bacnetDeviceId}</strong><br>⏰ System Uptime: <strong>${formatUptime(data.uptime)}</strong>`;
                    updateUIControls(data);
                })
                .catch(error => {
                    console.error('Error fetching status:', error);
                    document.getElementById('systemInfo').innerHTML = ' Error connecting to device. Please check if ESP8266 is running.';
                });
        }
        
        function updateUIControls(data) {
            document.getElementById('lightSwitch').checked = data.lightState;
            document.getElementById('lightSwitchText').textContent = data.lightState ? 'ON' : 'OFF';
            document.getElementById('brightnessSlider').value = data.lightBrightness;
            document.getElementById('brightnessValue').textContent = data.lightBrightness + '%';
            document.getElementById('acSwitch').checked = data.acState;
            document.getElementById('acSwitchText').textContent = data.acState ? 'ON' : 'OFF';
            document.getElementById('tempSlider').value = data.temperature;
            document.getElementById('tempValue').textContent = data.temperature + '°C';
            updateActiveButton('btn' + data.acMode.charAt(0).toUpperCase() + data.acMode.slice(1));
            updateActiveButton('btnFan' + getFanSpeedText(data.fanSpeed));
        }
        
        function updateActiveButton(activeId) {
            const buttons = document.querySelectorAll('button');
            buttons.forEach(btn => btn.classList.remove('active'));
            const activeBtn = document.getElementById(activeId);
            if (activeBtn) activeBtn.classList.add('active');
        }
        
        function getFanSpeedText(speed) {
            switch(parseInt(speed)) {
                case 0: return 'Auto'; case 1: return 'Low'; case 2: return 'Med'; case 3: return 'High'; default: return 'Unknown';
            }
        }
        
        function formatUptime(seconds) {
            const hours = Math.floor(seconds / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            const secs = seconds % 60;
            return `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
        }
        
        function toggleLight() { sendCommand('/api/light', {state: document.getElementById('lightSwitch').checked}); }
        function updateBrightnessDisplay(value) { document.getElementById('brightnessValue').textContent = value + '%'; }
        function setBrightness(value) { sendCommand('/api/light', {brightness: parseInt(value)}); }
        function toggleAC() { sendCommand('/api/ac', {state: document.getElementById('acSwitch').checked}); }
        function updateTempDisplay(value) { document.getElementById('tempValue').textContent = value + '°C'; }
        function setTemperature(value) { sendCommand('/api/temperature', {temperature: parseFloat(value)}); }
        function setACMode(mode) { sendCommand('/api/ac', {mode: mode}); }
        function setFanSpeed(speed) { sendCommand('/api/ac', {fanSpeed: speed}); }
        
        function sendCommand(endpoint, data) {
            fetch(endpoint, { method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify(data) })
            .then(response => { if (!response.ok) throw new Error('Command failed'); return response.json(); })
            .then(result => { console.log('Command successful:', result); updateStatus(); })
            .catch(error => { console.error('Error sending command:', error); alert('Error sending command. Please check connection.'); });
        }
        
        setInterval(updateStatus, 3000);
        updateStatus();
    </script>
</body>
</html>
)=====";
}