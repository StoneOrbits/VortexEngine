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
  static bool write(ByteStream &buffer);
  // read a serial buffer from storage
  static bool read(ByteStream &buffer);

  // the last save size (use STORAGE_SIZE For total space)
  static uint32_t lastSaveSize();

private:
  // the size of the last save
  static uint32_t m_lastSaveSize;
};

#endif
