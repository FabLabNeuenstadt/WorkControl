#ifndef CONVERSION_HPP
#define CONVERSION_HPP

//Both methods are low-endianess
unsigned long bufferToLong(byte* buffer, int start);
void longToBuffer(byte* buffer, unsigned long value, int start);

#endif
