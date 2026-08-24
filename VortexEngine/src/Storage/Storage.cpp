#include "Storage.h"

#include <string.h>
#include <stdlib.h>

#include "../VortexConfig.h"
#include "../Memory/Memory.h"
#include "../Serial/ByteStream.h"
#include "../Log/Log.h"

#ifdef VORTEX_LIB
#include "../VortexLib/VortexLib.h"
#endif

#ifdef VORTEX_EMBEDDED
#include "../Leds/Leds.h"
#include <nvs.h>
#else // VORTEX_EMBEDDED
#ifdef _WIN32
#include <Windows.h>
#else // _WIN32
#include <unistd.h>
#endif
#endif

#define DEFAULT_STORAGE_FILENAME "FlashStorage.flash"

// the storage page reserved for the global storage space where the save
// header is kept, this must be a page number that no profile will ever
// use so that it can never collide with any of the storage pages
#define GLOBAL_STORAGE_PAGE 15

#ifdef VORTEX_LIB
std::string Storage::m_storageFilename;
#define STORAGE_FILENAME m_storageFilename.c_str()
#else
#define STORAGE_FILENAME DEFAULT_STORAGE_FILENAME
#endif

uint32_t Storage::m_lastSaveSize = 0;
uint8_t Storage::m_storagePage = 0;

Storage::Storage()
{
}

bool Storage::init()
{
#ifdef VORTEX_LIB
  if (!m_storageFilename.length() && Vortex::storageEnabled()) {
    m_storageFilename = DEFAULT_STORAGE_FILENAME;
  }
#endif
  return true;
}

void Storage::cleanup()
{
}

void Storage::setStoragePage(uint8_t page)
{
  m_storagePage = page;
}

uint8_t Storage::getStoragePage()
{
  return m_storagePage;
}

// store a serial buffer to the global storage space, this simply targets
// slot 0 of the reserved global page so that the save header is not
// affected by whichever storage page is currently selected
bool Storage::writeGlobal(ByteStream &buffer)
{
  uint8_t currentPage = m_storagePage;
  m_storagePage = GLOBAL_STORAGE_PAGE;
  bool ok = write(0, buffer);
  m_storagePage = currentPage;
  return ok;
}

// read a serial buffer from the global storage space
bool Storage::readGlobal(ByteStream &buffer)
{
  uint8_t currentPage = m_storagePage;
  m_storagePage = GLOBAL_STORAGE_PAGE;
  bool ok = read(0, buffer);
  m_storagePage = currentPage;
  return ok;
}

// store a serial buffer to the mode header slot of the currently selected
// storage page
bool Storage::writeModeHeader(ByteStream &buffer)
{
  // slot 0 of each page is the mode header which holds the number of
  // modes stored in that page, the modes themselves are in slots 1 and up
  return write(0, buffer);
}

// read the mode header from the currently selected storage page
bool Storage::readModeHeader(ByteStream &buffer)
{
  return read(0, buffer);
}

// store a serial buffer to storage
bool Storage::write(uint16_t slot, ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // success so the system thinks it all worked
    return true;
  }
#endif
  // check size
  if (buffer.rawSize() > MAX_MODE_SIZE) {
    ERROR_LOG("Buffer too big for storage space");
    return false;
  }
  if (slot >= NUM_MODE_SLOTS) {
    return false;
  }
  // just in case
  buffer.recalcCRC();
#ifdef VORTEX_EMBEDDED
  // ESP32 Arduino environment
  nvs_handle_t nvs;
  uint8_t name[3] = { (uint8_t)('a' + m_storagePage), (uint8_t)('a' + (uint8_t)slot), 0 };
  esp_err_t err = nvs_open((char *)name, NVS_READWRITE, &nvs);
  if (err != ESP_OK) {
    return false;
  }
  err = nvs_set_blob(nvs, (char *)name, buffer.rawData(), buffer.rawSize());
  if (err != ESP_OK) {
    nvs_close(nvs);
    return false;
  }
  nvs_close(nvs);
#elif defined(_WIN32)
  HANDLE hFile = CreateFile(STORAGE_FILENAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    // error
    return false;
  }
  DWORD written = 0;
  DWORD offset = (slot * MAX_MODE_SIZE) + (m_storagePage * (MAX_MODE_SIZE * NUM_MODE_SLOTS));
  SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
  if (!WriteFile(hFile, buffer.rawData(), MAX_MODE_SIZE, &written, NULL)) {
    // error
    return false;
  }
  CloseHandle(hFile);
#else
  FILE *f = fopen(STORAGE_FILENAME, "w");
  if (!f) {
    return false;
  }
  long offset = (slot * MAX_MODE_SIZE) + (m_storagePage * (MAX_MODE_SIZE * NUM_MODE_SLOTS));
  fseek(f, offset, SEEK_SET);
  if (!fwrite(buffer.rawData(), sizeof(char), MAX_MODE_SIZE, f)) {
    return false;
  }
  fclose(f);
#endif // VORTEX_EMBEDDED
  DEBUG_LOGF("Wrote %u bytes to storage (max: %u)", m_lastSaveSize, STORAGE_SIZE);
  return true;
}

// read a serial buffer from storage
bool Storage::read(uint16_t slot, ByteStream &buffer)
{
#ifdef VORTEX_LIB
  if (!Vortex::storageEnabled()) {
    // return false here, but true in write because we don't want to return
    // an empty buffer after returning true
    return false;
  }
#endif
  uint32_t size = MAX_MODE_SIZE;
  if (size > STORAGE_SIZE || size < sizeof(ByteStream::RawBuffer) + 4 || slot >= NUM_MODE_SLOTS) {
    return false;
  }
  if (!buffer.init(size)) {
    return false;
  }
#ifdef VORTEX_EMBEDDED
  // ESP32 Arduino environment
  nvs_handle_t nvs;
  uint8_t name[3] = { (uint8_t)('a' + m_storagePage), (uint8_t)('a' + (uint8_t)slot), 0 };
  esp_err_t err = nvs_open((char *)name, NVS_READWRITE, &nvs);
  if (err != ESP_OK) {
    nvs_close(nvs);
    //Leds::holdAll(RGB_YELLOW);
    return false;
  }
  size_t read_size = size;
  // build a two letter name based on the slot and page
  err = nvs_get_blob(nvs, (char *)name, buffer.rawData(), &read_size);
  if (err != ESP_OK) {
    nvs_close(nvs);
    //Leds::holdAll(RGB_PURPLE);
    return false;
  }
  nvs_close(nvs);
#elif defined(_WIN32)
  HANDLE hFile = CreateFile(STORAGE_FILENAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    // error
    return false;
  }
  DWORD bytesRead = 0;
  DWORD offset = (slot * MAX_MODE_SIZE) + (m_storagePage * (MAX_MODE_SIZE * NUM_MODE_SLOTS));
  SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
  if (!ReadFile(hFile, buffer.rawData(), MAX_MODE_SIZE, &bytesRead, NULL)) {
    // error
    return false;
  }
  CloseHandle(hFile);
#else
  FILE *f = fopen(STORAGE_FILENAME, "r");
  if (!f) {
    return false;
  }
  long offset = (slot * MAX_MODE_SIZE) + (m_storagePage * (MAX_MODE_SIZE * NUM_MODE_SLOTS));
  fseek(f, offset, SEEK_SET);
  if (!fread(buffer.rawData(), sizeof(char), MAX_MODE_SIZE, f)) {
    return false;
  }
  fclose(f);
#endif
  // ensure the internal buffer is sane after reading it out, this
  // prevents segfaults if the internal size reports larger than capacity
  buffer.sanity();
  // check crc immediately since we read into raw data copying the
  // array could be dangerous
  if (!buffer.checkCRC()) {
    buffer.clear();
    ERROR_LOG("Could not verify buffer");
    return false;
  }
  m_lastSaveSize = size;
  DEBUG_LOGF("Loaded savedata (Size: %u)", buffer.size());
  return true;
}

uint32_t Storage::lastSaveSize()
{
  return m_lastSaveSize;
}
