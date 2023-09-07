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
bool Storage::write(uint16_t slot, ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // success so the system thinks it all worked
    return true;
  }
#endif
  bool first = (slot == 0);
  // Check size
  uint16_t size = first ? (sizeof(ByteStream::RawBuffer) + 5) : MAX_MODE_SIZE;
  if (!size || size > STORAGE_SIZE) {
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
  // start writing to eeprom
  uint16_t start = first ? 0 : (((slot - 1) * MAX_MODE_SIZE) + 4);
  for (uint16_t i = start; i < size; ++i) {
    if (buf[i] != eepromReadByte(i)) {
      eepromWriteByte(i, buf[i]);
    }
  }
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
  if ((NVMCTRL.STATUS & 4) != 0) {
    return false;
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
  return true;
}

// read a serial buffer from storage
bool Storage::read(uint16_t slot, ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // return false here, but true in write because we don't want to return
    // an empty buffer after returning true
    return false;
  }
#endif
  bool first = (slot == 0);
  uint32_t size = first ? (sizeof(ByteStream::RawBuffer) + 5) : MAX_MODE_SIZE;
  if (size > STORAGE_SIZE || size < sizeof(ByteStream::RawBuffer) + 4 || slot >= NUM_MODE_SLOTS) {
    return false;
  }
  if (!buffer.init(size)) {
    return false;
  }
#ifdef VORTEX_EMBEDDED
  // Read the data from EEPROM first
  uint8_t *pos = (uint8_t *)buffer.rawData();
  uint16_t start = first ? 0 : (((slot - 1) * MAX_MODE_SIZE) + 4);
  for (uint16_t i = start; i < size; ++i) {
    pos[i] = eepromReadByte(i);
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
  uint16_t adr;
  // The first two pages of the data goes into the eeprom and then the last page goes
  // into the USERROW which is located at 0x1300
  if (index > 255) {
    adr = 0x1300 + (index & 0xFF);
  } else {
    adr = MAPPED_EEPROM_START + index;
  }
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
}

uint8_t Storage::eepromReadByte(uint16_t index)
{
  if (index > 255) {
    // USERROW start
    return *(uint8_t *)(0x1300 + (index & 0xFF));
  }
  return *(uint8_t *)(MAPPED_EEPROM_START + index);
}
#endif
