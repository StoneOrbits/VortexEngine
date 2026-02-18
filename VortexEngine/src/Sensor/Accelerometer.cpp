#include "Accelerometer.h"

#ifdef VORTEX_EMBEDDED
#include <Wire.h>
#include <Arduino.h>

// Pin Definitons
#define ACCEL_SDA_PIN 2
#define ACCEL_SCL_PIN 3
#define ACCEL_INT1_PIN 6
#define ACCEL_INT2_PIN 7

// various constants for accel control
#define REG_WHO_AM_I 0x0F
#define REG_CTRL1    0x20
#define REG_CTRL4    0x23
#define REG_STATUS   0x27
#define REG_OUT_X_L  0x28
#define WHO_AM_I_EXPECTED 0x11

uint8_t Accelerometer::m_address = 0;
#endif

bool Accelerometer::m_initialized = false;

Accelerometer::Accelerometer()
{
}

bool Accelerometer::init()
{
#ifdef VORTEX_EMBEDDED

  Wire.begin(ACCEL_SDA_PIN, ACCEL_SCL_PIN);
  Wire.setClock(400000);

  pinMode(ACCEL_INT1_PIN, INPUT);
  pinMode(ACCEL_INT2_PIN, INPUT);

  if (!findDevice())
    return false;

  if (!configure())
    return false;

  m_initialized = true;
  return true;

#else
  m_initialized = false;
  return false;
#endif
}

bool Accelerometer::available()
{
  return m_initialized;
}

bool Accelerometer::read(Sample &out)
{
#ifdef VORTEX_EMBEDDED

  if (!m_initialized)
    return false;

  uint8_t status = 0;
  if (!readReg(REG_STATUS, status))
    return false;

  if ((status & 0x08) == 0)
    return false;

  uint8_t ctrl4 = 0;
  if (!readReg(REG_CTRL4, ctrl4))
    return false;

  float mgDigit = mgPerDigit(ctrl4);

  uint8_t buf[6];

  if (!readRegs(REG_OUT_X_L | 0x80, buf, 6))
    return false;

  int16_t rawX = (int16_t)(le16(buf[0], buf[1]) >> 4);
  int16_t rawY = (int16_t)(le16(buf[2], buf[3]) >> 4);
  int16_t rawZ = (int16_t)(le16(buf[4], buf[5]) >> 4);

  out.x_mg = (int32_t)((float)rawX * mgDigit);
  out.y_mg = (int32_t)((float)rawY * mgDigit);
  out.z_mg = (int32_t)((float)rawZ * mgDigit);

  return true;

#else
  (void)out;
  return false;
#endif
}

bool Accelerometer::setRange(uint8_t g)
{
#ifdef VORTEX_EMBEDDED

  uint8_t fsBits = 0;

  switch (g)
  {
    case 2:  fsBits = 0; break;
    case 4:  fsBits = 1; break;
    case 8:  fsBits = 2; break;
    case 16: fsBits = 3; break;
    default: return false;
  }

  uint8_t ctrl4 = 0;
  if (!readReg(REG_CTRL4, ctrl4))
    return false;

  ctrl4 &= ~(0x3 << 4);
  ctrl4 |= (fsBits << 4);

  return writeReg(REG_CTRL4, ctrl4);

#else
  (void)g;
  return false;
#endif
}

void Accelerometer::shutdown()
{
#ifdef VORTEX_EMBEDDED
  writeReg(REG_CTRL1, 0x00);
#endif
  m_initialized = false;
}

#ifdef VORTEX_EMBEDDED

bool Accelerometer::findDevice()
{
  for (uint8_t a = 0x18; a <= 0x1F; a++)
  {
    uint8_t who = 0;

    Wire.beginTransmission(a);
    Wire.write(REG_WHO_AM_I);

    if (Wire.endTransmission(false) != 0)
      continue;

    if (Wire.requestFrom((int)a, 1) != 1)
      continue;

    who = (uint8_t)Wire.read();

    if (who == WHO_AM_I_EXPECTED)
    {
      m_address = a;
      return true;
    }
  }

  return false;
}

bool Accelerometer::configure()
{
  if (!writeReg(REG_CTRL1, 0x57))
    return false;

  if (!writeReg(REG_CTRL4, 0x88))
    return false;

  return true;
}

bool Accelerometer::writeReg(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(m_address);
  Wire.write(reg);
  Wire.write(val);
  return (Wire.endTransmission() == 0);
}

bool Accelerometer::readReg(uint8_t reg, uint8_t &out)
{
  Wire.beginTransmission(m_address);
  Wire.write(reg);

  if (Wire.endTransmission(false) != 0)
    return false;

  if (Wire.requestFrom((int)m_address, 1) != 1)
    return false;

  out = (uint8_t)Wire.read();
  return true;
}

bool Accelerometer::readRegs(uint8_t startReg, uint8_t *buf, uint8_t len)
{
  Wire.beginTransmission(m_address);
  Wire.write(startReg);

  if (Wire.endTransmission(false) != 0)
    return false;

  if (Wire.requestFrom((int)m_address, (int)len) != len)
    return false;

  for (uint8_t i = 0; i < len; i++)
    buf[i] = (uint8_t)Wire.read();

  return true;
}

float Accelerometer::mgPerDigit(uint8_t ctrl4)
{
  uint8_t fs = (ctrl4 >> 4) & 0x03;

  switch (fs)
  {
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
