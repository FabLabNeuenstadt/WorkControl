#ifndef CONVERSION_HPP
#define CONVERSION_HPP
#include <Arduino.h>

//Both methods are low-endianess
unsigned long bufferToLong(byte* buffer, int start);
void longToBuffer(byte* buffer, unsigned long value, int start);
String longToTime(unsigned long convertTime);

#endif
