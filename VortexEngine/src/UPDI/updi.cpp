#include "updi.h"

#ifdef VORTEX_EMBEDDED

#include "../VortexConfig.h"
#include "../Time/TimeControl.h"
#include "../Log/Log.h"
#include "../Leds/Leds.h"
#include "../Serial/ByteStream.h"
#include "../Patterns/Pattern.h"
#include "../Modes/Mode.h"

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "soc/uart_periph.h"
#include "esp_rom_gpio.h"
#include "hal/uart_types.h"
#include "soc/gpio_struct.h"
#include "soc/soc.h"
#include "soc/io_mux_reg.h"
#include "esp_timer.h"
#include "driver/timer.h"
#include <Arduino.h>

#define HIGH 0x1
#define LOW 0x0

#define FLASH_PAGE_SIZE 128
#define EEPROM_PAGE_SIZE 64

// Define GPIO pin for UPDI communication
#define UPDI_PIN GPIO_NUM_10

// Timing macros (adjust as needed for your specific setup)
#define BIT_TIME_US 30
#define UPDI_BREAK_PAUSE_US    2000
#define UPDI_BREAK_DELAY_US    14000

// Direct GPIO manipulation macros using ESP32 registers
#define GPIO_SET_HIGH(gpio_num) (GPIO.out_w1ts.val = (1U << gpio_num))
#define GPIO_SET_LOW(gpio_num) (GPIO.out_w1tc.val = (1U << gpio_num))
#define GPIO_READ(gpio_num) ((GPIO.in.val >> gpio_num) & 0x1)

// Configure GPIO for output
#define GPIO_SET_OUTPUT(gpio_num) (GPIO.enable_w1ts.val = (1U << gpio_num))

// Configure GPIO for input
// the key here is setting the gpio as GPIO_FLOATING to prevent contention
#define GPIO_SET_INPUT(gpio_num) (GPIO.enable_w1tc.val = (1U << gpio_num)); gpio_set_pull_mode((gpio_num_t)gpio_num, GPIO_FLOATING);

#endif

bool UPDI::init()
{
  return true;
}

void UPDI::cleanup()
{
}

bool UPDI::readHeader(ByteStream &header)
{
#ifdef VORTEX_EMBEDDED
  if (!header.init(5)) {
    return false;
  }
  enterProgrammingMode();
  uint8_t *ptr = (uint8_t *)header.rawData();
  uint16_t addr = 0x1400;
  stptr_p((const uint8_t *)&addr, 2);
  for (uint16_t i = 0; i < header.rawSize(); ++i) {
    ptr[i] = ldinc_b();
  }
  header.sanity();
  if (!header.checkCRC()) {
    header.clear();
    ERROR_LOG("ERROR Header CRC Invalid!");
    reset();
    return false;
  }
#endif
  return true;
}

bool UPDI::readMode(uint8_t idx, ByteStream &modeBuffer)
{
#ifdef VORTEX_EMBEDDED
  // initialize mode buffer
  if (!modeBuffer.init(76)) {
    return false;
  }
  enterProgrammingMode();
  // 76 is the max duo mode size (the slot size)
  uint8_t *ptr = (uint8_t *)modeBuffer.rawData();
  uint16_t numBytes = modeBuffer.rawSize();
  uint16_t base;
  // there are 3 modes in the eeprom after the header
  if (idx < 3) {
    // 0x1400 is eeprom base
    // 17 is size of duo header
    // 76 is size of each duo mode
    base = 0x1400 + 17 + (idx * 76);
  } else {
    // 0xFe00 is the end of flash, 0x200 before
    base = 0xFe00 + ((idx - 3) * 76);
  }
  stptr_p((const uint8_t *)&base, 2);
  //rep(numBytes - 1);
  for (uint16_t i = 0; i < modeBuffer.rawSize(); ++i) {
    ptr[i] = ldinc_b();
  }
  reset();
#endif
  return true;
}

bool UPDI::writeHeader(ByteStream &headerBuffer)
{
#ifdef VORTEX_EMBEDDED
  headerBuffer.sanity();
  if (!headerBuffer.checkCRC()) {
    ERROR_LOG("ERROR Header CRC Invalid!");
    reset();
    return false;
  }
  if (headerBuffer.rawSize() != 17) {
    ERROR_LOG("ERROR Header Size Invalid!");
    reset();
    return false;
  }
  enterProgrammingMode();
  //uint8_t *ptr = (uint8_t *)headerBuffer.rawData();
  //for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; ++i) {
  //  uint16_t addr = 0x1400 + i;
  //  uint8_t value;
  //  if (i < headerBuffer.rawSize()) {
  //    // if this is within the slot then write out the new data
  //    value = ptr[i];
  //  } else {
  //    // otherwise just write-back the same value to fill the pagebuffer
  //    stptr_p((const uint8_t *)&addr, 2);
  //    value = ld_b();
  //  }
  //  sts_b(addr, value);
  //}
  //nvmCmd(NVM_ERWP);
  //nvmWait();
  //uint8_t pageBuffer[EEPROM_PAGE_SIZE];
  //if (!readEepromPage(0x1400, pageBuffer)) {
  //  return false;
  //}
  //memcpy(pageBuffer, headerBuffer.rawData(), headerBuffer.rawSize());
  //if (!writeEepromPage(0x1400, pageBuffer)) {
  //  return false;
  //}

    // read out the page so the |xxxxxxxxxxx part
    ByteStream pageBuffer(EEPROM_PAGE_SIZE);
    uint8_t *pagePtr = (uint8_t *)pageBuffer.data();
    uint16_t eepromStart = 0x1400;
    stptr_p((const uint8_t *)&eepromStart, 2);
    for (uint16_t i = 0; i < EEPROM_PAGE_SIZE; ++i) {
      pagePtr[i] = ldinc_b();
    }
    nvmWait();

    // overlay the actual data, so the SSSSS part of |xxxxxxSSSS
    memcpy(pagePtr, headerBuffer.rawData(), headerBuffer.rawSize());

    // write back the page
    nvmCmd(NVM_PBC);
    stptr_p((const uint8_t *)&eepromStart, 2);
    //stcs(Control_A, 0x0E);
    //rep(pageSize - 1);
    //stinc_b_noget(buf[0]);
    //for (uint8_t i = 1; i < pageSize; ++i) {
    //  sendByte(buf[i]);
    //}
    for (uint16_t i = 0; i < EEPROM_PAGE_SIZE; ++i) {
      stinc_b_noget(pagePtr[i]);
      //sts_b(eepromStart + i, pagePtr[i]);
    }
    //stcs(Control_A, 0x06);
    nvmCmd(NVM_WP);
    nvmWait();

  //reset();
#endif
  return true;
}

bool UPDI::writeMode(uint8_t idx, ByteStream &modeBuffer)
{
#ifdef VORTEX_EMBEDDED
  modeBuffer.sanity();
  if (!modeBuffer.checkCRC()) {
    ERROR_LOG("ERROR Mode CRC Invalid!");
    reset();
    return false;
  }
  // there are 3 modes in the eeprom after the header
  if (idx < 3) {
    return writeModeEeprom(idx, modeBuffer);
  }
  return writeModeFlash(idx, modeBuffer);
#else
  return true;
#endif
}

#ifdef VORTEX_EMBEDDED
bool UPDI::writeModeEeprom(uint8_t idx, ByteStream &modeBuffer)
{
  enterProgrammingMode();
  // 0x1400 is eeprom base
  // 17 is size of duo header
  // 76 is size of each duo mode
  uint16_t base = 0x1400 + 17 + (idx * 76);
  // the size of the mode being written out
  uint16_t size = modeBuffer.rawSize();
  uint8_t *ptr = (uint8_t *)modeBuffer.rawData();
  // The storage slot may lay across a page boundary which means potentially writing
  // two pages instead of just one. In order to update only part of a page, the page
  // buffer must be filled with both the previous content along with the new data.
  // For example, imagine 2 pages of data: |xxxxxxSSSS|SSSxxxxxxx| the x's are other
  // data that must be preserved, and the S's denote the storage slot being written.
  // This would take place over two iterations of the loop, each writing out one page
  // by read-then-writing-back the x's and writing out the new S's. This is necessary
  // because the page buffer must be filled to perform a page write, at least I think
  while (size > 0) {
    uint16_t pageStart = base & ~(EEPROM_PAGE_SIZE - 1);
    uint16_t offset = base % EEPROM_PAGE_SIZE;
    uint16_t space = EEPROM_PAGE_SIZE - offset;
    uint16_t writeSize = (size < space) ? size : space;

    // read out the page so the |xxxxxxxxxxx part
    ByteStream pageBuffer(EEPROM_PAGE_SIZE);
    uint8_t *pagePtr = (uint8_t *)pageBuffer.data();
    stptr_p((const uint8_t *)&pageStart, 2);
    for (uint16_t i = 0; i < EEPROM_PAGE_SIZE; ++i) {
      pagePtr[i] = ldinc_b();
    }
    nvmWait();

    // overlay the actual data, so the SSSSS part of |xxxxxxSSSS
    memcpy(pagePtr + offset, ptr, writeSize);

    // write back the page
    //nvmCmd(NVM_PBC);
    //stptr_p((const uint8_t *)&pageStart, 2);
    //stcs(Control_A, 0x0E);
    //rep(pageSize - 1);
    //stinc_b_noget(buf[0]);
    //for (uint8_t i = 1; i < pageSize; ++i) {
    //  sendByte(buf[i]);
    //}
    //sts_b(pageStart, pagePtr[0]);
    for (uint16_t i = 0; i < EEPROM_PAGE_SIZE; ++i) {
      //stinc_b(pagePtr[i]);
      sts_b(pageStart + i, pagePtr[i]);
    }
    //stcs(Control_A, 0x06);
    nvmCmd(NVM_ERWP);
    nvmWait();

    //for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; ++i) {
    //  uint8_t value;
    //  if (i >= offset && i < offset + writeSize) {
    //    // if this is within the slot then write out the new data
    //    value = ptr[i - offset];
    //  } else {
    //    // otherwise just write-back the same value to fill the pagebuffer
    //    uint16_t addr = pageStart + i;
    //    stptr_p((const uint8_t *)&addr, 2);
    //    value = ld_b();
    //  }
    //  sts_b(pageStart + i, value);
    //}

    //bool cont = false;
    //for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; ++i) {
    //  // if outside the write area
    //  if (i < offset || i >= (offset + writeSize)) {
    //    // if continuously reading just read and continue
    //    if (cont) {
    //      ptr[i] = ldinc_b();
    //      continue;
    //    }
    //    // otherwise start reading here
    //    uint16_t addr = pageStart + i;
    //    stptr_p((const uint8_t *)&addr, 2);
    //    ptr[i] = ldinc_b();
    //    cont = true;
    //  } else {
    //    // no longer continuously reading
    //    cont = false;
    //  }
    //}


    //// first read out the rest of the page
    //stptr_p((const uint8_t *)&base, 2);
    ////rep(numBytes - 1);
    //for (uint16_t i = 0; i < modeBuffer.rawSize(); ++i) {
    //  ptr[i] = ldinc_b();
    //}

    //nvmCmd(NVM_ERWP);
    //nvmWait();
    //nvmCmd(NVM_PBC);
    //stptr_w(pageStart + offset);
    //stcs(Control_A, 0x0E);
    //rep(writeSize - 1);
    //stinc_b_noget(ptr[0]);
    //for (uint8_t i = 1; i < writeSize; ++i) {
    //  sendByte(ptr[i]);
    //}
    //stcs(Control_A, 0x06);
    //nvmCmd(NVM_WP);
    //nvmWait();

    // continue to the next page
    base += writeSize;
    ptr += writeSize;
    size -= writeSize;
  }
  reset();
  return true;
}

bool UPDI::writeModeFlash(uint8_t idx, ByteStream &modeBuffer)
{
  enterProgrammingMode();
  // there are 3 modes in the eeprom after the header
  // 0xFe00 is the end of flash, 0x200 before
  uint16_t base = 0xFe00 + ((idx - 3) * 76);
  uint16_t size = modeBuffer.rawSize();
  uint8_t *ptr = (uint8_t *)modeBuffer.rawData();
  // The storage slot may lay across a page boundary which means potentially writing
  // two pages instead of just one. In order to update only part of a page, the page
  // buffer must be filled with both the previous content along with the new data.
  // For example, imagine 2 pages of data: |xxxxxxSSSS|SSSxxxxxxx| the x's are other
  // data that must be preserved, and the S's denote the storage slot being written.
  // This would take place over two iterations of the loop, each writing out one page
  // by read-then-writing-back the x's and writing out the new S's. This is necessary
  // because the page buffer must be filled to perform a page write, at least I think
  while (size > 0) {
    uint16_t pageStart = base & ~(FLASH_PAGE_SIZE - 1);
    uint16_t offset = base % FLASH_PAGE_SIZE;
    uint16_t space = FLASH_PAGE_SIZE - offset;
    uint16_t writeSize = (size < space) ? size : space;

    // read out the page
    ByteStream pageBuffer(FLASH_PAGE_SIZE);
    uint8_t *pagePtr = (uint8_t *)pageBuffer.data();
//#define NUM_SLICES 8
//#define PAGE_SLICE (FLASH_PAGE_SIZE / NUM_SLICES)
//    for (uint16_t j = 0; j < NUM_SLICES; ++j) {
//      uint16_t pos = pageStart = j;
//      stptr_p((const uint8_t *)&pos, 2);
//      for (uint16_t i = 0; i < PAGE_SLICE; ++i) {
//        ptr[(j * PAGE_SLICE) + i] = ldinc_b();
//      }
//      // wait idk
//      nvmWait();
//    }

    stptr_p((const uint8_t *)&pageStart, 2);
    for (uint16_t i = 0; i < FLASH_PAGE_SIZE; ++i) {
      pagePtr[i] = ldinc_b();
    }
    //if (!readFlashPage(pageStart, pageBuffer)) {
    //  return false;
    //}
    // copy in the thing
    memcpy(pagePtr + offset, ptr, writeSize);

    // write back the page
    //nvmCmd(NVM_PBC);
    //stptr_p((const uint8_t *)&pageStart, 2);
    //stcs(Control_A, 0x0E);
    //rep(pageSize - 1);
    //stinc_b_noget(buf[0]);
    //for (uint8_t i = 1; i < pageSize; ++i) {
    //  sendByte(buf[i]);
    //}
    //sts_b(pageStart, pagePtr[0]);
    for (uint16_t i = 0; i < FLASH_PAGE_SIZE; ++i) {
      //stinc_b(pagePtr[i]);
      sts_b(pageStart + i, pagePtr[i]);
    }
    //stcs(Control_A, 0x06);
    nvmCmd(NVM_ERWP);
    nvmWait();

    //if (!writeFlashPage(pageStart, pageBuffer)) {
    //  return false;
    //}

    //for (uint8_t i = 0; i < FLASH_PAGE_SIZE; ++i) {
    //  uint8_t value;
    //  if (i >= offset && i < offset + writeSize) {
    //    // if this is within the slot then write out the new data
    //    value = ptr[i - offset];
    //  } else {
    //    // otherwise just write-back the same value to fill the pagebuffer
    //    uint16_t addr = pageStart + i;
    //    stptr_p((const uint8_t *)&addr, 2);
    //    value = ld_b();
    //  }
    //  sts_b(pageStart + i, value);
    //}

    //bool cont = false;
    //for (uint8_t i = 0; i < FLASH_PAGE_SIZE; ++i) {
    //  // if outside the write area
    //  if (i < offset || i >= (offset + writeSize)) {
    //    // if continuously reading just read and continue
    //    if (cont) {
    //      ptr[i] = ldinc_b();
    //      continue;
    //    }
    //    // otherwise start reading here
    //    uint16_t addr = pageStart + i;
    //    stptr_p((const uint8_t *)&addr, 2);
    //    ptr[i] = ldinc_b();
    //    cont = true;
    //  } else {
    //    // no longer continuously reading
    //    cont = false;
    //  }
    //}



    //nvmCmd(NVM_ERWP);
    //nvmWait();
    //nvmCmd(NVM_PBC);
    //stptr_w(pageStart + offset);
    //stcs(Control_A, 0x0E);
    //rep(writeSize - 1);
    //stinc_b_noget(ptr[0]);
    //for (uint8_t i = 1; i < writeSize; ++i) {
    //  sendByte(ptr[i]);
    //}
    //stcs(Control_A, 0x06);
    //nvmCmd(NVM_WP);
    //nvmWait();

    // continue to the next page
    base += writeSize;
    ptr += writeSize;
    size -= writeSize;
  }
  reset();
  return true;
}

bool UPDI::writePage(uint16_t addr, const uint8_t *buf, uint16_t pageSize)
{
    stptr_w(addr);
    stcs(Control_A, 0x0E);
    rep(FLASH_PAGE_SIZE - 1);
    stinc_b_noget(buf[0]);
    for (uint16_t i = 1; i < FLASH_PAGE_SIZE; ++i) {
      sendByte(buf[i]);
    }
    stcs(Control_A, 0x06);
    nvmCmd(NVM_ERWP);
    nvmWait();

  //nvmCmd(NVM_PBC);
  //stptr_p((const uint8_t *)&addr, 2);
  ////stcs(Control_A, 0x0E);
  ////rep(pageSize - 1);
  ////stinc_b_noget(buf[0]);
  ////for (uint8_t i = 1; i < pageSize; ++i) {
  ////  sendByte(buf[i]);
  ////}
  //for (uint8_t i = 0; i < pageSize; ++i) {
  //  //stinc_b_noget(buf[i]);
  //  sts_b(addr + i, buf[i]);
  //}
  ////stcs(Control_A, 0x06);
  //nvmCmd(NVM_ERWP);
  //nvmWait();
  return true;
}

bool UPDI::readPage(uint8_t addr, uint8_t *buf, uint16_t pageSize)
{
  stptr_p((const uint8_t *)&addr, 2);
  for (uint16_t i = 0; i < pageSize; ++i) {
    buf[i] = ldinc_b();
  }
  //stcs(Control_A, 0x0E);
  //rep(pageSize - 1);
  //buf[0] = ldinc_b();
  //for (uint16_t i = 0; i < pageSize; ++i) {
  //  stptr_w(addr + i);
  //  buf[i] = ld_b();
  //}
  //stcs(Control_A, 0x06);
  return true;
}

bool UPDI::writeFlashPage(uint16_t addr, const uint8_t *buf)
{
  return writePage(addr, buf, FLASH_PAGE_SIZE);
}

bool UPDI::readFlashPage(uint8_t addr, uint8_t *buf)
{
  return readPage(addr, buf, FLASH_PAGE_SIZE);
}

bool UPDI::writeEepromPage(uint16_t addr, const uint8_t *buf)
{
  return writePage(addr, buf, EEPROM_PAGE_SIZE);
}

bool UPDI::readEepromPage(uint8_t addr, uint8_t *buf)
{
  return readPage(addr, buf, EEPROM_PAGE_SIZE);
}

#endif

bool UPDI::writeFirmware(uint32_t position, ByteStream &firmwareBuffer)
{
#ifdef VORTEX_EMBEDDED
  firmwareBuffer.sanity();
  if (!firmwareBuffer.checkCRC()) {
    ERROR_LOG("ERROR Header CRC Invalid!");
    reset();
    return false;
  }
  enterProgrammingMode();
  // 76 is the max duo mode size (the slot size)
  uint8_t *ptr = (uint8_t *)firmwareBuffer.data();
  // there are 3 modes in the eeprom after the header
  // 0xFe00 is the end of flash, 0x200 before
  uint16_t base = 0x8000 + position;
  uint16_t size = firmwareBuffer.size();
  while (size > 0) {
    uint16_t writeSize = (size < FLASH_PAGE_SIZE) ? size : FLASH_PAGE_SIZE;
    //for (uint8_t i = 0; i < FLASH_PAGE_SIZE; ++i) {
    //  uint8_t value = (i < writeSize) ? ptr[i] : 0;
    //  sts_b(base + i, value);
    //}
    stptr_w(base);
    stcs(Control_A, 0x0E);
    rep(FLASH_PAGE_SIZE - 1);
    stinc_b_noget(ptr[0]);
    for (uint16_t i = 1; i < FLASH_PAGE_SIZE; ++i) {
      sendByte((i < writeSize) ? ptr[i] : 0);
      //sts_b(base + i, (i < writeSize) ? ptr[i] : 0);
    }
    stcs(Control_A, 0x06);
    nvmCmd(NVM_ERWP);
    nvmWait();
    // continue to the next page
    base += writeSize;
    ptr += writeSize;
    size -= writeSize;
  }
  reset();
#endif
  return true;
}

bool UPDI::eraseMemory()
{
  uint8_t status = 0;
#ifdef VORTEX_EMBEDDED
  sendDoubleBreak();
  sendEraseKey();
  status = ldcs(ASI_Key_Status);
  if (status != 0x8) {
    ERROR_LOGF("Erasing mem, bad key status: 0x%02x...", status);
  }
  reset();
#endif
  return status == 0x8;
}

bool UPDI::readMemory()
{
#if 0
  ByteStream *modes = new ByteStream[headerData->numModes];
  if (!modes) {
    ERROR_LOG("Failed to initialize modes list");
    return;
  }
  for (uint8_t m = 0; m < headerData->numModes; ++m) {
    ByteStream &mode = modes[m];
    // don't mess with a bad mode
    mode.sanity();
    // now check it's crc
    if (!mode.checkCRC()) {
      ERROR_LOGF("ERROR Mode %u CRC Invalid!", m);
      continue;
    }
    // NOTE: this is a bit of a hack, this Mode object has 20 leds because it's for
    //       a chromadeck, but we will just use the first 2 and disregard the mode->ledCount
    Mode newMode;
    newMode.unserialize(mode);
    ERROR_LOGF("Mode %u:", m);
    ERROR_LOGF(" Flags: 0x%08x", newMode.getFlags());
    ERROR_LOG(" Patterns:");
    for (uint8_t p = 0; p < 2; ++p) {
      Pattern *pat = newMode.getPattern((LedPos)p);
      ERROR_LOGF("  Pattern %u:", p);
      ERROR_LOGF("   Pattern ID: %u", pat->getPatternID());
      PatternArgs args;
      pat->getArgs(args);
      // should only be 5 or 7 args on duo
      if (pat->getNumArgs() == 5) {
        ERROR_LOGF("   Params: %u %u %u %u %u", args.arg1, args.arg2, args.arg3, args.arg4, args.arg5);
      } else if (pat->getNumArgs() == 7) {
        ERROR_LOGF("   Params: %u %u %u %u %u %u %u", args.arg1, args.arg2, args.arg3, args.arg4, args.arg5, args.arg6, args.arg7);
      } else {
        // otherwise just log all 8
        ERROR_LOGF("   Params: %u %u %u %u %u %u %u %u", args.arg1, args.arg2, args.arg3, args.arg4, args.arg5, args.arg6, args.arg7, args.arg8);
      }
      Colorset set = pat->getColorset();
      switch (set.numColors()) {
      case 0:
        ERROR_LOG("   Colors: none");
        break;
      case 1:
        ERROR_LOGF("   Colors: %06X", set.get(0).raw());
        break;
      case 2:
        ERROR_LOGF("   Colors: %06X, %06X", set.get(0).raw(), set.get(1).raw());
        break;
      case 3:
        ERROR_LOGF("   Colors: %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw());
        break;
      case 4:
        ERROR_LOGF("   Colors: %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw());
        break;
      case 5:
        ERROR_LOGF("   Colors: %06X, %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw(), set.get(4).raw());
        break;
      case 6:
        ERROR_LOGF("   Colors: %06X, %06X, %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw(), set.get(4).raw(), set.get(5).raw());
        break;
      case 7:
        ERROR_LOGF("   Colors: %06X, %06X, %06X, %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw(), set.get(4).raw(), set.get(5).raw(), set.get(6).raw());
        break;
      case 8:
      default:
        ERROR_LOGF("   Colors: %06X, %06X, %06X, %06X, %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw(), set.get(4).raw(), set.get(5).raw(), set.get(6).raw(), set.get(7).raw());
        break;
      }
    }
  }
  reset();
#endif
  return true;
}

bool UPDI::writeMemory()
{
  return true;
}

#ifdef VORTEX_EMBEDDED

void UPDI::enterProgrammingMode()
{
  uint8_t mode;
  while (1) {
    sendDoubleBreak();
    stcs(Control_A, 0x6);
    mode = cpu_mode<0xEF>();
    if (mode != 0x82 && mode != 0x21 && mode != 0xA2 && mode != 0x08) {
      sendDoubleBreak();
      uint8_t status = ldcs(Status_B);
      ERROR_LOGF("Bad CPU Mode 0x%02x... error: 0x%02x", mode, status);
      reset();
      continue;
      //return false;
    }
    if (mode != 0x08) {
      sendProgKey();
      uint8_t status = ldcs(ASI_Key_Status);
      if (status != 0x10) {
        ERROR_LOGF("Bad prog key status: 0x%02x", status);
        reset();
        continue;
        //return false;
      }
      reset();
    }
    // break the while (1)
    break;
  }
  mode = cpu_mode();
  while (mode != 0x8) {
    mode = cpu_mode();
  }
}

void UPDI::resetOn()
{
  stcs(ASI_Reset_Request, 0x59);
}

bool UPDI::resetOff()
{
  stcs(ASI_Reset_Request, 0x0);
  uint8_t timeout = 0;
  while (cpu_mode<0x0E>() == 0 && timeout < 10) {
    Time::delayMicroseconds(100);
    timeout++;
  }
  return timeout < 2;
}

bool UPDI::reset()
{
  resetOn();
  return resetOff();
}

void UPDI::sendByte(uint8_t byte)
{
  uint8_t parity = __builtin_parity(byte);
  GPIO_SET_OUTPUT(UPDI_PIN);
  // Send start bit (low)
  GPIO_SET_LOW(UPDI_PIN);
  Time::delayMicroseconds(BIT_TIME_US);
  // Send data bits (LSB first)
  for (uint8_t i = 0; i < 8; i++) {
    if (byte & 0x01) {
      GPIO_SET_HIGH(UPDI_PIN);
    } else {
      GPIO_SET_LOW(UPDI_PIN);
    }
    Time::delayMicroseconds(BIT_TIME_US);
    byte >>= 1;
  }
  // Send parity bit (even parity)
  if (parity) {
    GPIO_SET_HIGH(UPDI_PIN);
  } else {
    GPIO_SET_LOW(UPDI_PIN);
  }
  Time::delayMicroseconds(BIT_TIME_US);
  // Send stop bit (high)
  GPIO_SET_HIGH(UPDI_PIN);
  Time::delayMicroseconds(BIT_TIME_US * 2);
  GPIO_SET_INPUT(UPDI_PIN);
}

uint8_t UPDI::receiveByte()
{
  uint8_t byte = 0;
  uint32_t counter = 0;
#define TIMEOUT_TT 40000000
  while (GPIO_READ(UPDI_PIN) == 1 && counter++ < TIMEOUT_TT);
  if (counter >= TIMEOUT_TT) {
    ERROR_LOG("Timed out waiting for start bit");
    return 0;
  }
  Time::delayMicroseconds(BIT_TIME_US / 2); // Half bit time for sampling
  // Read data bits (LSB first)
  for (uint8_t i = 0; i < 8; i++) {
    Time::delayMicroseconds(BIT_TIME_US);
    byte >>= 1;
    if (GPIO_READ(UPDI_PIN)) {
      byte |= 0x80;
    }
  }
  // Read and discard parity bit
  Time::delayMicroseconds(BIT_TIME_US);
  // Read stop bit
  Time::delayMicroseconds(BIT_TIME_US);
  if (GPIO_READ(UPDI_PIN) == 0) {
    //ERROR_LOG("Stop bit error");
  }
  Time::delayMicroseconds(BIT_TIME_US * 2);
  return byte;
}

void UPDI::sendBreak()
{
  GPIO_SET_OUTPUT(UPDI_PIN);
  GPIO_SET_LOW(UPDI_PIN);
  Time::delayMicroseconds(UPDI_BREAK_DELAY_US);
  GPIO_SET_HIGH(UPDI_PIN);
  Time::delayMicroseconds(UPDI_BREAK_DELAY_US);
}

void UPDI::sendDoubleBreak()
{
  GPIO_SET_OUTPUT(UPDI_PIN);

  // First break signal
  GPIO_SET_LOW(UPDI_PIN);
  Time::delayMicroseconds(UPDI_BREAK_DELAY_US);
  GPIO_SET_HIGH(UPDI_PIN);
  Time::delayMicroseconds(UPDI_BREAK_PAUSE_US);

  // Second break signal
  GPIO_SET_LOW(UPDI_PIN);
  Time::delayMicroseconds(UPDI_BREAK_DELAY_US);
  GPIO_SET_HIGH(UPDI_PIN);
  Time::delayMicroseconds(UPDI_BREAK_DELAY_US);
}

void UPDI::sendKey(const char *key)
{
  sendByte(0x55);
  sendByte(0xE0);
  for (int8_t i = 7; i >= 0; --i) {
    sendByte(key[i]);
  }
}

uint8_t UPDI::ldcs(cs_reg reg)
{
  sendByte(0x55);  // SYNCH
  sendByte(0x80 + reg);
  return receiveByte();
}

void UPDI::stcs(cs_reg r, uint8_t data)
{
  sendByte(0x55);
  sendByte(0xC0 + r);
  sendByte(data);
}

void UPDI::stptr_p(const uint8_t *addr_p, uint8_t n)
{
  sendByte(0x55);
  sendByte(0x68 + --n);
  sendByte(*(addr_p++));
  if (n >= 1) {
    sendByte(*(addr_p++));
  }
  if (n >= 2) {
    sendByte(*addr_p);
  }
  receiveByte();
}

void UPDI::stptr_l(uint32_t address)
{
  sendByte(0x55);
  sendByte(0x6A);
  sendByte(address & 0xFF);
  sendByte((address >> 8) & 0xFF);
  sendByte((address >> 16) & 0xFF);
  receiveByte();
}

void UPDI::stptr_w(uint16_t address)
{
  sendByte(0x55);
  sendByte(0x69);
  sendByte(address & 0xFF);
  sendByte(address >> 8);
  receiveByte();
}

void UPDI::stptr_inc_16(uint8_t *data, uint16_t len)
{
  sendByte(0x55);
  sendByte(0x65);
  receiveByte();
  uint16_t n = 2;
  while (n < len)
  {
    sendByte(data[n]);
    sendByte(data[n + 1]);
    receiveByte();
    n += 2;
  }
}

void UPDI::rep(uint8_t repeats)
{
  sendByte(0x55);
  sendByte(0xA0);
  sendByte(repeats);
}

uint8_t UPDI::ldinc_b()
{
  sendByte(0x55);
  sendByte(0x24);
  return receiveByte();
}

void UPDI::sts_b(uint16_t address, uint8_t data)
{
  sendByte(0x55);
  sendByte(0x44);
  sendByte(address & 0xFF);
  sendByte(address >> 8);
  receiveByte();
  sendByte(data);
  receiveByte();
}

uint8_t UPDI::lds_b(uint16_t address)
{
  sendByte(0x55);
  sendByte(0x04);
  sendByte(address & 0xFF);
  sendByte(address >> 8);
  return receiveByte();
}

uint8_t UPDI::ld_b()
{
  sendByte(0x55);
  sendByte(0x20);
  return receiveByte();
}

void UPDI::stinc_b(uint8_t data)
{
  sendByte(0x55);
  sendByte(0x100);
  sendByte(data);
  receiveByte();
}

void UPDI::stinc_b_noget(uint8_t data)
{
  sendByte(0x55);
  sendByte(0x64);
  sendByte(data);
}

void UPDI::stinc_w_noget(uint16_t data)
{
  sendByte(0x55);
  sendByte(0x65);
  sendByte(data & 0xFF);
  sendByte((data >> 8) & 0xFF);
}


#endif

