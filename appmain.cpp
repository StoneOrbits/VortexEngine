#include <inttypes.h>
#include <avr/io.h>

#define STORAGE_SIZE PROGMEM_PAGE_SIZE
#define storage_data ((uint8_t *)0x10000 - 0x400)

// store a serial buffer to storage
bool storage_write(uint8_t *buf)
{
  // copy in page
  for (uint8_t i = 0; i < PROGMEM_PAGE_SIZE; ++i) {
    storage_data[i] = buf[i];
  }
    // Erase + write the flash page
  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, 0x3);
  while (NVMCTRL.STATUS & 0x3);
  return (NVMCTRL.STATUS != 4);
}

// read a serial buffer from storage
bool storage_read(uint8_t *buf)
{
  // Read the data from EEPROM into the buffer
  for (uint32_t i = 0; i < STORAGE_SIZE; ++i) {
    buf[i] = storage_data[i];
  }
  return true;
}

int main()
{
  uint8_t inbuf[STORAGE_SIZE] = {0};
  uint8_t outbuf[STORAGE_SIZE] = {0};
  for (uint8_t i = 0; i < STORAGE_SIZE; ++i) {
    inbuf[i] = i;
  }
  if (!storage_write(inbuf)) {
    return 1;
  }
  if (!storage_read(outbuf)) {
    return 1;
  }
  for (uint8_t i = 0; i < STORAGE_SIZE; ++i) {
    if (inbuf[i] != outbuf[i]) {
      return 1;
    }
  }
  
  return 0;
}
