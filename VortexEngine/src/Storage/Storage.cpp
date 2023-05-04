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

#ifdef VORTEX_ARDUINO
//#define storage_data ((uint8_t *)(0xfc00))
//__attribute__((section(".storage"), used))
//static const uint8_t storage_data1[STORAGE_SIZE] = {0};

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
	//memset(storage_data1, 0, STORAGE_SIZE);
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
#ifdef VORTEX_ARDUINO
  // Check size
  const uint16_t size = buffer.rawSize();
  if (!size || size > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  // write out the buffer to storage
  for (size_t i = 0; i < buffer.rawSize(); ++i) {
    EEPROM.update(i, ((uint8_t *)buffer.rawData())[i]);
  }
  //uint16_t pages = (size / PROGMEM_PAGE_SIZE) + 1;
  //const uint8_t *buf = (const uint8_t *)buffer.rawData();
  //for (uint16_t i = 0; i < pages; i++) {
  //  // don't read past the end of the input buffer
  //  uint16_t target = i * PROGMEM_PAGE_SIZE;
  //  uint16_t s = ((target + PROGMEM_PAGE_SIZE) > size) ? (size % PROGMEM_PAGE_SIZE) : PROGMEM_PAGE_SIZE;
  //  // copy in the chunk of bytes
  //  memcpy(((uint8_t *)storage_data) + target, buf + target, s);
  //  // Erase + write the flash page
  //  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x3);
  //  while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm);
  //}
  //DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
#endif
  return true;
}

// read a serial buffer from storage
bool Storage::read(ByteStream &buffer)
{
#ifdef VORTEX_ARDUINO
  //uint32_t size = (*(uint32_t *)storage_data);
  //if (!size || size > STORAGE_SIZE) {
  //  return false;
  //}
  //// Read data from Flash
  //if (!buffer.rawInit(storage_data, size + sizeof(ByteStream::RawBuffer))) {
  //  ERROR_LOGF("Could not initialize buffer with %u bytes", STORAGE_SIZE);
  //  return false;
  //}
	//size += sizeof(ByteStream::RawBuffer);
	buffer.init(STORAGE_SIZE);
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
  //m_lastSaveSize = size;
#endif
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}

uint32_t Storage::lastSaveSize()
{
  return m_lastSaveSize;
}
