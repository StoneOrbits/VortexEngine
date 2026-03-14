#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "../VortexConfig.h"

#if ACCELEROMETER_ENABLE == 1

#include <inttypes.h>

class Accelerometer
{
public:

  struct Sample
  {
    int32_t x_mg;
    int32_t y_mg;
    int32_t z_mg;
  };

  // Lifecycle
  static bool init();
  static bool available();
  static bool setRange(uint8_t g);
  static void cleanup();

  // Sampling & Processing
  static bool sample();  // read raw sensor (rate-limited)
  static bool update();  // compute motion, vectors, tilt

  // Raw sensor data
  // Returns the last sampled raw accelerometer values (in mg)
  static const Sample &raw() { return m_lastSample; }

  // Overall magnitude of linear acceleration (gravity removed).
  // Unit: mg (milli-g, 1 g = 1000 mg)
  // Range: 0 to sensor-dependent maximum (e.g., ~2000 mg for quick hand swings)
  // Purpose: Represents how strongly the device is accelerating in 3D space
  //          ignoring gravity, useful for detecting motion intensity.
  static float motion() { return m_motion; }

  // Normalized motion [0..1], scaled relative to ~1000 mg threshold.
  // Unitless (dimensionless)
  // Range: 0.0 to 1.0
  // Purpose: Provides a convenient scale for visual effects,
  //          where 0 = no motion, 1 = strong motion. Values above 1.0
  //          are clamped to 1.0.
  static float normalized() { return m_normalized; }

  // Quadratically curved version of normalized motion.
  // Unitless
  // Range: 0.0 to 1.0
  // Purpose: Enhances sensitivity to small motions while preserving
  //          strong movements. Small gestures produce smaller outputs,
  //          giving a smoother, non-linear response for effects.
  static float curved() { return m_curved; }

  // Smoothed (low-pass filtered) version of curved motion.
  // Unitless
  // Range: 0.0 to 1.0
  // Purpose: Reduces jitter from sensor noise and sudden spikes,
  //          producing a more stable signal suitable for LEDs, audio,
  //          or other outputs that need smooth motion representation.
  static float filtered() { return m_filtered; }

  // Linear acceleration
  // Gravity-compensated acceleration along each axis (mg)
  static float linX() { return m_linX; }
  static float linY() { return m_linY; }
  static float linZ() { return m_linZ; }

  // Direction vector
  // Unit vector representing the direction of the last measured acceleration
  static float dirX() { return m_dirX; }
  static float dirY() { return m_dirY; }
  static float dirZ() { return m_dirZ; }

  // Tilt / orientation
  // Pitch: rotation around the X-axis (radians)
  static float pitch() { return m_pitch; }
  // Roll: rotation around the Y-axis (radians)
  static float roll() { return m_roll; }
  // Overall tilt magnitude in X-Y plane (radians)
  static float tilt() { return m_tilt; }

private:

#ifdef VORTEX_EMBEDDED
  // Hardware helpers
  static bool findDevice();
  static bool configure();
  static bool writeReg(uint8_t reg, uint8_t val);
  static bool readReg(uint8_t reg, uint8_t &out);
  static bool readRegs(uint8_t startReg, uint8_t *buf, uint8_t len);
  static float mgPerDigit(uint8_t ctrl4);
  static int16_t le16(uint8_t lo, uint8_t hi);

  static uint8_t m_address;
#endif

  // Internal state
  static bool m_initialized;
  static Sample m_lastSample;

  // Gravity estimate
  static float m_gravX;
  static float m_gravY;
  static float m_gravZ;

  // Linear acceleration (gravity removed)
  static float m_linX;
  static float m_linY;
  static float m_linZ;

  // Motion metrics
  static float m_motion;
  static float m_normalized;
  static float m_curved;
  static float m_filtered;

  // Direction vector
  static float m_dirX;
  static float m_dirY;
  static float m_dirZ;

  // Tilt
  static float m_pitch;
  static float m_roll;
  static float m_tilt;

  // Built-in sample interval (ms)
  static uint32_t m_lastSampleTime;
};
#endif

#endif
