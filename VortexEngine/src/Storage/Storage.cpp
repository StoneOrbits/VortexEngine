#include "Storage.h"

#include <FlashStorage.h>
#include <string.h>
#include <stdlib.h>

#include "../Memory/Memory.h"
#include "../Serial/ByteStream.h"
#include "../Log/Log.h"

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
  //DeleteFile("FlashStorage.flash");
#endif
#endif
  return true;
}

void Storage::cleanup()
{
}

// store a serial buffer to storage
bool Storage::write(ByteStream &buffer)
{
  // insert the version number at end of buffer
  buffer.serialize((uint8_t)VORTEX_VERSION_MAJOR);
  buffer.serialize((uint8_t)VORTEX_VERSION_MINOR);
  // compress
  if (!buffer.compress()) {
    // don't write if we can't compress
    ERROR_LOG("Did not compress storage");
    return false;
  }
  // check size
  if (buffer.size() > STORAGE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  // clear existing storage, this is necessary even if it's empty
  storage.erase();
  // write out the buffer to storage
  storage.write(_storagedata, buffer.rawData(), buffer.rawSize());
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", buffer.size(), STORAGE_SIZE);
  return true;
}

// read a serial buffer from storage
bool Storage::read(ByteStream &buffer)
{
  if (!buffer.init(STORAGE_SIZE)) {
    ERROR_LOGF("Could not initialize buffer with %u bytes", STORAGE_SIZE);
    return false;
  }
  storage.read(buffer.rawData());
  uint32_t compressedSize = buffer.size();
  if (!compressedSize) {
    DEBUG_LOG("Read null from storage");
    return false;
  }
  if (!buffer.decompress()) {
    ERROR_LOG("Failed to decompress buffer loaded from storage");
    buffer.clear();
    return false;
  }
  if (buffer.size() < 2) {
    ERROR_LOG("Invalid save file");
    buffer.clear();
    return false;
  }
  DEBUG_LOG("Checking version...");
  uint8_t major = 0;
  uint8_t minor = 0;
  // move to end of the buffer - 2
  buffer.moveUnserializer(buffer.size() - 2);
  buffer.unserialize(&major);
  buffer.unserialize(&minor);
  if (major != VORTEX_VERSION_MAJOR) {
    // cannot load save file because major version is different
    buffer.clear();
    return false;
  }
  if (minor != VORTEX_VERSION_MINOR) {
    // this is okay, maybe warn the user?
    DEBUG_LOG("Warning! minor version mismatch");
  }
  // trim off the extra bytes used for version
  buffer.trim(2);
  DEBUG_LOGF("Successfully loaded save (Version: %u.%u Size: %u -> %u)", 
    major, minor, compressedSize, buffer.size());
  return true;
}
