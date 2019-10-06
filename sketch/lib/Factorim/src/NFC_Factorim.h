#ifndef NFC_Factorim_h
#define NFC_Factorim_h

#include "Arduino.h"
#include <Adafruit_PN532.h>

class NFC_Factorim : public Adafruit_PN532
{
private:
  String getHexString(const byte *data, const uint32_t numBytes);
  char *getNfcData(const byte *data1, const byte *data2, const byte *data3, const byte *data4);

public:
  NFC_Factorim(int din, int load, int clk, int num);
  void nfcInfo();
  char *readPassive();
};

#endif