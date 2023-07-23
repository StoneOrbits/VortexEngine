#ifndef STORAGE_H
#define STORAGE_H

#include <inttypes.h>

class ByteStream;

class Storage
{
  Storage();
public:

  // init storage
  static bool init();
  static void cleanup();

  // store a serial buffer to storage
  static bool write(ByteStream &buffer, uint16_t pos);
  // read a serial buffer from storage
  static bool read(ByteStream &buffer, uint16_t pos, uint16_t amt);

  // the last save size (use STORAGE_SIZE For total space)
  static uint32_t lastSaveSize();

private:
  // write out a single byte to the eeprom
  static void eepromWriteByte(uint16_t index, uint8_t in);
  static uint8_t eepromReadByte(uint16_t index);

  // the size of the last save
  static uint32_t m_lastSaveSize;
};

#endif
