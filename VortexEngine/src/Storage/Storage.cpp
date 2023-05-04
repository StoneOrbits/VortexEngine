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

__attribute__((section(".storage")))
static const uint8_t storage_data[STORAGE_SIZE] = {0};

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

#include "../Leds/Leds.h"
#include <Arduino.h>
void ledVerification(uint8_t val, uint8_t d)
{
#ifdef VORTEX_ARDUINO
  uint8_t updateTime = 30;
  // check for write errors
  if (d == 0) {
    // blank = data to write is 0 so cannot test
    Leds::setAll(RGB_OFF);
    updateTime = 0;
  } else {
    if (val == d) {
      // green = flash data is good
      Leds::setAll(RGBColor(0, 255, 0));
    } else {
      if (val == 0) {
        // red = the flash data is still 0
        Leds::setAll(RGBColor(255, 0, 0));
      } else {
        // yellow = the flash data is't 0 but not right
        Leds::setAll(RGBColor(150, 150, 0));
      }
    }
  }
  Leds::update();
  delay(updateTime);
#endif
}

// store a serial buffer to storage
bool Storage::write(ByteStream &buffer)
{
  // Check size
  const uint16_t size = buffer.rawSize();
  if (!size || size > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  const uint8_t *buf = (const uint8_t *)buffer.rawData();
  for (uint16_t i = 0; i < size; i += PROGMEM_PAGE_SIZE) {
    // don't read past the end of the input buffer
    uint16_t s = ((i + PROGMEM_PAGE_SIZE) >= size) ? (size % PROGMEM_PAGE_SIZE) : PROGMEM_PAGE_SIZE;
    // copy in the chunk of bytes
    memcpy(((uint8_t *)storage_data) + i, buf + i, s);
    // Erase + write the flash page
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x3);
    while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm);
  }
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
  return true;
}

// read a serial buffer from storage
bool Storage::read(ByteStream &buffer)
{
  uint32_t size = (*(uint32_t *)storage_data) + sizeof(ByteStream::RawBuffer);
  if (!size || size > STORAGE_SIZE) {
    return false;
  }
#ifdef VORTEX_ARDUINO
  // Read data from Flash
  if (!buffer.rawInit(storage_data, size)) {
    ERROR_LOGF("Could not initialize buffer with %u bytes", STORAGE_SIZE);
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
