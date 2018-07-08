#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MFRC522.h>
#include "config.hpp"

#define RED_PIN D1
#define GREEN_PIN D2
#define RST_PIN D3
#define SS_PIN D8
#define WORK_PIN D0
#define FREE_PIN D3

#define ARRAY_OFFSET 1

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

  byte mode = buffer[0];
  unsigned long timestamp = bufferToLong(buffer, ARRAY_OFFSET);

  
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

//Both conversion methods are for low-endianess
unsigned long bufferToLong(byte* buffer, int start) {
  unsigned long result = (unsigned long) buffer[start] 
                                    |  (unsigned long) buffer[start + 1]  << 8
                                    |  (unsigned long) buffer[start + 2]  << 16
                                    |  (unsigned long) buffer[start + 3]  << 24;
  return result;
}       

void longToBuffer(unsigned long value, byte* buffer, int start) {
  buffer[start] = value;
  buffer[start + 1] = value >> 8;
  buffer[start + 2] = value >> 16;
  buffer[start + 3] = value >> 24; 
}

