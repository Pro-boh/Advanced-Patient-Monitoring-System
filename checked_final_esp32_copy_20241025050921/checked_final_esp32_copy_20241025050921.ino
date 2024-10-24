#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Pixel_3125";
const char* password = "promaxpro";

WebServer server(80);

// Variables to store sensor data
String temperature = "0";
String ecgValue = "0";
String value = "0";    // Replacing sensorValue1 with value
String BPM = "0";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>Health Monitoring Dashboard</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f0f2f5;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
        }
        .card {
            background-color: white;
            border-radius: 10px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            padding: 20px;
            margin-bottom: 20px;
        }
        h1 {
            color: #1a73e8;
            text-align: center;
            margin-bottom: 30px;
        }
        .sensor-value {
            font-size: 2em;
            font-weight: bold;
            text-align: center;
            margin: 10px 0;
        }
        .label {
            color: #5f6368;
            text-align: center;
            font-size: 1.2em;
            margin-bottom: 5px;
        }
    </style>
    <script>
        setInterval(function() {
            getData();
        }, 1000); // Update every second
        
        function getData() {
            var xhr = new XMLHttpRequest();
            xhr.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    var data = JSON.parse(this.responseText);
                    document.getElementById("temperature").innerHTML = data.temperature;
                    document.getElementById("bpm").innerHTML = data.bpm;
                    document.getElementById("ecg").innerHTML = data.ecg;
                    document.getElementById("value").innerHTML = data.value;  // Updated to use "value"
                }
            };
            xhr.open("GET", "/readings", true);
            xhr.send();
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Health Monitoring Dashboard</h1>
        
        <div class="card">
            <div class="label">Body Temperature</div>
            <div class="sensor-value">
                <span id="temperature">--</span>°C
            </div>
        </div>
        
        <div class="card">
            <div class="label">Heart Rate</div>
            <div class="sensor-value">
                <span id="bpm">--</span> BPM
            </div>
        </div>

        <div class="card">
            <div class="label">ECG Reading</div>
            <div class="sensor-value">
                <span id="ecg">--</span>
            </div>
        </div>

        <div class="card">
            <div class="label">Waist</div>
            <div class="sensor-value">
                <span id="value">--</span> cm <!-- Changed id from "ir" to "value" -->
            </div>
        </div>
    </div>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleReadings() {
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["ecg"] = ecgValue;
  doc["value"] = value;  // Updated to use "value"
  doc["bpm"] = BPM;
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  server.send(200, "application/json", jsonString);
}

void setup() {
  Serial.begin(115200);   // Initialize Serial communication
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // Initialize UART on pins 16 (RX) and 17 (TX)
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  // Print ESP32 IP address
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up web server routes
  server.on("/", handleRoot);
  server.on("/readings", handleReadings);
  
  // Start server
  server.begin();
}

void loop() {
  server.handleClient();
  
  if (Serial2.available()) {
    String receivedData = Serial2.readStringUntil('\n'); // Read data until newline
    
    // Parse the received data
    temperature = receivedData.substring(0, receivedData.indexOf(','));
    receivedData = receivedData.substring(receivedData.indexOf(',') + 1);
    ecgValue = receivedData.substring(0, receivedData.indexOf(','));
    receivedData = receivedData.substring(receivedData.indexOf(',') + 1);
    value = receivedData.substring(0, receivedData.indexOf(','));  // Updated to use "value"
    BPM = receivedData.substring(receivedData.indexOf(',') + 1);
    
    // Debug print
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("ECG Value: ");
    Serial.println(ecgValue);
    Serial.print("Waist: ");  // Updated to print "value"
    Serial.println(value);
    Serial.print("BPM: ");
    Serial.println(BPM);
  }
}
