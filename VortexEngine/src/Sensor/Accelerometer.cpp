#include "Accelerometer.h"

#if ACCELEROMETER_ENABLE == 1
#include <math.h>

#ifdef VORTEX_EMBEDDED
#include "../Time/TimeControl.h"

#include <Arduino.h>
#include <Wire.h>

// Pin definitions
#define ACCEL_SDA_PIN 2
#define ACCEL_SCL_PIN 3
#define ACCEL_INT1_PIN 6
#define ACCEL_INT2_PIN 7

// Registers
#define REG_WHO_AM_I 0x0F
#define REG_CTRL1    0x20
#define REG_CTRL4    0x23
#define REG_STATUS   0x27
#define REG_OUT_X_L  0x28
#define WHO_AM_I_EXPECTED 0x11

// Sampling Interval
//
// This is the rate at which the accelerometer can be sampled, ie the number of
// ticks that must pass before the accel can be sampled again
#define SAMPLE_INTERVAL_MS 1

uint8_t Accelerometer::m_address = 0;
#endif

// State
bool Accelerometer::m_initialized = false;
Accelerometer::Sample Accelerometer::m_lastSample = { 0,0,0 };

float Accelerometer::m_gravX = 0.0f;
float Accelerometer::m_gravY = 0.0f;
float Accelerometer::m_gravZ = 1.0f;

float Accelerometer::m_linX = 0.0f;
float Accelerometer::m_linY = 0.0f;
float Accelerometer::m_linZ = 0.0f;

float Accelerometer::m_motion = 0.0f;
float Accelerometer::m_normalized = 0.0f;
float Accelerometer::m_curved = 0.0f;
float Accelerometer::m_filtered = 0.0f;

float Accelerometer::m_dirX = 0.0f;
float Accelerometer::m_dirY = 0.0f;
float Accelerometer::m_dirZ = 1.0f;

float Accelerometer::m_pitch = 0.0f;
float Accelerometer::m_roll = 0.0f;
float Accelerometer::m_tilt = 0.0f;

uint32_t Accelerometer::m_lastSampleTime = 0;

bool Accelerometer::init()
{
#ifdef VORTEX_EMBEDDED
  Wire.begin(ACCEL_SDA_PIN, ACCEL_SCL_PIN);
  Wire.setClock(400000);

  pinMode(ACCEL_INT1_PIN, INPUT);
  pinMode(ACCEL_INT2_PIN, INPUT);

  if (!findDevice()) {
    return false;
  }
  if (!configure()) {
    return false;
  }
#endif
  m_initialized = true;
  return true;
}

bool Accelerometer::available()
{
  return m_initialized;
}

bool Accelerometer::setRange(uint8_t g)
{
#ifdef VORTEX_EMBEDDED
  uint8_t fsBits = 0;

  switch (g) {
  case 2:  fsBits = 0; break;
  case 4:  fsBits = 1; break;
  case 8:  fsBits = 2; break;
  case 16: fsBits = 3; break;
  default: return false; // unsupported range
  }

  uint8_t ctrl4 = 0;
  if (!readReg(REG_CTRL4, ctrl4)) {
    return false;
  }

  // clear FS bits and set new range
  ctrl4 &= ~(0x3 << 4);
  ctrl4 |= (fsBits << 4);

  return writeReg(REG_CTRL4, ctrl4);
#else
  (void)g;
  return false;
#endif
}

void Accelerometer::cleanup()
{
#ifdef VORTEX_EMBEDDED
  writeReg(REG_CTRL1, 0x00);
#endif
  m_initialized = false;
}

// SAMPLE FUNCTION
bool Accelerometer::sample()
{
#ifdef VORTEX_EMBEDDED
  if (!m_initialized) {
    return false;
  }

  uint32_t now = Time::getCurtime();
  if ((now - m_lastSampleTime) < SAMPLE_INTERVAL_MS) {
    return false;
  }
  m_lastSampleTime = now;

  uint8_t status = 0;
  if (!readReg(REG_STATUS, status)) {
    return false;
  }
  if ((status & 0x08) == 0) {
    return false;
  }

  uint8_t ctrl4 = 0;
  if (!readReg(REG_CTRL4, ctrl4)) {
    return false;
  }
  float mgDigit = mgPerDigit(ctrl4);

  uint8_t buf[6];
  if (!readRegs(REG_OUT_X_L | 0x80, buf, 6)) {
    return false;
  }

  m_lastSample.x_mg = (int32_t)(le16(buf[0], buf[1]) >> 4) * mgDigit;
  m_lastSample.y_mg = (int32_t)(le16(buf[2], buf[3]) >> 4) * mgDigit;
  m_lastSample.z_mg = (int32_t)(le16(buf[4], buf[5]) >> 4) * mgDigit;

  return true;
#else
  return false;
#endif
}

// Update function to sample the sensor and calculate the values for this tick
bool Accelerometer::update()
{
  // attempt to sample the sensor each update, but it may fail sometimes
  // TODO: Create sampling rate constant to allow configuring sample rate
  sample();

#ifdef VORTEX_EMBEDDED
  // Convert to float
  float fx = (float)m_lastSample.x_mg;
  float fy = (float)m_lastSample.y_mg;
  float fz = (float)m_lastSample.z_mg;

  // Direction vector
  float mag = sqrtf(fx * fx + fy * fy + fz * fz);
  if (mag > 0.0001f) {
    float inv = 1.0f / mag;
    m_dirX = fx * inv;
    m_dirY = fy * inv;
    m_dirZ = fz * inv;
  } else {
    m_dirX = 0.0f;
    m_dirY = 0.0f;
    m_dirZ = 0.0f;
  }

  // Gravity estimation (low-pass)
  const float alpha = 0.98f;
  m_gravX = m_gravX * alpha + fx * (1.0f - alpha);
  m_gravY = m_gravY * alpha + fy * (1.0f - alpha);
  m_gravZ = m_gravZ * alpha + fz * (1.0f - alpha);

  // Linear acceleration
  m_linX = fx - m_gravX;
  m_linY = fy - m_gravY;
  m_linZ = fz - m_gravZ;

  // Motion magnitude
  m_motion = sqrtf(m_linX * m_linX + m_linY * m_linY + m_linZ * m_linZ);
  m_normalized = m_motion / 1000.0f;
  if (m_normalized > 1.0f) m_normalized = 1.0f;
  m_curved = m_normalized * m_normalized;

  // Low-pass filter
  m_filtered = m_filtered * 0.85f + m_curved * 0.15f;

  // Tilt calculation
  float gMag = sqrtf(m_gravX * m_gravX + m_gravY * m_gravY + m_gravZ * m_gravZ);
  if (gMag > 0.0001f) {
    float nx = m_gravX / gMag;
    float ny = m_gravY / gMag;
    float nz = m_gravZ / gMag;
    m_roll = atan2f(ny, nz);
    m_pitch = atan2f(-nx, sqrtf(ny * ny + nz * nz));
    m_tilt = sqrtf(nx * nx + ny * ny);
  } else {
    m_roll = 0.0f;
    m_pitch = 0.0f;
    m_tilt = 0.0f;
  }
#endif
  return true;
}

#ifdef VORTEX_EMBEDDED

// Hardware functions
bool Accelerometer::findDevice()
{
  for (uint8_t a = 0x18; a <= 0x1F; a++) {
    Wire.beginTransmission(a);
    Wire.write(REG_WHO_AM_I);
    if (Wire.endTransmission(false) != 0) continue;
    if (Wire.requestFrom((int)a, 1) != 1) continue;
    if (Wire.read() == WHO_AM_I_EXPECTED) {
      m_address = a;
      return true;
    }
  }
  return false;
}

bool Accelerometer::configure()
{
  if (!writeReg(REG_CTRL1, 0x57)) {
    return false;
  }
  if (!writeReg(REG_CTRL4, 0x88)) {
    return false;
  }
  return true;
}

bool Accelerometer::writeReg(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(m_address);
  Wire.write(reg);
  Wire.write(val);
  return (Wire.endTransmission() == 0);
}

bool Accelerometer::readReg(uint8_t reg, uint8_t & out)
{
  Wire.beginTransmission(m_address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom((int)m_address, 1) != 1) {
    return false;
  }
  out = Wire.read();
  return true;
}

bool Accelerometer::readRegs(uint8_t startReg, uint8_t * buf, uint8_t len)
{
  Wire.beginTransmission(m_address);
  Wire.write(startReg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom((int)m_address, (int)len) != len) {
    return false;
  }
  for (uint8_t i = 0; i < len; i++) buf[i] = Wire.read();
  return true;
}

float Accelerometer::mgPerDigit(uint8_t ctrl4)
{
  switch ((ctrl4 >> 4) & 0x03) {
  case 0: return 1.0f;
  case 1: return 2.0f;
  case 2: return 4.0f;
  case 3: return 8.0f;
  }
  return 1.0f;
}

int16_t Accelerometer::le16(uint8_t lo, uint8_t hi)
{
  return (int16_t)((uint16_t)lo | ((uint16_t)hi << 8));
}

#endif

#endif