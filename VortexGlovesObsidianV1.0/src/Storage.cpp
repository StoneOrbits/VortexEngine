#include "Storage.h"

#include <FlashStorage.h>
#include <string.h>
#include <stdlib.h>

#include "Memory.h"
#include "SerialBuffer.h"
#include "Log.h"

#ifdef TEST_FRAMEWORK
#ifndef LINUX_FRAMEWORK
#include <Windows.h>
#endif
#endif

__attribute__((__aligned__(256)))
#ifdef TEST_FRAMEWORK
uint8_t _storagedata[(STORAGE_SIZE+255)/256*256] = { };
#else
const uint8_t _storagedata[(STORAGE_SIZE+255)/256*256] = { };
#endif
FlashClass storage(_storagedata, STORAGE_SIZE);

#ifdef TEST_FRAMEWORK
uint32_t written_size = 0;
#define WRITTEN_SIZE written_size
#else
#define WRITTEN_SIZE *(uint32_t *)_storagedata;
#endif

Storage::Storage()
{
}

bool Storage::init()
{
#ifdef TEST_FRAMEWORK
#ifndef LINUX_FRAMEWORK
  DeleteFile("FlashStorage.flash");
#endif
#endif
  return true;
}

// store a serial buffer to storage
bool Storage::write(SerialBuffer &buffer)
{
  if (buffer.size() > STORAGE_SIZE) {
    DEBUG("ERROR buffer too big");
    return false;
  }
#ifdef DEBUG_ALLOCATIONS
  DEBUGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
#endif
  storage.erase();
  storage.write(_storagedata, buffer.rawData(), buffer.rawSize());
  DEBUGF("Wrote %u bytes to storage (max: %u)", buffer.size(), STORAGE_SIZE);
#ifdef DEBUG_ALLOCATIONS
  DEBUGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
#endif
#ifdef TEST_FRAMEWORK
  written_size = buffer.size();
#endif
  return true;
}

// read a serial buffer from storage
bool Storage::read(SerialBuffer &buffer)
{
  if (!buffer.init(STORAGE_SIZE)) {
    return false;
  }
  storage.read(buffer.rawData());
  if (!buffer.size()) {
    DEBUG("Read null from storage");
    return false;
  }
  DEBUGF("Read %u bytes from storage", buffer.size());
  buffer.shrink();
  return true;
}
