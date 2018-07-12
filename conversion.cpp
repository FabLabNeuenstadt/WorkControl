#include "conversion.hpp"

unsigned long bufferToLong(byte* buffer, int start) {
  unsigned long result = (unsigned long) buffer[start] 
                                    |  (unsigned long) buffer[start + 1]  << 8
                                    |  (unsigned long) buffer[start + 2]  << 16
                                    |  (unsigned long) buffer[start + 3]  << 24;
  return result;
}       

void longToBuffer(byte* buffer, unsigned long value, int start) {
  buffer[start] = value;
  buffer[start + 1] = value >> 8;
  buffer[start + 2] = value >> 16;
  buffer[start + 3] = value >> 24; 
}

String longToTime(unsigned long convertTime) {
  convertTime /= 60;
  unsigned long hours = convertTime / 60;
  unsigned long minutes = convertTime % 60;
  String result = String(hours) + ":" + String(minutes);

  return result;
}

