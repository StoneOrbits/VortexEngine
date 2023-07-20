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

#define STORAGE_FILENAME "FlashStorage.flash"

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
  uint16_t adr = (uint16_t)MAPPED_EEPROM_START + index;
  __asm__ __volatile__(
      "ldi r30, 0x00"     "\n\t"
      "ldi r31, 0x10"     "\n\t"
      "in r0, 0x3f"       "\n\t"
      "ldd r18, Z+2"      "\n\t"
      "andi r18, 3"       "\n\t"
      "brne .-6"          "\n\t"
      "cli"               "\n\t"
      "st X, %0"          "\n\t"
      "ldi %0, 0x9D"      "\n\t"
      "out 0x34, %0"      "\n\t"
      "ldi %0, 0x03"      "\n\t"
      "st Z, %0"          "\n\t"
      "out 0x3f, r0"      "\n"
      :"+d"(in)
      : "x"(adr)
      : "r30", "r31", "r18");
}
#endif

uint32_t Storage::m_lastSaveSize = 0;

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
    // success so the system thinks it all worked
    return true;
  }
#endif
  // Check size
  uint16_t size = buffer.rawSize();
  if (size < STORAGE_SIZE) {
    buffer.extend(STORAGE_SIZE - size);
  }
  size = buffer.rawSize();
  if (!size || size > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
#ifdef VORTEX_EMBEDDED
  const uint8_t *buf = (const uint8_t *)buffer.rawData();
  // start writing to eeprom
  for (uint16_t i = 0; i < STORAGE_SIZE; ++i) {
    if (*buf != *(uint8_t *)(MAPPED_EEPROM_START + i)) {
      eepromWriteByte(i, *buf);
    }
    buf++;
    size--;
    if (!size) {
      return true;
    }
  }
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
  return (NVMCTRL.STATUS & 4) == 0;
#elif defined(_MSC_VER)
  HANDLE hFile = CreateFile(STORAGE_FILENAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
#else
  FILE *f = fopen(STORAGE_FILENAME, "w");
  if (!f) {
    return false;
  }
  if (!fwrite(buffer.rawData(), sizeof(char), buffer.rawSize(), f)) {
    return false;
  }
  fclose(f);
#endif // VORTEX_EMBEDDED
  return true;
}

// read a serial buffer from storage
bool Storage::read(ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // return false here, but true in write because we don't want to return
    // an empty buffer after returning true
    return false;
  }
#endif
  uint32_t size = STORAGE_SIZE;
  if (size > STORAGE_SIZE || size < sizeof(ByteStream::RawBuffer) + 4) {
    return false;
  }
  if (!buffer.init(size)) {
    return false;
  }
#ifdef VORTEX_EMBEDDED
  // Read the data from EEPROM first
  uint8_t *pos = (uint8_t *)buffer.rawData();
  for (size_t i = 0; i < STORAGE_SIZE; ++i) {
    *pos = *(uint8_t *)(MAPPED_EEPROM_START + i);
    pos++;
    size--;
    if (!size) {
      break;
    }
  }
#elif defined(_MSC_VER)
  HANDLE hFile = CreateFile(STORAGE_FILENAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
  FILE *f = fopen(STORAGE_FILENAME, "r");
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
  m_lastSaveSize = size;
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}

uint32_t Storage::lastSaveSize()
{
  return m_lastSaveSize;
}


