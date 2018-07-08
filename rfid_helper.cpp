#include "rfid_helper.hpp"

#include <Arduino.h>
#include "const.hpp"

void writeBlock(byte* buffer, MFRC522* mfrc522) {
  MFRC522::StatusCode status = (MFRC522::StatusCode) mfrc522->MIFARE_Write(BLOCK, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522->GetStatusCodeName(status));
  }
  Serial.println();
}

