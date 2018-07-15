#include "mode.hpp"

#include <Arduino.h>
#include "const.hpp"
#include "conversion.hpp"
#include "rfid_helper.hpp"
#include <TimeLib.h>

unsigned long lcdStarted = 0;

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

bool nextDay(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd) {
  unsigned long cardTime = bufferToLong(buffer, TIME_OFFSET);
  if (day(cardTime) != day(currTime)) {
    longToBuffer(buffer, currTime, TIME_OFFSET);
    longToBuffer(buffer, 0, WORK_OFFSET);
    longToBuffer(buffer, 0, FREE_OFFSET);

    writeBlock(buffer, mfrc522);
    Serial.println(F("Next day. Resetted times"));
    
    return true;
  }
  return false;
}

void unset(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd) {
  byte switchMode = getCurrentMode();
  longToBuffer(buffer, currTime, TIME_OFFSET);
  longToBuffer(buffer, 0, WORK_OFFSET);
  longToBuffer(buffer, 0, FREE_OFFSET);
  
  lcd->clear();
  if (switchMode == WORK_MODE || switchMode == SHOW_MODE) {
    Serial.println(F("New card. Set to:WORK_MODE"));
    lcd->print(F("New card"));
    lcd->setCursor(0, 1);
    lcd->print(F("Set to:WORK_MODE"));
    lcdStarted = millis();

    buffer[0] = WORK_MODE;
  } else {
    Serial.println(F("New card. Set to:FREE_MODE"));
    lcd->print(F("New card")); 
    lcd->setCursor(0, 1);
    lcd->print(F("Set to:FREE_MODE"));
    lcdStarted = millis();

    buffer[0] = FREE_MODE;
  }

  writeBlock(buffer, mfrc522);
}

void work_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd) {
  nextDay(currTime, buffer, mfrc522, lcd);

  unsigned long cardTime = bufferToLong(buffer, TIME_OFFSET);
  unsigned long workTime = bufferToLong(buffer, WORK_OFFSET);
  unsigned long freeTime = bufferToLong(buffer, FREE_OFFSET);

  if (buffer[0] == WORK_MODE) {
    workTime += (currTime - cardTime);
  } else if (buffer[0] == FREE_MODE) {
    freeTime += (currTime - cardTime);
  }
  
  lcd->clear();
  Serial.print(F("Worked time:"));
  Serial.println(longToTime(workTime));

  lcd->print(F("Worked:"));
  lcd->print(longToTime(workTime));

  if (buffer[0] != WORK_MODE) {
    lcd->setCursor(0, 1);
    lcd->print(F("Set to:WORK_MODE"));

    buffer[0] = WORK_MODE;
    longToBuffer(buffer, currTime, TIME_OFFSET);
    longToBuffer(buffer, freeTime, FREE_OFFSET);
    writeBlock(buffer, mfrc522);  
  }

  lcdStarted = millis();
}

void free_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd) {
  nextDay(currTime, buffer, mfrc522, lcd);

  unsigned long cardTime = bufferToLong(buffer, TIME_OFFSET);
  unsigned long workTime = bufferToLong(buffer, WORK_OFFSET);
  unsigned long freeTime = bufferToLong(buffer, FREE_OFFSET);
  
  if (buffer[0] == WORK_MODE) {
    workTime += (currTime - cardTime);
  } else if (buffer[0] == FREE_MODE) {
    freeTime += (currTime - cardTime);
  }
  
  lcd->clear();
  Serial.print(F("Free time:"));
  Serial.println(longToTime(freeTime));

  lcd->print(F("Free:"));
  lcd->print(longToTime(freeTime));

  if (buffer[0] != FREE_MODE) {
    lcd->setCursor(0, 1);
    lcd->print(F("Set to:FREE_MODE"));

    buffer[0] = FREE_MODE;
    longToBuffer(buffer, currTime, TIME_OFFSET);
    longToBuffer(buffer, workTime, WORK_OFFSET);
    writeBlock(buffer, mfrc522);  
  }

  lcdStarted = millis();
}

void show_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd)  {
  nextDay(currTime, buffer, mfrc522, lcd);
  
  unsigned long cardTime = bufferToLong(buffer, TIME_OFFSET);
  unsigned long workTime = bufferToLong(buffer, WORK_OFFSET);
  unsigned long freeTime = bufferToLong(buffer, FREE_OFFSET);
  
  if (buffer[0] == WORK_MODE) {
    workTime += (currTime - cardTime);
  } else if (buffer[0] == FREE_MODE) {
    freeTime += (currTime - cardTime);
  }
  
  lcd->clear();
  Serial.print(F("Worked time:"));
  Serial.println(longToTime(workTime));
  Serial.print(F("Free time:"));
  Serial.println(longToTime(freeTime));

  lcd->print(F("Worked:"));
  lcd->print(longToTime(workTime));
  lcd->setCursor(0, 1);
  lcd->print(F("Free:"));
  lcd->print(longToTime(freeTime));

  lcdStarted = millis();
}

