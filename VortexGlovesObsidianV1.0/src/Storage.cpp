#include "Storage.h"

#include <FlashStorage.h>

#include "SerialBuffer.h"
#include "Log.h"

Flash(storage, STORAGE_SIZE);

Storage::Storage()
{
}

Storage::~Storage()
{
}

// store a serial buffer to storage
bool Storage::write(SerialBuffer &buffer)
{
  if (buffer.size() > STORAGE_SIZE) {
    DEBUG("ERROR buffer too big");
    return false;
  }
  // hopefully the flash storage is large enough
  SerialBuffer realBuffer(STORAGE_SIZE);
  realBuffer.serialize(buffer.size());
  realBuffer.append(buffer);
  storage.erase();
  storage.write(realBuffer.m_pBuffer);
  DEBUG("Wrote storage");
  return true;
}

// read a serial buffer from storage
bool Storage::read(SerialBuffer &buffer)
{
  buffer.init(STORAGE_SIZE);
  storage.read(buffer.m_pBuffer);
  buffer.m_size = *(uint32_t *)buffer.m_pBuffer;
  if (!buffer.m_size) {
    return false;
  }
  buffer.m_position += sizeof(uint32_t);
  DEBUG("Read storage");
  return true;
}
