#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <inttypes.h>

#include "../VortexConfig.h"

class Accelerometer
{
  Accelerometer();

public:

  struct Sample
  {
    int32_t x_mg;
    int32_t y_mg;
    int32_t z_mg;
  };

  static bool init();
  static bool available();
  static bool read(Sample &out);
  static bool setRange(uint8_t g);
  static void shutdown();

private:

#ifdef VORTEX_EMBEDDED
  static bool findDevice();
  static bool configure();

  static bool writeReg(uint8_t reg, uint8_t val);
  static bool readReg(uint8_t reg, uint8_t &out);
  static bool readRegs(uint8_t startReg, uint8_t *buf, uint8_t len);

  static float mgPerDigit(uint8_t ctrl4);
  static int16_t le16(uint8_t lo, uint8_t hi);

  static uint8_t m_address;
#endif

  static bool m_initialized;
};

#endif
