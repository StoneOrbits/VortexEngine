#include "UPDI.h"
#include "../VortexConfig.h"
#include "../Time/TimeControl.h"
#include "../Log/Log.h"
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

#ifdef VORTEX_EMBEDDED
#include <Arduino.h>
#else
#define HIGH 0x1
#define LOW 0x0
#endif

// Define GPIO pin for UPDI communication
#define UPDI_PIN GPIO_NUM_10

// Timing macros (adjust as needed for your specific setup)
#define BIT_TIME_US 100
#define UPDI_BREAK_PAUSE_US    2000
#define UPDI_BREAK_DELAY_US    24000

// Direct GPIO manipulation macros using ESP32 registers
#define GPIO_SET_HIGH(gpio_num) (GPIO.out_w1ts.val = (1U << gpio_num))
#define GPIO_SET_LOW(gpio_num) (GPIO.out_w1tc.val = (1U << gpio_num))
#define GPIO_READ(gpio_num) ((GPIO.in.val >> gpio_num) & 0x1)

// Configure GPIO for output
#define GPIO_SET_OUTPUT(gpio_num) (GPIO.enable_w1ts.val = (1U << gpio_num))

// Configure GPIO for input
// the key here is setting the gpio as GPIO_FLOATING to prevent contention
#define GPIO_SET_INPUT(gpio_num) (GPIO.enable_w1tc.val = (1U << gpio_num)); gpio_set_pull_mode((gpio_num_t)gpio_num, GPIO_FLOATING);

bool UPDI::init()
{
  return true;
}

void UPDI::cleanup()
{
}

void UPDI::eraseMemory()
{
  sendDoubleBreak();
  enterProgrammingMode();
  sendEraseKey();
  uint8_t status = ldcs(ASI_Key_Status);
  if (status != 0x8) {
    INFO_LOGF("Erasing mem, bad key status: 0x%02x...", status);
  }
  reset();
}

void UPDI::readMemory()
{
  sendDoubleBreak();
  enterProgrammingMode();
  sendProgKey();
  uint8_t status = ldcs(ASI_Key_Status);
  if (status != 0x10) {
    INFO_LOGF("Bad prog key status: 0x%02x", status);
    return;
  }
  reset();
  status = cpu_mode();
  while (status != 0x8) {
    status = cpu_mode();
  }

  ByteStream header(5);
  uint8_t *ptr = (uint8_t *)header.rawData();
  for (uint16_t i = 0; i < header.rawSize(); ++i) {
    uint16_t addr = 0x1400 + i;
    stptr_p((const uint8_t *)&addr, 2);
    ptr[i] = ld_b();
  }
  if (!header.checkCRC()) {
    INFO_LOG("ERROR Header CRC Invalid!");
    reset();
    return;
  }
  struct HeaderData
  {
    uint8_t vMajor;
    uint8_t vMinor;
    uint8_t globalFlags;
    uint8_t brightness;
    uint8_t numModes;
  };
  HeaderData *headerData = (HeaderData *)header.data();

  INFO_LOG("Header:");
  INFO_LOGF(" Version: %u.%u", headerData->vMajor, headerData->vMinor);
  INFO_LOGF(" Flags: 0x%08x", headerData->globalFlags);
  INFO_LOGF(" Brightness: %u", headerData->brightness);
  INFO_LOGF(" Num Modes: %u", headerData->numModes);

  ByteStream *modes = new ByteStream[headerData->numModes];
  if (!modes) {
    ERROR_LOG("Failed to initialize modes list");
    return;
  }
  for (uint8_t m = 0; m < headerData->numModes; ++m) {
    ByteStream &mode = modes[m];
    mode.init(76);
    // 76 is the max duo mode size (the slot size)
    uint8_t *ptr = (uint8_t *)mode.rawData();
    uint16_t base;
    // there are 3 modes in the eeprom after the header
    if (m < 3) {
      // 0x1400 is eeprom base
      // 17 is size of duo header
      // 76 is size of each duo mode
      base = 0x1400 + 17 + (m * 76);
    } else {
      // 0xFe00 is the end of flash, 0x200 before
      base = 0xFe00 + ((m - 3) * 76);
    }
    for (uint16_t i = 0; i < mode.rawSize(); ++i) {
      uint16_t addr = base + i;
      // base of eeprom + size of header + mode slot
      stptr_p((const uint8_t *)&addr, 2);
      ptr[i] = ld_b();
    }
  }
  for (uint8_t m = 0; m < headerData->numModes; ++m) {
    ByteStream &mode = modes[m];
    // don't mess with a bad mode
    mode.sanity();
    // now check it's crc
    if (!mode.checkCRC()) {
      INFO_LOGF("ERROR Mode %u CRC Invalid!", m);
      continue;
    }
    // NOTE: this is a bit of a hack, this Mode object has 20 leds because it's for
    //       a chromadeck, but we will just use the first 2 and disregard the mode->ledCount
    Mode newMode;
    newMode.unserialize(mode);
    INFO_LOGF("Mode %u:", m);
    INFO_LOGF(" Flags: 0x%08x", newMode.getFlags());
    INFO_LOG(" Patterns:");
    for (uint8_t p = 0; p < 2; ++p) {
      Pattern *pat = newMode.getPattern((LedPos)p);
      INFO_LOGF("  Pattern %u:", p);
      INFO_LOGF("   Pattern ID: %u", pat->getPatternID());
      PatternArgs args;
      pat->getArgs(args);
      // should only be 5 or 7 args on duo
      if (pat->getNumArgs() == 5) {
        INFO_LOGF("   Params: %u %u %u %u %u", args.arg1, args.arg2, args.arg3, args.arg4, args.arg5);
      } else if (pat->getNumArgs() == 7) {
        INFO_LOGF("   Params: %u %u %u %u %u %u %u", args.arg1, args.arg2, args.arg3, args.arg4, args.arg5, args.arg6, args.arg7);
      } else {
        // otherwise just log all 8
        INFO_LOGF("   Params: %u %u %u %u %u %u %u %u", args.arg1, args.arg2, args.arg3, args.arg4, args.arg5, args.arg6, args.arg7, args.arg8);
      }
      Colorset set = pat->getColorset();
      switch (set.numColors()) {
      case 0:
        INFO_LOG("   Colors: none");
        break;
      case 1:
        INFO_LOGF("   Colors: %06X", set.get(0).raw());
        break;
      case 2:
        INFO_LOGF("   Colors: %06X, %06X", set.get(0).raw(), set.get(1).raw());
        break;
      case 3:
        INFO_LOGF("   Colors: %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw());
        break;
      case 4:
        INFO_LOGF("   Colors: %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw());
        break;
      case 5:
        INFO_LOGF("   Colors: %06X, %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw(), set.get(4).raw());
        break;
      case 6:
        INFO_LOGF("   Colors: %06X, %06X, %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw(), set.get(4).raw(), set.get(5).raw());
        break;
      case 7:
        INFO_LOGF("   Colors: %06X, %06X, %06X, %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw(), set.get(4).raw(), set.get(5).raw(), set.get(6).raw());
        break;
      case 8:
      default:
        INFO_LOGF("   Colors: %06X, %06X, %06X, %06X, %06X, %06X, %06X, %06X", set.get(0).raw(), set.get(1).raw(), set.get(2).raw(), set.get(3).raw(), set.get(4).raw(), set.get(5).raw(), set.get(6).raw(), set.get(7).raw());
        break;
      }
    }
  }
  reset();
}

void UPDI::writeMemory()
{
}

void UPDI::enterProgrammingMode()
{
  INFO_LOG("Entering programming mode");
  uint8_t buf[256] = { 0 };
  memset(buf, 0, sizeof(buf));
  // As per datasheet:
  //  This selects the guard time value that will be used by the UPDI when the
  //  transmission direction switches from RX to TX.
  //  0x6 = UPDI guard time: 2 cycles
  stcs(Control_A, 0x6);
  uint8_t mode = 0;
  mode = cpu_mode<0xEF>();
  if (mode != 0x82 && mode != 0x21 && mode != 0xA2) {
    sendDoubleBreak();
    uint8_t status = ldcs(Status_B);
    INFO_LOGF("Bad CPU Mode 0x%02x... error: 0x%02x", mode, status);
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
    INFO_LOG("Timed out waiting for start bit");
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
    //INFO_LOG("Stop bit error");
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
