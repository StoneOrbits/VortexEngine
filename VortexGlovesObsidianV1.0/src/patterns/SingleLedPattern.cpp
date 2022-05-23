#include "SingleLedPattern.h"

#include "../TimeControl.h"

SingleLedPattern::SingleLedPattern() :
  Pattern()
{
}

SingleLedPattern::~SingleLedPattern()
{
}

void SingleLedPattern::init(Colorset *colorset, LedPos pos)
{
  Pattern::init(colorset, pos);
  // if the led position is LED_COUNT that means this pattern is
  // a complex pattern that operates on all LEDS. If it's not a
  // complex pattern then the pattern needs to be fast-forwarded
  // based on the finger-tick-offset
  if (pos != LED_COUNT) {
    // skip forward however many ticks this led is offset
    skip(Time::getTickOffset(pos));
  }
  // resume the pattern after skipping it forward, this moves the timer
  // to the current time so that it will start from now with whatever
  // state it ended up in from the simulation above
  resume();
}

void SingleLedPattern::skip(uint32_t ticks)
{
  uint32_t startTime = Time::startSimulation();
  for (int i = 0; i < ticks; ++i) {
    play();  // simulate playing the pattern
    Time::tickSimulation();
  }
  uint32_t endTime = Time::endSimulation();
}

void SingleLedPattern::resume()
{
  // this will be overridden by derived classes
}
