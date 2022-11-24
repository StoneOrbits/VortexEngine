#include "LighthousePattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

LighthousePattern::LighthousePattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration, uint8_t fadeAmount, uint8_t fadeRate) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_fadeAmount(fadeAmount),
  m_fadeRate(fadeRate),
  m_fadeTimer(),
  m_stash(),
  m_progress(0)
{
}

LighthousePattern::~LighthousePattern()
{
}

// init the pattern to initial state
void LighthousePattern::init()
{
  BlinkStepPattern::init();

  // fade timer setup
  m_fadeTimer.reset();
  m_fadeTimer.addAlarm(m_fadeRate);
  m_fadeTimer.start();

  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

// pure virtual must override the play function
void LighthousePattern::play()
{
  if (m_fadeTimer.alarm() == 0) {
    fade();
  }

  BlinkStepPattern::play();
}

void LighthousePattern::blinkOn()
{
  Leds::restoreAll(m_stash);
  Leds::setIndex((LedPos)m_progress, m_colorset.cur());
}

void LighthousePattern::blinkOff()
{
  Leds::stashAll(m_stash);
  Leds::clearAll();
}

void LighthousePattern::poststep()
{
  m_progress = (m_progress + 1) % LED_COUNT;
  m_colorset.getNext();
}

void LighthousePattern::fade()
{
  Leds::adjustBrightnessAll(m_fadeAmount);
}

// must override the serialize routine to save the pattern
void LighthousePattern::serialize(ByteStream& buffer) const
{
  BlinkStepPattern::serialize(buffer);
  buffer.serialize(m_fadeAmount);
  buffer.serialize(m_fadeRate);
}

void LighthousePattern::unserialize(ByteStream& buffer)
{
  BlinkStepPattern::unserialize(buffer);
  buffer.unserialize(&m_fadeAmount);
  buffer.unserialize(&m_fadeRate);
}

#if SAVE_TEMPLATE == 1
void LighthousePattern::saveTemplate(int level) const
{
  BlinkStepPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"FadeAmount\": %d,", m_fadeAmount);
  IndentMsg(level + 1, "\"FadeRate\": %d,", m_fadeRate);
}
#endif


