#include "Storage.h"

#include <string.h>
#include <stdlib.h>

#include "../VortexConfig.h"
#include "../Memory/Memory.h"
#include "../Serial/ByteStream.h"
#include "../Log/Log.h"
#include "../Leds/Leds.h"

#ifdef VORTEX_EMBEDDED
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#endif

#ifdef VORTEX_LIB
#include "../VortexLib/VortexLib.h"
#endif

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#include <stdio.h>
#endif

#ifdef VORTEX_EMBEDDED
// The first half of the data goes into the eeprom and then the rest goes into
// flash, the EEPROM is 256 and storage size is 512 so the flash storage is 256
#define FLASH_STORAGE_SIZE (STORAGE_SIZE)
// The position of the flash storage is right before the end of the flash, as long
// as the program leaves 256 bytes of space at the end of flash then this will fit
#define FLASH_STORAGE_SPACE ((uint8_t *)(0x10000 - FLASH_STORAGE_SIZE))

// write out the eeprom byte
void Storage::eepromWriteByte(uint16_t index, uint8_t in)
{
  //uint16_t adr = (uint16_t)MAPPED_EEPROM_START + index;
  //__asm__ __volatile__(
  //    "ldi r30, 0x00"     "\n\t"
  //    "ldi r31, 0x10"     "\n\t"
  //    "in r0, 0x3f"       "\n\t"
  //    "ldd r18, Z+2"      "\n\t"
  //    "andi r18, 3"       "\n\t"
  //    "brne .-6"          "\n\t"
  //    "cli"               "\n\t"
  //    "st X, %0"          "\n\t"
  //    "ldi %0, 0x9D"      "\n\t"
  //    "out 0x34, %0"      "\n\t"
  //    "ldi %0, 0x03"      "\n\t"
  //    "st Z, %0"          "\n\t"
  //    "out 0x3f, r0"      "\n"
  //    :"+d"(in)
  //    : "x"(adr)
  //    : "r30", "r31", "r18");
}

void flash_writePage(const uint8_t *data, uint16_t page)
{
  // Copy ram buffer to temporary flash buffer
  memcpy(FLASH_STORAGE_SPACE + (PROGMEM_PAGE_SIZE * page), data, PROGMEM_PAGE_SIZE);
  uint8_t sreg_save = SREG;  // save old SREG value
  asm volatile("cli");  // disable interrupts
  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
  while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm))
      ; // wait for flash and EEPROM not busy, just in case.
  SREG = sreg_save; // restore last interrupts state
}
#endif

#ifdef VORTEX_LIB
uint32_t Storage::m_lastSaveSize = 0;
#endif

Storage::Storage()
{
}

bool Storage::init()
{
  return true;
}

void Storage::cleanup()
{
}

// store a serial buffer to storage
bool Storage::write(ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // true? idk
    return false;
  }
#endif
#ifdef VORTEX_EMBEDDED
  // Check size
  uint16_t size = buffer.rawSize();
  if (!size || size > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  const uint8_t *buf = (const uint8_t *)buffer.rawData();
  //// start writing to eeprom
  //for (uint16_t i = 0; i < EEPROM_SIZE; ++i) {
  //  if (*buf != *(uint8_t *)(MAPPED_EEPROM_START + i)) {
  //    eepromWriteByte(i, *buf);
  //  }
  //  buf++;
  //  size--;
  //  if (!size) {
  //    return true;
  //  }
  //}
  // write all pages (most likely 4, 512 / 128 = 4)
  for (uint8_t i = 0; i < (STORAGE_SIZE / PROGMEM_PAGE_SIZE); ++ i) {
    flash_writePage(buf + (PROGMEM_PAGE_SIZE * i), i);
  }
  //uint16_t s = (size > PROGMEM_PAGE_SIZE) ? PROGMEM_PAGE_SIZE : size;
  //// write the rest to flash
  //memcpy(FLASH_STORAGE_SPACE, buf, s);
  //// Erase + write the flash page
  //_PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x3);
  //while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm))
  //if (size > PROGMEM_PAGE_SIZE) {
  //  s = size - PROGMEM_PAGE_SIZE;
  //  // write the rest to flash
  //  memcpy(FLASH_STORAGE_SPACE + PROGMEM_PAGE_SIZE, buf + PROGMEM_PAGE_SIZE, s);
  //  // Erase + write the flash page
  //  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x3);
  //  while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm))
  //}
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
  return (NVMCTRL.STATUS & 4) == 0;
  //return true;
#elif defined(_MSC_VER)
  HANDLE hFile = CreateFile("FlashStorage.flash", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (!hFile) {
    // error
    return false;
  }
  DWORD written = 0;
  if (!WriteFile(hFile, buffer.rawData(), buffer.rawSize(), &written, NULL)) {
    // error
    return false;
  }
  CloseHandle(hFile);
  return true;
#else
  FILE *f = fopen("FlashStorage.flash", "w");
  if (!f) {
    return false;
  }
  if (!fwrite(buffer.rawData(), sizeof(char), buffer.rawSize(), f)) {
    return false;
  }
  fclose(f);
  return true;
#endif // VORTEX_EMBEDDED
}

// read a serial buffer from storage
bool Storage::read(ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // true? idk
    return false;
  }
#endif
  uint32_t size = STORAGE_SIZE;
  if (size > STORAGE_SIZE || size < sizeof(ByteStream::RawBuffer) + 4) {
    return false;
  }
#ifdef VORTEX_EMBEDDED
  if (!buffer.init(STORAGE_SIZE)) {
    return false;
  }
  //return true;
  // Read the data from EEPROM first
  uint8_t *pos = (uint8_t *)buffer.rawData();
  //for (size_t i = 0; i < EEPROM_SIZE; ++i) {
  //  *pos = *(uint8_t *)(MAPPED_EEPROM_START + i);
  //  pos++;
  //  size--;
  //  if (!size) {
  //    break;
  //  }
  //}
  //if (size) {
    // Read the rest of data from Flash
  memcpy(pos, FLASH_STORAGE_SPACE, STORAGE_SIZE);
  //for (uint16_t i = 0; i < STORAGE_SIZE; ++i) {
  //  *pos = pgm_read_byte(FLASH_STORAGE_SPACE + i);
  //  pos++;
  //}
  //}
#elif defined(_MSC_VER)
  HANDLE hFile = CreateFile("FlashStorage.flash", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (!hFile) {
    // error
    return false;
  }
  DWORD bytesRead = 0;
  if (!ReadFile(hFile, buffer.rawData(), size, &bytesRead, NULL)) {
    // error
    return false;
  }
  CloseHandle(hFile);
#else
  FILE *f = fopen("FlashStorage.flash", "r");
  if (!f) {
    return false;
  }
  if (!fread(buffer.rawData(), sizeof(char), size, f)) {
    return false;
  }
  fclose(f);
#endif
  // check crc immediately since we read into raw data copying the
  // array could be dangerous
  if (!buffer.checkCRC()) {
    buffer.clear();
    ERROR_LOG("Could not verify buffer");
    return false;
  }
#ifdef VORTEX_LIB
  m_lastSaveSize = size;
#endif
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}

uint32_t Storage::lastSaveSize()
{
#ifdef VORTEX_LIB
  return m_lastSaveSize;
#else
  return 0;
#endif
}


