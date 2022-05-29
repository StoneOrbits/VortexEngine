#include "MultiLedPattern.h"

MultiLedPattern::MultiLedPattern() :
  Pattern()
{
  // this is a multi led pattern
  m_patternFlags |= PATTERN_FLAG_MULTI;
}

MultiLedPattern::~MultiLedPattern()
{
}

void MultiLedPattern::bind(const Colorset *set)
{
  Pattern::bind(set, LED_COUNT);
}

// init the pattern to initial state
void MultiLedPattern::init()
{
  Pattern::init();
}
