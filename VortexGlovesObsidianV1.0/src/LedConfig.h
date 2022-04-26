#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include <inttypes.h>

// Defined the LED positions, their order, and index
enum LedPos : uint32_t
{
  // this should always be first
  LED_FIRST = 0,

  // the first should be equal to LED_FIRST
  THUMB_TOP = LED_FIRST,
  THUMB_TIP,

  INDEX_TOP,
  INDEX_TIP,

  MIDDLE_TOP,
  MIDDLE_TIP,

  RING_TOP,
  RING_TIP,

  PINKIE_TOP,
  PINKIE_TIP,

  // INSERT NEW ENTRIES HERE

  // the number of entries above
  LED_COUNT,

  // the last LED index
  LED_LAST = (LED_COUNT - 1)
};

// Finger enum starting from index because all menus start
// from pinkie so this makes iteration and access easier
enum Finger : uint32_t
{
  FINGER_FIRST = 0,

  FINGER_THUMB = FINGER_FIRST, // proof thumb is finger confirmed
  FINGER_INDEX,
  FINGER_MIDDLE,
  FINGER_RING,
  FINGER_PINKIE,

  FINGER_COUNT, // 5
  FINGER_LAST = (FINGER_COUNT - 1),
};

// get the led index for the top/tip of a finger
inline LedPos fingerTop(Finger finger) {
  return (LedPos)((uint32_t)finger * 2);
}
inline LedPos fingerTip(Finger finger) {
  return (LedPos)(((uint32_t)finger * 2) + 1);
}

// convert an led position to a finger
inline Finger ledToFinger(LedPos pos) {
  // have to flip the index
  return (Finger)(FINGER_THUMB - (Finger)((uint32_t)pos / 2));
}

// LedPos operators
inline LedPos& operator++(LedPos &c) {
  c = LedPos(((uint32_t)c) + 1);
  return c;
}
inline LedPos operator++(LedPos &c, int) {
  LedPos temp = c;
  ++c;
  return temp;
}
inline LedPos operator+(LedPos &c, int b) {
  return (LedPos)((uint32_t)c + b);
}
inline LedPos operator-(LedPos &c, int b) {
  return (LedPos)((uint32_t)c - b);
}

// finger operators
inline Finger& operator++(Finger &c) {
  c = Finger(((uint32_t)c) + 1);
  return c;
}
inline Finger operator++(Finger &c, int) {
  Finger temp = c;
  ++c;
  return temp;
}
inline Finger operator+(Finger &c, int b) {
  return (Finger)((uint32_t)c + b);
}
inline Finger operator-(Finger &c, int b) {
  return (Finger)((uint32_t)c - b);
}
#endif
