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