#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>
#include "config.hpp"
#include "const.hpp"
#include "conversion.hpp"

WiFiUDP udp;
NTPClient ntp(udp, "europe.pool.ntp.org", 0,  interval);
MFRC522 mfrc522(SS_PIN, RST_PIN);

enum {
  UNSET = 0,
  WORK_MODE = 1,
  FREE_MODE = 2,
  SHOW_MODE = 3
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  digitalWrite(GREEN_PIN, HIGH);

  connect();

  ntp.begin();
}

void loop() {
  connect();
  ntp.update();
  unsigned long currentTime = ntp.getEpochTime();

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
#define RED_PIN D1
#define GREEN_PIN D2
#define RST_PIN D3
#define SS_PIN D8
#define WORK_PIN D0
#define FREE_PIN D3

#define ARRAY_OFFSET 1

  MFRC522::MIFARE_Key key;
  for (int i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  const byte block = 1;
  const byte trailor = 3;
  //1 byte + 1 long
  const byte bufferSize = 18;

  MFRC522::StatusCode status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailor,  &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return; 
  }

  byte buffer[bufferSize];
  byte bufferSizeCopy = bufferSize;
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(block, buffer, &bufferSizeCopy);
  if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
  }

  byte cardMode = buffer[0];
  byte switchMode = getCurrentMode();
  
  switch (cardMode) {
    case UNSET: {
      unset(switchMode);
    }break;
    case WORK_MODE: {
      work_mode(switchMode);
    }break;
    case FREE_MODE: {
      free_mode(switchMode);
    }break;
    default: {
      Serial.println(F("Invalid Card! Unknown mode"));
    }
  }  
}

void connect() {
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
}

byte getCurrentMode() {
  bool workState = digitalRead(WORK_PIN);
  bool freeState = digitalRead(FREE_PIN);

  if (workState) {
    return WORK_MODE;
  } else if (freeState) {
    return FREE_MODE;
  } else {
    return SHOW_MODE;
  }
}

