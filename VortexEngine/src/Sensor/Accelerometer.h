#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "../VortexConfig.h"

#if ACCELEROMETER_ENABLE == 1

#include <inttypes.h>

class VortexEngine;

class Accelerometer
{
public:

  Accelerometer(VortexEngine &engine);

  struct Sample
  {
    int32_t x_mg;
    int32_t y_mg;
    int32_t z_mg;
  };

  // Lifecycle
  bool init();
  bool available();
  bool setRange(uint8_t g);
  void cleanup();

  // Sampling & Processing
  bool sample();  // read raw sensor (rate-limited)
  bool update();  // compute motion, vectors, tilt

  // Raw sensor data
  // Returns the last sampled raw accelerometer values (in mg)
  const Sample &raw() { return m_lastSample; }

  // Overall magnitude of linear acceleration (gravity removed).
  // Unit: mg (milli-g, 1 g = 1000 mg)
  // Range: 0 to sensor-dependent maximum (e.g., ~2000 mg for quick hand swings)
  // Purpose: Represents how strongly the device is accelerating in 3D space
  //          ignoring gravity, useful for detecting motion intensity.
  float motion() { return m_motion; }

  // Normalized motion [0..1], scaled relative to ~1000 mg threshold.
  // Unitless (dimensionless)
  // Range: 0.0 to 1.0
  // Purpose: Provides a convenient scale for visual effects,
  //          where 0 = no motion, 1 = strong motion. Values above 1.0
  //          are clamped to 1.0.
  float normalized() { return m_normalized; }

  // Quadratically curved version of normalized motion.
  // Unitless
  // Range: 0.0 to 1.0
  // Purpose: Enhances sensitivity to small motions while preserving
  //          strong movements. Small gestures produce smaller outputs,
  //          giving a smoother, non-linear response for effects.
  float curved() { return m_curved; }

  // Smoothed (low-pass filtered) version of curved motion.
  // Unitless
  // Range: 0.0 to 1.0
  // Purpose: Reduces jitter from sensor noise and sudden spikes,
  //          producing a more stable signal suitable for LEDs, audio,
  //          or other outputs that need smooth motion representation.
  float filtered() { return m_filtered; }

  // Linear acceleration
  // Gravity-compensated acceleration along each axis (mg)
  float linX() { return m_linX; }
  float linY() { return m_linY; }
  float linZ() { return m_linZ; }

  // Direction vector
  // Unit vector representing the direction of the last measured acceleration
  float dirX() { return m_dirX; }
  float dirY() { return m_dirY; }
  float dirZ() { return m_dirZ; }

  // Tilt / orientation
  // Pitch: rotation around the X-axis (radians)
  float pitch() { return m_pitch; }
  // Roll: rotation around the Y-axis (radians)
  float roll() { return m_roll; }
  // Overall tilt magnitude in X-Y plane (radians)
  float tilt() { return m_tilt; }

private:

#ifdef VORTEX_EMBEDDED
  // Hardware helpers
  bool findDevice();
  bool configure();
  bool writeReg(uint8_t reg, uint8_t val);
  bool readReg(uint8_t reg, uint8_t &out);
  bool readRegs(uint8_t startReg, uint8_t *buf, uint8_t len);
  float mgPerDigit(uint8_t ctrl4);
  int16_t le16(uint8_t lo, uint8_t hi);

  uint8_t m_address;
#endif

  // Internal state
  bool m_initialized;
  Sample m_lastSample;

  // Gravity estimate
  float m_gravX;
  float m_gravY;
  float m_gravZ;

  // Linear acceleration (gravity removed)
  float m_linX;
  float m_linY;
  float m_linZ;

  // Motion metrics
  float m_motion;
  float m_normalized;
  float m_curved;
  float m_filtered;

  // Direction vector
  float m_dirX;
  float m_dirY;
  float m_dirZ;

  // Tilt
  float m_pitch;
  float m_roll;
  float m_tilt;

  // Built-in sample interval (ms)
  uint32_t m_lastSampleTime;
};
#endif

#endif
