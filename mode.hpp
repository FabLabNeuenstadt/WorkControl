#ifndef MODE_HPP
#define MODE_HPP
#include <MFRC522.h>
#include <Adafruit_LiquidCrystal.h>

enum {
  UNSET = 0,
  WORK_MODE = 1,
  FREE_MODE = 2,
  SHOW_MODE = 3
};

byte getCurrentMode();

bool nextDay(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd);
void unset(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd);

void work_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd);
void free_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd);
void show_mode(unsigned long currTime, byte* buffer, MFRC522* mfrc522, Adafruit_LiquidCrystal* lcd);

#endif
