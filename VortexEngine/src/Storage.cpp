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

void Storage::cleanup()
{
}

// store a serial buffer to storage
bool Storage::write(SerialBuffer &buffer)
{
  if (buffer.size() > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big");
    return false;
  }
  if (!buffer.compress()) {
    // don't write if we can't compress
    ERROR_LOG("Did not compress storage");
    return false;
  }
  storage.erase();
  storage.write(_storagedata, buffer.rawData(), buffer.rawSize());
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", buffer.size(), STORAGE_SIZE);
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
    DEBUG_LOG("Read null from storage");
    return false;
  }
  if (!buffer.decompress()) {
    DEBUG_LOG("Failed to decompress buffer loaded from storage");
    return false;
  }
  if (!buffer.shrink()) {
    DEBUG_LOG("Failed to shrink buffer loaded from storage");
    return false;
  }
  DEBUG_LOGF("Loaded %u bytes from storage", buffer.size());
  return true;
}
