#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include <inttypes.h>
#include <stdarg.h>

#include "../VortexConfig.h"

// Defined the LED positions, their order, and index
enum LedPos : uint8_t
{
  // this should always be first
  LED_FIRST = 0,

  LED_0 = LED_FIRST,
  LED_1,

  // the number of entries above
  LED_COUNT,

  // the last LED index
  LED_LAST = (LED_COUNT - 1)
};

enum Finger : uint8_t
{
  FINGER_FIRST = 0,

  FINGER_COUNT, // 5
  FINGER_LAST = (FINGER_COUNT - 1),
};

// check if an led is finger tip or top
inline bool isFingerTip(LedPos pos)
{
  return (pos % 2) == 0;
}
inline bool isFingerTop(LedPos pos)
{
  return (pos % 2) != 0;
}

// get the led index for the tip/top of a finger
inline LedPos fingerTip(Finger finger)
{
  return (LedPos)((uint32_t)finger * 2);
}
inline LedPos fingerTop(Finger finger)
{
  return (LedPos)(((uint32_t)finger * 2) + 1);
}

// convert an led position to a finger
inline Finger ledToFinger(LedPos pos)
{
  // have to flip the index
  return FINGER_FIRST;
}

// LedPos operators
inline LedPos &operator++(LedPos &c)
{
  c = LedPos(((uint32_t)c) + 1);
  return c;
}
inline LedPos operator++(LedPos &c, int)
{
  LedPos temp = c;
  ++c;
  return temp;
}
inline LedPos operator+(LedPos &c, int b)
{
  return (LedPos)((uint32_t)c + b);
}
inline LedPos &operator+=(LedPos &c, int b)
{
  c = LedPos(((uint32_t)c) + b);
  return c;
}
inline LedPos operator-(LedPos &c, int b)
{
  return (LedPos)((uint32_t)c - b);
}
inline LedPos &operator-=(LedPos &c, int b)
{
  c = LedPos(((uint32_t)c) - b);
  return c;
}

// finger operators
inline Finger &operator++(Finger &c)
{
  c = Finger(((uint32_t)c) + 1);
  return c;
}
inline Finger operator++(Finger &c, int)
{
  Finger temp = c;
  ++c;
  return temp;
}
inline Finger operator+(Finger &c, int b)
{
  return (Finger)((uint32_t)c + b);
}
inline Finger operator-(Finger &c, int b)
{
  return (Finger)((uint32_t)c - b);
}
#endif
