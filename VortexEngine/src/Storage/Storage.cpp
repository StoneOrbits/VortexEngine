#include "Storage.h"

#include <string.h>
#include <stdlib.h>

#include "../VortexConfig.h"
#include "../Memory/Memory.h"
#include "../Serial/ByteStream.h"
#include "../Log/Log.h"

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

// only arduino needs const I guess?
static const uint8_t _storagedata[STORAGE_SIZE] = { };

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
  if (buffer.rawSize() > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, totalSpace());
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
  DEBUG_LOGF("Loaded savedata (Size: %u)", m_lastSaveSize);
  return true;
}

uint32_t Storage::lastSaveSize()
{
  return m_lastSaveSize;
}
