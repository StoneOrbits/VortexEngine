#include "Storage.h"

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <FlashStorage.h>

#include "../VortexConfig.h"
#include "../Memory/Memory.h"
#include "../Serial/ByteStream.h"
#include "../Log/Log.h"

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

// global for storage data I guess
uint8_t _storagedata[(STORAGE_SIZE+255)/256*256] = { };

uint32_t Storage::m_lastSaveSize = 0;

Storage::Storage()
{
}

bool Storage::init()
{
#ifdef VORTEX_LIB
#ifdef _MSC_VER
  DeleteFile("FlashStorage.flash");
#else
  unlink("FlashStorage.flash");
#endif
#endif
  DEBUG_LOGF("Total space: %u Engine size: %u Available space: %u",
    MAX_STORAGE_SPACE, ENGINE_SIZE, STORAGE_SIZE);
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
  memset(_storagedata, 0, sizeof(_storagedata));
  // write out the buffer to storage
  memcpy(_storagedata, buffer.rawData(), buffer.rawSize());
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
  memcpy(buffer.rawData(), _storagedata, STORAGE_SIZE);
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}

uint32_t Storage::totalSpace()
{
  return MAX_STORAGE_SPACE - ENGINE_SIZE;
}

uint32_t Storage::lastSaveSize()
{
  return m_lastSaveSize;
}
