#include "HueShiftPattern.h"

#include "../../TimeControl.h"
#include "../../Leds.h"

HueShiftPattern::HueShiftPattern(uint8_t scale, uint8_t speed) :
  MultiLedPattern(),
  m_speed(speed),
  m_scale(scale)
{
}

HueShiftPattern::~HueShiftPattern()
{
}

// init the pattern to initial state
void HueShiftPattern::init()
{
}

// pure virtual must override the play function
void HueShiftPattern::play()
{
  uint64_t start_time = Time::getCurtime() / m_speed;
  uint32_t start_hue = (uint32_t)(start_time % 255);
  HSVColor hsv(0, 255, 255);
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    hsv.hue = start_hue + (pos * (255 / (LED_COUNT / m_scale)));
    Leds::setIndex(pos, hsv);
  }
}
