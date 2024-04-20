#include "Storage.h"

#include <string.h>
#include <stdlib.h>

#include "../VortexConfig.h"
#include "../Memory/Memory.h"
#include "../Serial/ByteStream.h"
#include "../Log/Log.h"

#ifdef VORTEX_EMBEDDED
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#endif

#ifdef VORTEX_LIB
#include "../VortexLib/VortexLib.h"
#endif

#ifndef VORTEX_EMBEDDED
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <stdio.h>
#endif
#endif

#define DEFAULT_STORAGE_FILENAME "FlashStorage.flash"

#ifdef VORTEX_LIB
std::string Storage::m_storageFilename;
#define STORAGE_FILENAME m_storageFilename.c_str()
#else
#define STORAGE_FILENAME DEFAULT_STORAGE_FILENAME
#endif

// The first half of the data goes into the eeprom and then the rest goes into
// flash, the EEPROM is 256 and storage size is 512 so the flash storage is 256
#define FLASH_STORAGE_SIZE (STORAGE_SIZE)
// The position of the flash storage is right before the end of the flash, as long
// as the program leaves 256 bytes of space at the end of flash then this will fit
#define FLASH_STORAGE_SPACE ((volatile uint8_t *)(0x10000 - FLASH_STORAGE_SIZE))

uint32_t Storage::m_lastSaveSize = 0;

Storage::Storage()
{
}

bool Storage::init()
{
#ifdef VORTEX_LIB
  if (!m_storageFilename.length() && Vortex::storageEnabled()) {
    m_storageFilename = DEFAULT_STORAGE_FILENAME;
  }
#endif
  return true;
}

void Storage::cleanup()
{
}

// store a serial buffer to storage
bool Storage::write(uint8_t slot, ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // success so the system thinks it all worked
    return true;
  }
#endif
  // check size
  uint16_t size = buffer.rawSize();
  if (buffer.rawSize() > MAX_MODE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  if (slot >= NUM_MODE_SLOTS) {
    return false;
  }
  // just in case
  buffer.recalcCRC();
#ifdef VORTEX_EMBEDDED
  const uint8_t *buf = (const uint8_t *)buffer.rawData();
  if (slot > 5) {
    uint8_t eepSlot = 128 * (slot - 6);
    for (uint8_t i = 0; i < 128; ++i) {
      uint8_t b = (i < size) ? buf[i] : 0x00;
      if (eepromReadByte(i + eepSlot) != b) {
        eepromWriteByte(i + eepSlot, b);
      }
    }
  } else {
    volatile uint8_t *dest = (volatile uint8_t *)FLASH_STORAGE_SPACE + (128 * slot);
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);
    while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm));
    // implement device storage here
    for (uint8_t i = 0; i < 128; ++i) {
      dest[i] = (i < size) ? buf[i] : 0x00;
    }
    // Erase + write the flash page
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
    // wait for flash and EEPROM not busy, just in case.
    while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm));
  }
#elif defined(_WIN32)
  HANDLE hFile = CreateFile(STORAGE_FILENAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    // error
    return false;
  }
  DWORD written = 0;
  DWORD offset = slot * MAX_MODE_SIZE;
  SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
  if (!WriteFile(hFile, buffer.rawData(), MAX_MODE_SIZE, &written, NULL)) {
    // error
    return false;
  }
  CloseHandle(hFile);
#else
  FILE *f = fopen(STORAGE_FILENAME, "w");
  if (!f) {
    return false;
  }
  long offset = slot * MAX_MODE_SIZE;
  fseek(f, offset, SEEK_SET);
  if (!fwrite(buffer.rawData(), sizeof(char), MAX_MODE_SIZE, f)) {
    return false;
  }
  fclose(f);
#endif // VORTEX_EMBEDDED
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
  return true;
}

// read a serial buffer from storage
bool Storage::read(uint8_t slot, ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // return false here, but true in write because we don't want to return
    // an empty buffer after returning true
    return false;
  }
#endif
  uint32_t size = MAX_MODE_SIZE;
  if (size > STORAGE_SIZE || size < sizeof(ByteStream::RawBuffer) + 4 || slot >= NUM_MODE_SLOTS) {
    return false;
  }
  if (!buffer.init(size)) {
    return false;
  }
#ifdef VORTEX_EMBEDDED
  uint8_t *buf = (uint8_t *)buffer.rawData();
  volatile uint8_t *src;
  if (slot > 5) {
    src = (volatile uint8_t *)(MAPPED_EEPROM_START + (128 * (slot - 6)));
  } else {
    src = (volatile uint8_t *)FLASH_STORAGE_SPACE + (128 * slot);
  }
  // implement device storage here
  for (uint8_t i = 0; i < size; ++i) {
    buf[i] = src[i];
  }
#elif defined(_WIN32)
  HANDLE hFile = CreateFile(STORAGE_FILENAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    // error
    return false;
  }
  DWORD bytesRead = 0;
  DWORD offset = slot * MAX_MODE_SIZE;
  SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
  if (!ReadFile(hFile, buffer.rawData(), MAX_MODE_SIZE, &bytesRead, NULL)) {
    // error
    return false;
  }
  CloseHandle(hFile);
#else
  FILE *f = fopen(STORAGE_FILENAME, "r");
  if (!f) {
    return false;
  }
  long offset = slot * MAX_MODE_SIZE;
  fseek(f, offset, SEEK_SET);
  if (!fread(buffer.rawData(), sizeof(char), MAX_MODE_SIZE, f)) {
    return false;
  }
  fclose(f);
#endif
  // ensure the internal buffer is sane after reading it out, this
  // prevents segfaults if the internal size reports larger than capacity
  buffer.sanity();
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

#ifdef VORTEX_EMBEDDED
// write out the eeprom byte
void Storage::eepromWriteByte(uint16_t index, uint8_t in)
{
  // The first two pages of the data goes into the eeprom and then the last page goes
  // into the USERROW which is located at 0x1300
  uint16_t adr = MAPPED_EEPROM_START + (index);
  __asm__ __volatile__(
    "ldi r30, 0x00"     "\n\t"
    "ldi r31, 0x10"     "\n\t"
    "ldd r18, Z+2"      "\n\t"
    "andi r18, 3"       "\n\t"
    "brne .-6"          "\n\t"
    "st X, %0"          "\n\t"
    "ldi %0, 0x9D"      "\n\t"
    "out 0x34, %0"      "\n\t"
    "ldi %0, 0x03"      "\n\t"
    "st Z, %0"          "\n\t"
    :"+d"(in)
    : "x"(adr)
    : "r30", "r31", "r18");

  while (!(NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm));
}

uint8_t Storage::eepromReadByte(uint16_t index)
{
  return *(volatile uint8_t *)(MAPPED_EEPROM_START + index);
}
#endif
