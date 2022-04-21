#ifndef LED_CONFIG_H
#define LED_CONFIG_H

// Defined the LED positions, their order, and index
typedef enum ledPositionEnum {

  THUMB_TOP = 0,
  THUMB_TIP,

  INDEX_TOP,
  INDEX_TIP,

  MIDDLE_TOP,
  MIDDLE_TIP,

  RING_TOP,
  RING_TIP,

  PINKIE_TOP,
  PINKIE_TIP,

  // the number of entries above
  LED_COUNT

} LedPos;

#endif
