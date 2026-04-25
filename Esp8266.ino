#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define MAX_NETWORKS 10  // अधिकतम नेटवर्क की संख्या
#define EEPROM_SIZE (MAX_NETWORKS * 33)  // हर SSID के लिए 32 कैरेक्टर + 1

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  EEPROM.begin(EEPROM_SIZE);  // EEPROM शुरू करें

  Serial.println("WiFi स्कैन शुरू हो रहा है...");
  int n = WiFi.scanNetworks();

  if (n == 0) {
    Serial.println("कोई नेटवर्क नहीं मिला");
  } else {
    Serial.print(n);
    Serial.println(" नेटवर्क्स पाए गए:");
    for (int i = 0; i < n && i < MAX_NETWORKS; i++) {
      String ssid = WiFi.SSID(i);
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(ssid);
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm)");
      Serial.println();

      // EEPROM में सेव करें
      int pos = i * 33;
      for (int j = 0; j < ssid.length(); j++) {
        EEPROM.write(pos + j, ssid[j]);
      }
      EEPROM.write(pos + ssid.length(), '\0');  // टर्मिनेटर जोड़ें
    }
    EEPROM.commit();  // बदलाव सेव करें
  }
}

void loop() {
  // कोई काम नहीं, बस स्कैन एक बार हुआ
}
