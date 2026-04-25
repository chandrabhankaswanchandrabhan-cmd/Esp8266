#include <EEPROM.h>
#include <ESP8266WiFi.h>

#define MAX_NETWORKS 10
#define EEPROM_SIZE (MAX_NETWORKS * 33)

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  Serial.println("पढ़ा जा रहा है डेटा...");

  for (int i = 0; i < MAX_NETWORKS; i++) {
    int pos = i * 33;
    char ssid[33];
    int j = 0;
    while (EEPROM.read(pos + j) != '\0' && j < 32) {
      ssid[j] = EEPROM.read(pos + j);
      j++;
    }
    ssid[j] = '\0';  // टर्मिनेटर जोड़ें

    if (ssid[0] != '\0') {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(ssid);
      Serial.println();
    }
  }
}

void loop() {
  // कोई काम नहीं, बस डेटा पढ़ लिया
}
