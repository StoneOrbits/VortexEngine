#include "SingleLedPattern.h"

#include "../../TimeControl.h"

SingleLedPattern::SingleLedPattern() :
  Pattern()
{
}

SingleLedPattern::~SingleLedPattern()
{
}

void SingleLedPattern::init()
{
  Pattern::init();
}

// NOTE: this isn't working correctly because of some issues, need to fix
void SingleLedPattern::skip(uint32_t ticks)
{
  Time::startSimulation();
  for (uint32_t i = 0; i < ticks; ++i) {
    play();  // simulate playing the pattern
    Time::tickSimulation();
  }
  Time::endSimulation();
}
