#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "config.hpp"

WiFiUDP udp;
NTPClient ntp(udp, "europe.pool.ntp.org", 0,  interval);

void setup() {
  Serial.begin(9600);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  connect();

  ntp.begin();
}

void loop() {
  connect();
  ntp.update();
  unsigned long currentTime = ntp.getEpochTime();
}

void connect() {
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}

