#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include <inttypes.h>

// Defined the LED positions, their order, and index
enum LedPos : uint8_t
{
  // this should always be first
  LED_FIRST = 0,

  // the first should be equal to LED_FIRST
  PINKIE_TIP = LED_FIRST,
  PINKIE_TOP,

  RING_TIP,
  RING_TOP,

  MIDDLE_TIP,
  MIDDLE_TOP,

  INDEX_TIP,
  INDEX_TOP,

  THUMB_TIP,
  THUMB_TOP,

  // INSERT NEW ENTRIES HERE
  //PALM_TIP,
  //PALM_TOP,

  // the number of entries above
  LED_COUNT,

  // the last LED index
  LED_LAST = (LED_COUNT - 1)
};

enum Finger : uint8_t
{
  FINGER_FIRST = 0,

  FINGER_PINKIE = FINGER_FIRST,
  FINGER_RING,
  FINGER_MIDDLE,
  FINGER_INDEX,
  FINGER_THUMB, // proof thumb is finger confirmed

  FINGER_COUNT, // 5
  FINGER_LAST = (FINGER_COUNT - 1),
};

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
  return (Finger)(FINGER_THUMB - (Finger)((uint32_t)pos / 2));
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
inline LedPos& operator+=(LedPos &c, int b)
{
  c = LedPos(((uint32_t)c) + b);
  return c;
}
inline LedPos operator-(LedPos &c, int b)
{
  return (LedPos)((uint32_t)c - b);
}
inline LedPos& operator-=(LedPos &c, int b)
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
