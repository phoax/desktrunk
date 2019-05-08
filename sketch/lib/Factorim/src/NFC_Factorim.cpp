#include "Arduino.h"
#include "NFC_Factorim.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
 
NFC_Factorim::NFC_Factorim(int din, int load, int clk, int num): Adafruit_PN532(din, load, clk, num) {
 // do some stuff
}
 
/**
 * Display nfc info
 */
void NFC_Factorim::nfcInfo() {
 uint32_t versiondata = getFirmwareVersion();
  if (! versiondata) {
  Serial.print("Didn't find PN532 board");
    return;
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
}

/**
 * Convert hex to string
 * @param data Bytes data
 * @param numBytes Number of bytes
 * @returns string converted
 */
String NFC_Factorim::getHexString(const byte * data, const uint32_t numBytes)
{
  String text;
  uint32_t szPos;
  for (szPos=0; szPos < numBytes; szPos++)
  {
    if (data[szPos] <= 0x1F) {
      // text[0] = F(".");
    } else
      text += (char)data[szPos];
  }
  return text;
}

/**
 * Get nfc data
 * @param data1 First bytes of data 
 * @param data2 Second bytes of data
 * @param data3 Third bytes of data
 * @param data4 Fourth bytes of data
 * @returns concatenated data
 */
char * NFC_Factorim::getNfcData(const byte * data1, const byte * data2, const byte * data3, const byte * data4) {
  String message;
  message += getHexString(data1, 16);
  message += getHexString(data2, 16);
  message += getHexString(data3, 16);
  message += getHexString(data4, 16);
  return const_cast<char*>(message.c_str());;
}

/**
 * Read passive
 * @returns NFC data
 */
char * NFC_Factorim::readPassive() {
 char * data;
 boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 1000);
  
 if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
	  
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      // uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      // uint8_t keya[6] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 };
      uint8_t keya[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
	  
      // Start with block 4 (the first block of sector 1) since sector 0
      // contains the manufacturer data and it's probably better just
      // to leave it alone unless you know what you're doing
      success = mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
	  
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data1[16];
        uint8_t data2[16];
        uint8_t data3[16];
        uint8_t data4[16];
		
        // If you want to write something to block 4 to test with, uncomment
		    // the following line and this text should be read back in a minute
        //memcpy(data, (const uint8_t[]){ 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0 }, sizeof data);
        // success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = mifareclassic_ReadDataBlock(4, data1);
        success = mifareclassic_ReadDataBlock(5, data2);
        success = mifareclassic_ReadDataBlock(6, data3);
        success = mifareclassic_ReadDataBlock(7, data4);
		
        if (success)
        {
          // Data seems to have been read ... spit it out
          Serial.println("Reading Blocks");
          
          data = getNfcData(data1, data2, data3, data4);
          return data;
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
  }
  data = (char *)'\0';
  return data;
}