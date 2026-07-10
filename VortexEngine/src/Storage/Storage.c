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

#ifdef VORTEX_EMBEDDED
static uint8_t eepromReadByte(uint16_t index);
static void eepromWriteByte(uint16_t index, uint8_t in);
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
static char m_storageFilename[256];
#define STORAGE_FILENAME m_storageFilename
#else
#define STORAGE_FILENAME DEFAULT_STORAGE_FILENAME
#endif

#define FLASH_PAGE_SIZE 128
#define FLASH_STORAGE_SIZE (STORAGE_SIZE)
#define FLASH_STORAGE_SPACE ((volatile uint8_t *)(0x10000 - FLASH_STORAGE_SIZE))
#define HEADER_SIZE 15
#define STORAGE_HEADER_SIZE (HEADER_SIZE + 12)

static uint32_t m_lastSaveSize = 0;

bool Storage_init()
{
#ifdef VORTEX_LIB
  if (strlen(m_storageFilename) == 0 && Vortex_storageEnabled()) {
    strcpy(m_storageFilename, DEFAULT_STORAGE_FILENAME);
  }
#endif
  return true;
}

void Storage_cleanup()
{
}

bool Storage_write(uint8_t slot, ByteStream *buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex_storageEnabled()) {
    return true;
  }
#endif
  uint16_t size = ByteStream_rawSize(buffer);
  if (ByteStream_rawSize(buffer) > MAX_MODE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  if (slot >= NUM_MODE_SLOTS) {
    return false;
  }
  ByteStream_recalcCRC(buffer, false);
#ifdef VORTEX_EMBEDDED
  const uint8_t *buf = (const uint8_t *)ByteStream_rawData(buffer);
  if (slot < 4) {
    uint8_t eepSlot = 0;
    if (slot > 0) {
      eepSlot = STORAGE_HEADER_SIZE + (MAX_MODE_SIZE * (slot - 1));
    }
    uint8_t slotSize = (slot == 0) ? STORAGE_HEADER_SIZE : MAX_MODE_SIZE;
    for (uint8_t i = 0; i < slotSize; ++i) {
      uint8_t b = (i < size) ? buf[i] : 0x00;
      eepromWriteByte(eepSlot + i, b);
    }
  } else {
    uint16_t slotAddr = (uint16_t)FLASH_STORAGE_SPACE + (MAX_MODE_SIZE * (slot - 4));
    while (size > 0) {
      uint16_t pageStart = slotAddr & ~(FLASH_PAGE_SIZE - 1);
      uint16_t offset = slotAddr % FLASH_PAGE_SIZE;
      uint16_t space = FLASH_PAGE_SIZE - offset;
      uint16_t writeSize = (size < space) ? size : space;

      for (uint8_t i = 0; i < FLASH_PAGE_SIZE; ++i) {
        uint8_t value;
        if (i >= offset && i < offset + writeSize) {
          value = buf[i - offset];
        } else {
          value = *(volatile uint8_t *)(pageStart + i);
        }
        *(volatile uint8_t *)(pageStart + i) = value;
      }

      _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
      while (NVMCTRL.STATUS & (NVMCTRL_FBUSY_bm | NVMCTRL_EEBUSY_bm));

      slotAddr += writeSize;
      buf += writeSize;
      size -= writeSize;
    }
  }
#elif defined(_WIN32)
  HANDLE hFile = CreateFile(STORAGE_FILENAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    return false;
  }
  DWORD written = 0;
  DWORD offset = slot * MAX_MODE_SIZE;
  SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
  uint8_t modeBuffer[MAX_MODE_SIZE] = {0};
  memcpy(modeBuffer, ByteStream_rawData(buffer), size);
  if (!WriteFile(hFile, modeBuffer, MAX_MODE_SIZE, &written, NULL)) {
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
  uint8_t modeBuffer[MAX_MODE_SIZE] = {0};
  memcpy(modeBuffer, ByteStream_rawData(buffer), size);
  if (!fwrite(modeBuffer, sizeof(char), MAX_MODE_SIZE, f)) {
    return false;
  }
  fclose(f);
#endif
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
  return true;
}

bool Storage_read(uint8_t slot, ByteStream *buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex_storageEnabled()) {
    return false;
  }
#endif
  uint32_t size = MAX_MODE_SIZE;
  if (size > STORAGE_SIZE || size < sizeof(RawBuffer) + 4 || slot >= NUM_MODE_SLOTS) {
    return false;
  }
  if (!ByteStream_init(buffer, size, NULL)) {
    return false;
  }
#ifdef VORTEX_EMBEDDED
  uint8_t *buf = (uint8_t *)ByteStream_rawData(buffer);
  volatile uint8_t *src;
  if (slot == 0) {
    src = (volatile uint8_t *)MAPPED_EEPROM_START;
    size = STORAGE_HEADER_SIZE;
  } else if (slot < 4) {
    src = (volatile uint8_t *)MAPPED_EEPROM_START + STORAGE_HEADER_SIZE + (MAX_MODE_SIZE * (slot - 1));
  } else {
    src = (volatile uint8_t *)FLASH_STORAGE_SPACE + (MAX_MODE_SIZE * (slot - 4));
  }
  for (uint8_t i = 0; i < size; ++i) {
    buf[i] = src[i];
  }
#elif defined(_WIN32)
  HANDLE hFile = CreateFile(STORAGE_FILENAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    return false;
  }
  DWORD bytesRead = 0;
  DWORD offset = slot * MAX_MODE_SIZE;
  SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
  if (!ReadFile(hFile, ByteStream_rawData(buffer), MAX_MODE_SIZE, &bytesRead, NULL)) {
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
  if (!fread(ByteStream_rawData(buffer), sizeof(char), MAX_MODE_SIZE, f)) {
    return false;
  }
  fclose(f);
#endif
  ByteStream_sanity(buffer);
  if (!ByteStream_checkCRC(buffer)) {
    ByteStream_clear(buffer);
    ERROR_LOG("Could not verify buffer");
    return false;
  }
  m_lastSaveSize = (uint32_t)size;
  DEBUG_LOGF("Loaded savedata (Size: %u)", ByteStream_size(buffer));
  return true;
}

uint32_t Storage_lastSaveSize()
{
  return m_lastSaveSize;
}

#ifdef VORTEX_EMBEDDED
static void eepromWriteByte(uint16_t index, uint8_t in)
{
  if (eepromReadByte(index) == in) {
    return;
  }
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

static uint8_t eepromReadByte(uint16_t index)
{
  return *(volatile uint8_t *)(MAPPED_EEPROM_START + index);
}
#endif

#ifdef VORTEX_LIB
void Storage_setStorageFilename(const char *name)
{
  strncpy(m_storageFilename, name, sizeof(m_storageFilename) - 1);
  m_storageFilename[sizeof(m_storageFilename) - 1] = '\0';
}

const char *Storage_getStorageFilename()
{
  return m_storageFilename;
}
#endif
