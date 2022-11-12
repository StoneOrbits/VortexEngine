#include "HueShiftPattern.h"

#include "../../SerialBuffer.h"
#include "../../TimeControl.h"
#include "../../Leds.h"
#include "../../Log.h"

HueShiftPattern::HueShiftPattern(uint8_t speed, uint8_t scale) :
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
  MultiLedPattern::init();
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

// must override the serialize routine to save the pattern
void HueShiftPattern::serialize(SerialBuffer &buffer) const
{
  MultiLedPattern::serialize(buffer);
  buffer.serialize(m_speed);
  buffer.serialize(m_scale);
}

void HueShiftPattern::unserialize(SerialBuffer &buffer)
{
  MultiLedPattern::unserialize(buffer);
  buffer.unserialize(&m_speed);
  buffer.unserialize(&m_scale);
}

#ifdef TEST_FRAMEWORK
void HueShiftPattern::saveTemplate(int level) const
{
  MultiLedPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"Speed\": %d,", m_speed);
  IndentMsg(level + 1, "\"Scale\": %d,", m_scale);
}
#endif
