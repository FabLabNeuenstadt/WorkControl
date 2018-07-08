#include "mode.hpp"

#include <Arduino.h>
#include "const.hpp"
#include "conversion.hpp"
#include "rfid_helper.hpp"
#include <TimeLib.h>

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

bool nextDay(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd) {
  unsigned long cardTime = bufferToLong(buffer, TIME_OFFSET);
  if (day(cardTime) != day(currTime)) {
    longToBuffer(buffer, currTime, TIME_OFFSET);
    longToBuffer(buffer, 0, WORK_OFFSET);
    longToBuffer(buffer, 0, TIME_OFFSET);

    writeBlock(buffer, mfrc522);
    Serial.println(F("Next day. Resetted times"));
    
    return true;
  }
  return false;
}

void unset(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd) {
  byte switchMode = getCurrentMode();
  longToBuffer(buffer, currTime, TIME_OFFSET);
  longToBuffer(buffer, 0, WORK_OFFSET);
  longToBuffer(buffer, 0, TIME_OFFSET);

  lcd->clear();
  if (switchMode == WORK_MODE || switchMode == SHOW_MODE) {
    Serial.println(F("New card. Set to: WORK_MODE"));
    lcd->print(F("New card"));
    lcd->setCursor(0, 1);
    lcd->print(F("Set to: WORK_MODE"));

    buffer[0] = WORK_MODE;
  } else {
    Serial.println(F("New card. Set to: FREE_MODE"));
    lcd->print(F("New card")); 
    lcd->setCursor(0, 1);
    lcd->print(F("Set to: FREE_MODE"));

    buffer[0] = FREE_MODE;
  }

  writeBlock(buffer, mfrc522);
}

void work_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd) {
  nextDay(currTime, buffer, mfrc522, lcd);

  byte cardMode = buffer[0];
  unsigned long cardTime = bufferToLong(buffer, TIME_OFFSET);
  unsigned long workTime = bufferToLong(buffer, WORK_OFFSET);
  unsigned long freeTime = bufferToLong(buffer, FREE_OFFSET);
  unsigned long worked = (currTime - cardTime) + workTime;
  unsigned long freed = (currTime - cardTime) + freeTime;
  
  lcd->clear();
  Serial.print(F("Worked time: "));
  Serial.println(longToTime(worked));

  lcd->print(F("Worked: "));
  lcd->print(longToTime(worked));

  if (cardMode == FREE_MODE) {
    lcd->setCursor(0, 1);
    lcd->print(F("Set to: WORK_MODE"));

    buffer[0] = WORK_MODE;
    longToBuffer(buffer, currTime, TIME_OFFSET);
    longToBuffer(buffer, freed, FREE_OFFSET);
    writeBlock(buffer, mfrc522);  
  }
}

void free_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd) {
  nextDay(currTime, buffer, mfrc522, lcd);

  byte cardMode = buffer[0];
  unsigned long cardTime = bufferToLong(buffer, TIME_OFFSET);
  unsigned long workTime = bufferToLong(buffer, WORK_OFFSET);
  unsigned long freeTime = bufferToLong(buffer, FREE_OFFSET);
  unsigned long worked = (currTime - cardTime) + workTime;
  unsigned long freed = (currTime - cardTime) + freeTime;
  
  lcd->clear();
  Serial.print(F("Free time: "));
  Serial.println(longToTime(freed));

  lcd->print(F("Free: "));
  lcd->print(longToTime(freed));

  if (cardMode == WORK_MODE) {
    lcd->setCursor(0, 1);
    lcd->print(F("Set to: FREE_MODE"));

    buffer[0] = FREE_MODE;
    longToBuffer(buffer, currTime, TIME_OFFSET);
    longToBuffer(buffer, worked, WORK_OFFSET);
    writeBlock(buffer, mfrc522);  
  }
}

void show_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd)  {
  nextDay(currTime, buffer, mfrc522, lcd);
  
  unsigned long cardTime = bufferToLong(buffer, TIME_OFFSET);
  unsigned long workTime = bufferToLong(buffer, WORK_OFFSET);
  unsigned long freeTime = bufferToLong(buffer, FREE_OFFSET);
  unsigned long worked = (currTime - cardTime) + workTime;
  unsigned long freed = (currTime - cardTime) + freeTime;
  
  lcd->clear();
  Serial.print(F("Worked time: "));
  Serial.println(longToTime(worked));
  Serial.print(F("Free time: "));
  Serial.println(longToTime(freed));

  lcd->print(F("Worked: "));
  lcd->print(longToTime(worked));
  lcd->setCursor(0, 1);
  lcd->print(F("Free: "));
  lcd->print(longToTime(freed));
}

