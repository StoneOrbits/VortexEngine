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

#ifdef VORTEX_ARDUINO
#include <EEPROM.h>
#endif

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
#ifdef VORTEX_ARDUINO
  // write out the buffer to storage
  for (size_t i = 0; i < buffer.rawSize(); ++i) {
    EEPROM.update(i, ((uint8_t *)buffer.rawData())[i]);
  }
#endif
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
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
#ifdef VORTEX_ARDUINO
  // Read the data from EEPROM into the buffer
  for (size_t i = 0; i < STORAGE_SIZE; ++i) {
    ((uint8_t *)buffer.rawData())[i] = EEPROM.read(i);
  }
  // check crc immediately since we read into raw data copying the
  // array could be dangerous
  if (!buffer.checkCRC()) {
    ERROR_LOG("Could not verify buffer");
    return false;
  }
#endif
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}

uint32_t Storage::lastSaveSize()
{
  return m_lastSaveSize;
}
