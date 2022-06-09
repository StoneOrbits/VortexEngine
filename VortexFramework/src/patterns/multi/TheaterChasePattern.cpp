#include "TheaterChasePattern.h"

#include "../single/SingleLedPattern.h"
#include "MultiLedPattern.h"

#include "../../PatternBuilder.h"
#include "../../Colorset.h"
#include "../../Log.h"

TheaterChasePattern::TheaterChasePattern() :
  MultiLedPattern()
{
}

TheaterChasePattern::~TheaterChasePattern()
{
}

void TheaterChasePattern::init()
{
  MultiLedPattern::init();
}

void TheaterChasePattern::play()
{
  MultiLedPattern::play();
}

void TheaterChasePattern::serialize(SerialBuffer &buffer) const
{
  //DEBUG_LOG("Serialize");
  MultiLedPattern::serialize(buffer);
}

// must override unserialize to load patterns
void TheaterChasePattern::unserialize(SerialBuffer &buffer)
{
  //DEBUG_LOG("Unserialize");
  MultiLedPattern::unserialize(buffer);
}