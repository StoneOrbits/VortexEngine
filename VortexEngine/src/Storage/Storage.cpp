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

class VersionStorage
{
public:
  VersionStorage(uint8_t maj = 0, uint8_t min = 0, uint8_t r1 = 0, uint8_t r2 = 0) :
    major(maj), minor(min), reserved1(r1), reserved2(r2)
  {
  }
  void serialize(ByteStream &buffer)
  {
    buffer.serialize(major);
    buffer.serialize(minor);
    buffer.serialize(reserved1);
    buffer.serialize(reserved2);
  }
  void unserialize(ByteStream &buffer)
  {
    buffer.unserialize(&major);
    buffer.unserialize(&minor);
    buffer.unserialize(&reserved1);
    buffer.unserialize(&reserved2);
  }
  // public members:
  uint8_t major;
  uint8_t minor;
  uint8_t reserved1;
  uint8_t reserved2;
};

Storage::Storage()
{
}

bool Storage::init()
{
  return true;
}

void Storage::cleanup()
{
}

// store a serial buffer to storage
bool Storage::write(ByteStream &buffer)
{
  // insert the version number at end of buffer
  VersionStorage version(VORTEX_VERSION_MAJOR, VORTEX_VERSION_MINOR);
  version.serialize(buffer);
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
  // init storage buffer with max storaeg size so we can read into it
  if (!buffer.init(STORAGE_SIZE)) {
    ERROR_LOGF("Could not initialize buffer with %u bytes", STORAGE_SIZE);
    return false;
  }
  // read directly into the rawdata of the byte array, this will
  // include the crc, size, flags and entire buffer of data
  storage.read(buffer.rawData());
  // check the compressed size of the buffer, we should have something
  uint32_t compressedSize = buffer.size();
  if (!compressedSize) {
    DEBUG_LOG("Read null from storage");
    return false;
  }
  // decompress the buffer and inflate it
  if (!buffer.decompress()) {
    ERROR_LOG("Failed to decompress buffer loaded from storage");
    buffer.clear();
    return false;
  }
  // make sure the inflated size is big enough for a version number
  if (buffer.size() < sizeof(VersionStorage)) {
    ERROR_LOG("Invalid save file");
    buffer.clear();
    return false;
  }
  DEBUG_LOG("Checking version...");
  // move to end of the buffer - size of version
  buffer.moveUnserializer(buffer.size() - sizeof(VersionStorage));
  // unserialize the version number
  VersionStorage version;
  version.unserialize(buffer);
  // reset unserializer for later
  buffer.resetUnserializer();
  // check the major version of the save file for compatibility
  if (version.major != VORTEX_VERSION_MAJOR) {
    // cannot load save file because major version is different
    DEBUG_LOGF("Warning! Major version mismatch %u != %u not loading",
      version.major, VORTEX_VERSION_MAJOR);
    buffer.clear();
    return false;
  }
  // minor version incompatibility doesn't really matter
  if (version.minor != VORTEX_VERSION_MINOR) {
    // this is okay, maybe warn the user?
    DEBUG_LOG("Warning! minor version mismatch");
  }
  // trim off the extra bytes used for version
  buffer.trim(sizeof(VersionStorage));
  DEBUG_LOGF("Successfully loaded save (Version: %u.%u Size: %u -> %u)",
    version.major, version.minor, compressedSize, buffer.size());
  return true;
}
