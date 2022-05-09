#include "MultiLedPattern.h"

MultiLedPattern::MultiLedPattern() :
  Pattern()
{
}

MultiLedPattern::~MultiLedPattern()
{
}

// init the pattern to initial state
void MultiLedPattern::init(Colorset *colorset, LedPos pos)
{
  Pattern::init(colorset, pos);
}

// pure virtual must override the play function
void MultiLedPattern::play()
{
}

// must override the serialize routine to save the pattern
void MultiLedPattern::serialize() const
{
}

// must override unserialize to load patterns
void MultiLedPattern::unserialize()
{
}
