#include "Storage.h"

#include <FlashStorage.h>
#include <stdlib.h>

#include "Memory.h"
#include "SerialBuffer.h"
#include "Log.h"

__attribute__((__aligned__(256)))
static const uint8_t _storagedata[(STORAGE_SIZE+255)/256*256] = { };

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
  // hopefully the flash storage is large enough
  SerialBuffer realBuffer(STORAGE_SIZE);
  realBuffer.serialize(buffer.size());
  realBuffer.append(buffer);
  storage.erase();
  storage.write(_storagedata, (void *)realBuffer.data(), realBuffer.size());
  DEBUGF("Wrote %u bytes to storage", realBuffer.capacity());
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
  storage.read(buffer.m_pBuffer);
#ifdef DEBUG_ALLOCATIONS
  DEBUGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
#endif
  buffer.m_size = *(uint32_t *)buffer.m_pBuffer;
  if (!buffer.m_size) {
    return false;
  }
  buffer.m_position += sizeof(uint32_t);
  DEBUGF("Read %u bytes from storage", buffer.m_size);
  buffer.shrink();
  return true;
}
