#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include "config.hpp"
#include "const.hpp"
#include "conversion.hpp"
#include "mode.hpp"

WiFiUDP udp;
NTPClient ntp(udp, "europe.pool.ntp.org", 0,  interval);
MFRC522 mfrc522(SS_PIN, MFRC522::UNUSED_PIN);
LiquidCrystal_PCF8574 lcd(0x27);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);
  Wire.begin(D3, D4);
  lcd.setBacklight(128);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  connect();

  ntp.begin();
}

void loop() {
  connect();
  ntp.update();
  unsigned long currTime = ntp.getEpochTime();

  if (!mfrc522.PICC_IsNewCardPresent()) 
    return;

  if (!mfrc522.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  if (         piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
  }

  MFRC522::MIFARE_Key key;
  for (int i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  const byte bufferSize = 18;

  MFRC522::StatusCode status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILOR,  &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return; 
  }

  byte buffer[bufferSize];
  byte bufferSizeCopy = bufferSize;
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(BLOCK, buffer, &bufferSizeCopy);
  if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
  }

  byte cardMode = buffer[0];  
  if (cardMode == UNSET) {
    unset(currTime, buffer, &mfrc522, &lcd);  
  }

  byte switchMode = getCurrentMode();
  switch (switchMode) {
    case WORK_MODE: {
      work_mode(currTime, buffer, &mfrc522, &lcd);
    }break;
    case FREE_MODE: {
      work_mode(currTime, buffer, &mfrc522, &lcd);
    }break;
    case SHOW_MODE: {
      work_mode(currTime, buffer, &mfrc522, &lcd);
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(1000);
}

void connect() {
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}
