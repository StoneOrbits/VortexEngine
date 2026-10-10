#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <inttypes.h>

#ifdef VORTEX_LIB
#include <string>
#endif

typedef struct ByteStream ByteStream;

// init storage
bool Storage_init(void);
void Storage_cleanup(void);

// store a serial buffer to storage
bool Storage_write(uint8_t slot, ByteStream *buffer);
// read a serial buffer from storage
bool Storage_read(uint8_t slot, ByteStream *buffer);

// the last save size (use STORAGE_SIZE For total space)
uint32_t Storage_lastSaveSize(void);

// write out a byte to the eeprom
void Storage_eepromWriteByte(uint16_t index, uint8_t in);
// read a byte from the eeprom
uint8_t Storage_eepromReadByte(uint16_t index);

#ifdef VORTEX_LIB
void Storage_setStorageFilename(const std::string &name);
std::string Storage_getStorageFilename(void);
#endif

#ifdef VORTEX_LIB
extern std::string Storage_m_storageFilename;
#endif

// the size of the last save
extern uint32_t Storage_m_lastSaveSize;

#endif