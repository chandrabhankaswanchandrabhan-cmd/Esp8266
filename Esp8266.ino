#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

// Telegram config - LIVE CREDENTIALS
const char* botToken = "7377856555:AAH-Dn5E6x3OaL3y2V8oqY2h7iY6qK1jK0M";
const char* chatId = "6617462455";

// Fallback hotspot
const char* fallbackSSID = "redmi";
const char* fallbackPass = "12345678";

// Web server
ESP8266WebServer server(80);

// Timers
unsigned long startTime = 0;
const unsigned long TOTAL_RUNTIME = 5 * 60 * 1000; // 5 minutes trial
const unsigned long SCREEN_UPDATE = 5000; // Update every 5s

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("🚀 Agent 4.0 HACKED Screen - LIVE");
  
  // 5s deploy delay
  delay(5000);
  
  startTime = millis();
  
  // Start HACKED AP
  startHackedAP();
  
  // Telegram alert
  sendTelegram("🔥 AGENT 4.0 TRIAL LIVE!\n📱 HACKED Screen: redmi/12345678\n⏱️ 5min mission");
}

void loop() {
  unsigned long now = millis();
  
  // Auto-kill
  if (now - startTime > TOTAL_RUNTIME) {
    sendTelegram("💀 Agent 4.0 self-destruct complete");
    delay(2000);
    ESP.restart();
  }
  
  // Handle screen requests
  server.handleClient();
  
  delay(100);
}

void startHackedAP() {
  WiFi.mode(WIFI_AP);
  bool apStarted = WiFi.softAP(fallbackSSID, fallbackPass);
  
  if (apStarted) {
    IPAddress IP = WiFi.softAPIP();
    Serial.printf("✅ HACKED AP: %s @ %s\n", fallbackSSID, IP.toString().c_str());
    
    // Web routes
    server.on("/", handleHackedPage);
    server.on("/style.css", handleCSS);
    server.on("/audio", handleBeep);
    server.begin();
    
    Serial.println("🌐 Web server ready - connect to see HACKED screen");
  } else {
    Serial.println("❌ AP failed");
  }
}

void handleHackedPage() {
  String uptime = String((millis() - startTime) / 1000);
  String remaining = String((TOTAL_RUNTIME - (millis() - startTime)) / 1000);
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv="refresh" content="5">
<link rel="stylesheet" href="/style.css">
<title>HACKED</title>
</head>
<body>
<div class="matrix">
  <div class="center">
    <h1>YOU ARE HACKING</h1>
    <div class="blink">● ACTIVE</div>
    <p>Uptime: )" + uptime + R"rawliteral( sec</p>
    <p>Remaining: )" + remaining + R"rawliteral( sec</p>
    <div class="warning">AGENT 4.0</div>
  </div>
</div>
<audio autoplay loop>
<source src="/audio" type="audio/wav">
</audio>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

void handleCSS() {
  String css = R"rawliteral(
* { margin: 0; padding: 0; box-sizing: border-box; }
body { 
  background: #000; 
  color: #00ff41; 
  font-family: 'Courier New', monospace; 
  overflow: hidden;
  height: 100vh;
}

.matrix {
  position: relative;
  width: 100%;
  height: 100vh;
  background: linear-gradient(180deg, #000 0%, #001100 50%, #000 100%);
  animation: glitch 3s infinite;
}

.center {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  text-align: center;
  z-index: 10;
}

h1 {
  font-size: clamp(2.5em, 8vw, 6em);
  margin: 0 0 20px;
  text-shadow: 0 0 10px #00ff41, 0 0 20px #00ff41;
  animation: glow 2s infinite alternate, flicker 0.15s infinite;
  letter-spacing: 5px;
}

@keyframes glow {
  0% { text-shadow: 0 0 20px #00ff41, 0 0 30px #00ff41; }
  100% { text-shadow: 0 0 30px #00ff41, 0 0 40px #00ff41, 0 0 50px #00ff41; }
}

@keyframes flicker {
  0%, 18%, 22%, 25%, 53%, 57%, 100% { opacity: 1; }
  20%, 24%, 55% { opacity: 0.4; }
}

@keyframes glitch {
  0%, 100% { transform: translate(0); }
  20% { transform: translate(-2px, 2px); }
  40% { transform: translate(-2px, -2px); }
  60% { transform: translate(2px, 2px); }
  80% { transform: translate(2px, -2px); }
}

.blink {
  font-size: 2em;
  animation: blink 1s infinite;
  margin: 20px 0;
}

@keyframes blink {
  0%, 50% { opacity: 1; }
  51%, 100% { opacity: 0; }
}

.warning {
  font-size: 1.2em;
  color: #ff0040;
  text-shadow: 0 0 10px #ff0040;
  animation: pulse 1.5s infinite;
}

@keyframes pulse {
  0%, 100% { transform: scale(1); }
  50% { transform: scale(1.05); }
}

p { 
  font-size: 1.3em; 
  margin: 10px 0; 
  opacity: 0.9;
}
)rawliteral";
  
  server.send(200, "text/css", css);
}

void handleBeep() {
  // Simple beep tone (browser synthesized)
  String audio = R"rawliteral(
data:audio/wav;base64,UklGRnoGAABXQVZFZm10IBAAAAABAAEAQB8AAEAfAAABAAgAZGF0YQoGAACBhYqFbF1fdJivrJBhNjVgodDbq2EcBj+a2/LDciUFLIHO8tiJNwgZaLvt559NEAxQp+PwtmMcBjiR1/LMeSwFJHfH8N2QQAo
)rawliteral";
  server.sendHeader("Content-Disposition", "inline; filename=beep.wav");
  server.send(200, "audio/wav", audio);
}

void sendTelegram(String message) {
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();
  
  String url = "https://api.telegram.org/bot";
  url += botToken;
  url += "/sendMessage?chat_id=";
  url += chatId;
  url += "&text=";
  url += urlEncode(message);
  url += "&parse_mode=HTML";
  
  http.begin(client, url);
  http.addHeader("User-Agent", "ESP8266-Agent4.0");
  
  int httpCode = http.GET();
  if (httpCode == 200) {
    Serial.println("✅ Telegram sent");
  } else {
    Serial.printf("❌ Telegram failed: %d\n", httpCode);
  }
  http.end();
}

String urlEncode(String str) {
  String encoded = str;
  encoded.replace(" ", "%20");
  encoded.replace("!", "%21");
  encoded.replace("#", "%23");
  encoded.replace("$", "%24");
  encoded.replace("&", "%26");
  encoded.replace("'", "%27");
  encoded.replace("(", "%28");
  encoded.replace(")", "%29");
  encoded.replace("*", "%2A");
  encoded.replace("+", "%2B");
  encoded.replace(",", "%2C");
  encoded.replace("/", "%2F");
  encoded.replace(":", "%3A");
  encoded.replace(";", "%3B");
  encoded.replace("=", "%3D");
  encoded.replace("?", "%3F");
  encoded.replace("@", "%40");
  return encoded;
}
