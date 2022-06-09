#include "TheaterChasePattern.h"

#include "MultiLedPattern.h"

#include "../../TimeControl.h"
#include "../../Colorset.h"
#include "../../Leds.h"
#include "../../Log.h"

TheaterChasePattern::TheaterChasePattern() :
  MultiLedPattern(),
  m_oneHundy(false),
  m_twentyThree(false)
{
}

TheaterChasePattern::~TheaterChasePattern()
{
}

void TheaterChasePattern::init()
{
  MultiLedPattern::init();
  m_oneHundy = false;
  m_twentyThree = false;
}

void TheaterChasePattern::play()
{
  // DO THE STUFF
  if ((Time::getCurtime() % 100) == 0) {
    m_oneHundy = !m_oneHundy;
  }
  if ((Time::getCurtime() % 23) == 0) {
    m_twentyThree = !m_twentyThree;
  }
  // if m_twentyThree...
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