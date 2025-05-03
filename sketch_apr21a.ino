#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Replace with your Wi-Fi credentials
const char* ssid = "Potterhead";
const char* password = "12345678";

// GPIO pins
const int flameSensorPin = D1;  // Flame sensor input
const int ledPin = D5;          // LED output
const int buzzerPin = D6;       // Buzzer output

bool fireDetected = false;

ESP8266WebServer server(80);

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8">
      <title>🔥 Fire Alert | ESP8266</title>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <style>
        body {
          margin: 0;
          padding: 0;
          font-family: 'Segoe UI', sans-serif;
          background: url('https://wallpapercave.com/wp/wp7090411.jpg') no-repeat center center fixed;
          background-size: cover;
          color: white;
          text-align: center;
        }
        .overlay {
          background-color: rgba(0, 0, 0, 0.7);
          min-height: 100vh;
          display: flex;
          flex-direction: column;
          justify-content: center;
          align-items: center;
          padding: 20px;
        }
        h1 {
          font-size: 2.5em;
          margin-bottom: 20px;
          animation: fadeIn 1s ease-in-out;
        }
        .status {
          font-size: 1.5em;
          padding: 15px 30px;
          border-radius: 15px;
          margin-top: 20px;
          display: inline-block;
          background-color: rgba(255, 255, 255, 0.2);
        }
        @keyframes fadeIn {
          from { opacity: 0; transform: translateY(-10px); }
          to { opacity: 1; transform: translateY(0); }
        }
      </style>
      <script>
        setTimeout(function() {
          location.reload();
        }, 3000);
      </script>
    </head>
    <body>
      <div class="overlay">
        <h1>🔥 Fire Detection System 🔥</h1>
  )rawliteral";

  if (fireDetected) {
    html += R"rawliteral(
        <div class="status" style="color: red; background-color: rgba(255, 0, 0, 0.3);">
          <strong>🔥 Fire Detected!</strong><br>Take Immediate Action!
        </div>
    )rawliteral";
  } else {
    html += R"rawliteral(
        <div class="status" style="color: lightgreen; background-color: rgba(0, 128, 0, 0.3);">
          ✅ All Clear<br>No Fire Detected
        </div>
    )rawliteral";
  }

  html += R"rawliteral(
      </div>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(flameSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start mDNS
  if (MDNS.begin("fire-alert")) {
    Serial.println("mDNS responder started: http://fire-alert.local");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Flame sensor active LOW: LOW = fire
  fireDetected = digitalRead(flameSensorPin) == LOW;

  if (fireDetected) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  server.handleClient();
}