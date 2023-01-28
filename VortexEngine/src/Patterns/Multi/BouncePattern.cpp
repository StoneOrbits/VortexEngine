#include "BouncePattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#define TOTAL_STEPS ((RING_COUNT * 2))
#define HALF_STEPS (TOTAL_STEPS / 2)

BouncePattern::BouncePattern(int8_t onDuration, uint8_t offDuration, uint8_t stepDuration, uint8_t fadeAmount) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_progress(0),
  m_fadeAmount(fadeAmount),
  m_stash()
{
  m_patternID = PATTERN_BOUNCE;
}

BouncePattern::BouncePattern(const PatternArgs &args) :
  BouncePattern()
{
  setArgs(args);
}

BouncePattern::~BouncePattern()
{
}

// init the pattern to initial state
void BouncePattern::init()
{
  BlinkStepPattern::init();
  // reset progress
  m_progress = 0;
  // start colorset at index 0 so cur() works
  m_colorset.setCurIndex(0);
}

void BouncePattern::blinkOn()
{
  for (int i = 0; i < LED_COUNT; ++i) {
    m_stash[i].adjustBrightness(m_fadeAmount);
  }
  Leds::restoreAll(m_stash);
  //Leds::setAll(m_colorset.cur());
  if (m_progress < RING_COUNT) {
    Leds::setRing((Ring)m_progress, m_colorset.cur());
  } else {
    Leds::setRing((Ring)(TOTAL_STEPS - (m_progress + 1)), m_colorset.cur());
  }
  Leds::stashAll(m_stash);
}

void BouncePattern::poststep()
{
  m_progress = (m_progress + 1) % TOTAL_STEPS;
  if (m_progress == 0 || m_progress == HALF_STEPS) {
    m_colorset.getNext();
  }
}

void BouncePattern::setArgs(const PatternArgs &args)
{
  BlinkStepPattern::setArgs(args);
  m_fadeAmount = args.arg4;
}

void BouncePattern::getArgs(PatternArgs &args) const
{
  BlinkStepPattern::getArgs(args);
  args.arg4 = m_fadeAmount;
  args.numArgs += 1;
}
