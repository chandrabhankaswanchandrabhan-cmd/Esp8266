#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Ticker.h>

// HARDCODED - TERA HOTSPOT
const char* ssid = "redmi";
const char* password = "12345678";
String telegram_token = "8560004612:AAHS3lGPkfLS0OWkLUVZl1nPXX1y1ewrz30";
String chat_id = "7440499919";

Ticker killTimer;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n🚀 NodeMCU ESP8266 Agent 4.0 TRIAL booting...");
  
  // Tera HotSPOT connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to redmi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to redmi!");
    Serial.println("IP: " + WiFi.localIP().toString());
    sendTelegram("🥳 **Agent 4.0 TRIAL** NodeMCU ESP8266\n✅ Hotspot: *redmi* connected!\n📍 IP: " + WiFi.localIP().toString() + "\n⏳ 5min trial start...");
    
    // 5 MIN KILL TIMER
    killTimer.once(300, selfKill);  // 300 sec = 5 min
    Serial.println("⏳ 5min timer started...");
    
  } else {
    Serial.println("\n❌ redmi connect FAIL!");
    sendTelegram("⚠️ Agent 4.0 TRIAL: redmi hotspot FAIL");
    ESP.restart();
  }
}

void loop() {
  delay(1000);
  Serial.print("Alive... Signal: ");
  Serial.println(WiFi.RSSI());
}

void selfKill() {
  Serial.println("\n💀 5min TRIAL END - Self destruct!");
  sendTelegram("💀 **Agent 4.0 TRIAL** self-killed after 5min\n✅ Mission success!\nNodeMCU ESP8266 offline.");
  
  // ESP restart (kill)
  ESP.restart();
}

void sendTelegram(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.telegram.org/bot" + telegram_token + "/sendMessage?chat_id=" + chat_id + "&text=" + urlEncode(message) + "&parse_mode=Markdown");
    http.addHeader("User-Agent", "ESP8266");
    
    int httpCode = http.GET();
    if (httpCode == 200) {
      Serial.println("✅ Telegram sent!");
    } else {
      Serial.println("❌ Telegram fail: " + String(httpCode));
    }
    http.end();
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
