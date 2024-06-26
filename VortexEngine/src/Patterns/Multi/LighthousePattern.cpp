#include "LighthousePattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

LighthousePattern::LighthousePattern(const PatternArgs &args) :
  BlinkStepPattern(args),
  m_fadeAmount(0),
  m_fadeRate(0),
  m_fadeTimer(),
  m_stash(),
  m_progress(0)
{
  m_patternID = PATTERN_LIGHTHOUSE;
  REGISTER_ARG(m_fadeAmount);
  REGISTER_ARG(m_fadeRate);
  setArgs(args);
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
  Leds::stashAll(m_stash);
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
  for (int i = 0; i < LED_COUNT; ++i) {
    m_stash[i].adjustBrightness(m_fadeAmount);
  }
}
