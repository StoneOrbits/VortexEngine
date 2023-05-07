#include "Storage.h"

#include <string.h>
#include <stdlib.h>

#include "../VortexConfig.h"
#include "../Memory/Memory.h"
#include "../Serial/ByteStream.h"
#include "../Log/Log.h"

#ifdef VORTEX_ARDUINO
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#endif

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define storage_data ((uint8_t *)(0xF600))

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
#ifdef VORTEX_ARDUINO
  // Check size
  const uint16_t size = buffer.rawSize();
  if (!size || size > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  uint16_t pages = (size / PROGMEM_PAGE_SIZE) + 1;
  const uint8_t *buf = (const uint8_t *)buffer.rawData();
  for (uint16_t i = 0; i < pages; i++) {
    // don't read past the end of the input buffer
    uint16_t target = i * PROGMEM_PAGE_SIZE;
    uint16_t s = ((target + PROGMEM_PAGE_SIZE) > size) ? (size % PROGMEM_PAGE_SIZE) : PROGMEM_PAGE_SIZE;
    memcpy(storage_data + target, buf + target, s);
    // Erase + write the flash page
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x3);
    while (NVMCTRL.STATUS & 0x3);
  }
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
#endif // VORTEX_ARDUINO
  return (NVMCTRL.STATUS & 4) == 0;
}

// read a serial buffer from storage
bool Storage::read(ByteStream &buffer)
{
#ifdef VORTEX_ARDUINO
  uint32_t size = (*(uint32_t *)storage_data);
  if (size > STORAGE_SIZE || size < sizeof(ByteStream::RawBuffer) + 4) {
    return false;
  }
  // Read data from Flash
  if (!buffer.rawInit(storage_data, size + sizeof(ByteStream::RawBuffer))) {
    ERROR_LOGF("Could not initialize buffer with %u bytes", STORAGE_SIZE);
    return false;
  }
  // check crc immediately since we read into raw data copying the
  // array could be dangerous
  if (!buffer.checkCRC()) {
    buffer.clear();
    ERROR_LOG("Could not verify buffer");
    return false;
  }
  m_lastSaveSize = size;
#endif
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}

uint32_t Storage::lastSaveSize()
{
  return m_lastSaveSize;
}
