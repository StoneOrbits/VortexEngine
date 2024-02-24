#ifndef UPDI_FLASHER_H
#define UPDI_FLASHER_H

#include "../VortexConfig.h"

#ifdef VORTEX_EMBEDDED

#include "FastGPIO.h"
#include <vector>

class UPDIFlasher
{
public:
  static bool init();
  static void readSIB();

private:
  static void sendBreak();
  static void sendSynch();
  static void sendUPDICommand(const std::vector<uint8_t> &command);
  static std::vector<uint8_t> readBytes(size_t count);
  static void writeByte(uint8_t byte);
  static uint8_t readByte();
  static void writeBit(bool bit);
  static unsigned int calculateBitDuration();
};

#endif
#endif
