#include "SingleLedPattern.h"

#include "../../Time/TimeControl.h"

SingleLedPattern::SingleLedPattern(const PatternArgs &args) :
  Pattern(args)
{
  // SingleLed is an abstract class it cannot be directly
  // instantiated so we do not need to assign a pattern id
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
