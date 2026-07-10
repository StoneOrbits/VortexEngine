#ifndef STORAGE_H
#define STORAGE_H

#include <inttypes.h>
#include <stdbool.h>

typedef struct ByteStream ByteStream;

bool Storage_init();
void Storage_cleanup();

bool Storage_write(uint8_t slot, ByteStream *buffer);
bool Storage_read(uint8_t slot, ByteStream *buffer);

uint32_t Storage_lastSaveSize();

#ifdef VORTEX_LIB
void Storage_setStorageFilename(const char *name);
const char *Storage_getStorageFilename();
#endif

#endif
