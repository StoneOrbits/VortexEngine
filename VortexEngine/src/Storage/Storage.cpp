#include "Storage.h"

#include <FlashStorage.h>
#include <string.h>
#include <stdlib.h>

#include "../Memory/Memory.h"
#include "../Serial/ByteStream.h"
#include "../Log/Log.h"

#ifdef TEST_FRAMEWORK
#ifndef LINUX_FRAMEWORK
#include <Windows.h>
#endif
#endif

__attribute__((__aligned__(256)))
#ifdef TEST_FRAMEWORK
uint8_t _storagedata[(STORAGE_SIZE+255)/256*256] = { };
#else
const uint8_t _storagedata[(STORAGE_SIZE+255)/256*256] = { };
#endif
FlashClass storage(_storagedata, STORAGE_SIZE);

Storage::Storage()
{
}

bool Storage::init()
{
#ifdef TEST_FRAMEWORK
  DeleteFile("FlashStorage.flash");
#endif
  return true;
}

void Storage::cleanup()
{
}

// store a serial buffer to storage
bool Storage::write(ByteStream &buffer)
{
  // check size
  if (buffer.size() > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  // clear existing storage, this is necessary even if it's empty
  storage.erase();
  // write out the buffer to storage
  storage.write(_storagedata, buffer.rawData(), buffer.rawSize());
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", buffer.size(), STORAGE_SIZE);
  return true;
}

// read a serial buffer from storage
bool Storage::read(ByteStream &buffer)
{
  // init storage buffer with max storaeg size so we can read into it
  if (!buffer.init(STORAGE_SIZE)) {
    ERROR_LOGF("Could not initialize buffer with %u bytes", STORAGE_SIZE);
    return false;
  }
  // read directly into the rawdata of the byte array, this will
  // include the crc, size, flags and entire buffer of data
  storage.read(buffer.rawData());
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}
