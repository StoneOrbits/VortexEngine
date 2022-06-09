#include "TheaterChasePattern.h"

#include "MultiLedPattern.h"

#include "../../Colorset.h"
#include "../../Leds.h"
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
  // DO THE STUFF
  Leds::setFinger(FINGER_FIRST, RGB_RED);
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