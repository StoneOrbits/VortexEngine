#include "TheaterChasePattern.h"

#include "../../Leds/Leds.h"

#define THEATER_CHASE_STEPS 3

TheaterChasePattern::TheaterChasePattern(uint8_t onDuration, uint8_t offDuration, uint8_t stepDuration) :
  BlinkStepPattern(onDuration, offDuration, stepDuration),
  m_stepCounter(0)
{
  m_patternID = PATTERN_THEATER_CHASE;
}

TheaterChasePattern::TheaterChasePattern(const PatternArgs &args) :
  TheaterChasePattern()
{
  setArgs(args);
}

TheaterChasePattern::~TheaterChasePattern()
{
}

void TheaterChasePattern::init()
{
  BlinkStepPattern::init();
  // starts on odd tips
  m_stepCounter = 0;
}

void TheaterChasePattern::blinkOn()
{
  for (LedPos i = LED_FIRST; i < LED_COUNT; ++i) {
    if ((i % 3) == m_stepCounter) {
      Leds::setIndex(i, m_colorset.cur());
    }
  }
}

void TheaterChasePattern::poststep()
{
  // increment step counter
  m_stepCounter = (m_stepCounter + 1) % THEATER_CHASE_STEPS;
  m_colorset.getNext();
}
