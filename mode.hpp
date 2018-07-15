#ifndef MODE_HPP
#define MODE_HPP
#include <MFRC522.h>
#include <LiquidCrystal_PCF8574.h>

enum {
  UNSET = 0,
  WORK_MODE = 1,
  FREE_MODE = 2,
  SHOW_MODE = 3
};

extern unsigned long lcdStarted;

byte getCurrentMode();

bool nextDay(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd);
void unset(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd);

void work_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd);
void free_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd);
void show_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, LiquidCrystal_PCF8574* lcd);

#endif
