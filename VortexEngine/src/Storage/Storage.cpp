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

#define storage_data ((uint8_t *)0x7c00)

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

/**
 * \brief Erase a page in flash
 *
 * \param[in] flash_adr The byte-address in flash to erase. Must point to start-of-page.
 *
 * \return Status of the operation
 */
void erase_flash_page(uint16_t flash_adr)
{
	// Create a pointer in unified memory map to the page to erase
	uint8_t *data_space = (uint8_t *)(flash_adr);
	// Perform a dummy write to this address to update the address register in NVMCTL
	*data_space = 0;
	// Erase the flash page
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASE_gc);
	while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm);
}

/**
 * \brief Write a page in flash.
 *
 * \param[in] flash_adr The byte-address of the flash page to write to. Must point to start-of-page.
 * \param[in] data The data to write to the flash page
 *
 * \return Status of the operation
 */
void write_flash_page(uint16_t flash_adr, uint8_t *data)
{

	// Create a pointer in unified memory map to the page to write
	uint8_t *data_space = (uint8_t *)(flash_adr);
	// Write data to the page buffer
	memcpy(data_space, data, PROGMEM_PAGE_SIZE);
	// Write the flash page
	_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEWRITE_gc);
	while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm);
}

// store a serial buffer to storage
bool Storage::write(ByteStream &buffer)
{
  // Check size
  if (buffer.rawSize() > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  const uint8_t *buf = (const uint8_t *)buffer.rawData();
  const uint8_t *end = buf + buffer.rawSize();
  for (uint16_t i = 0; i < buffer.rawSize(); i += PROGMEM_PAGE_SIZE) {
		erase_flash_page(storage_data + i);
		write_flash_page(storage_data + i, buf + i);
  }

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
  // Read data from Flash
  for (uint32_t i = 0; i < STORAGE_SIZE; i++) {
    ((uint8_t *)buffer.rawData())[i] = storage_data[i];
  }
#endif
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}

uint32_t Storage::lastSaveSize()
{
  return m_lastSaveSize;
}
