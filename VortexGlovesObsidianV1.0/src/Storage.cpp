#include "Storage.h"

#include <FlashStorage.h>
#include <string.h>
#include <stdlib.h>

#include "Memory.h"
#include "SerialBuffer.h"
#include "Log.h"

__attribute__((__aligned__(256)))
const uint8_t _storagedata[(STORAGE_SIZE+255)/256*256] = { };
FlashClass storage(_storagedata, STORAGE_SIZE);

Storage::Storage()
{
}

bool Storage::init()
{
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
  DEBUGF("Wrote %u bytes to storage", buffer.capacity());
#ifdef DEBUG_ALLOCATIONS
  DEBUGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
#endif
  return true;
}

// read a serial buffer from storage
bool Storage::read(SerialBuffer &buffer)
{
#ifdef DEBUG_ALLOCATIONS
  DEBUGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
#endif
  buffer.init(STORAGE_SIZE);
#ifdef DEBUG_ALLOCATIONS
  DEBUGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
#endif
  storage.read(buffer.rawData());
#ifdef DEBUG_ALLOCATIONS
  DEBUGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
#endif
  if (!buffer.size()) {
    DEBUG("Read null from storage");
    return false;
  }
  DEBUGF("Read %u bytes from storage", buffer.size());
  buffer.shrink();
  return true;
}
