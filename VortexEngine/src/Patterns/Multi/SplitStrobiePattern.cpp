#include "SplitStrobiePattern.h"

#include "../PatternBuilder.h"
#include "../../Serial/ByteStream.h"
#include "../../Colors/Colorset.h"
#include "../../Log/Log.h"

SplitStrobiePattern::SplitStrobiePattern(const PatternArgs &args) :
  HybridPattern(args),
  m_stepDuration(0),
  m_stepTimer(),
  m_switch(false),
  m_firstPatternArgs(0, 0, 0),
  m_secondPatternArgs(0, 0)
{
  m_patternID = PATTERN_SPLITSTROBIE;
  REGISTER_ARG(m_firstPatternArgs.arg1);
  REGISTER_ARG(m_firstPatternArgs.arg2);
  REGISTER_ARG(m_firstPatternArgs.arg3);
  REGISTER_ARG(m_secondPatternArgs.arg1);
  REGISTER_ARG(m_secondPatternArgs.arg2);
  REGISTER_ARG(m_stepDuration);
  setArgs(args);
}

SplitStrobiePattern::~SplitStrobiePattern()
{
}

// init the pattern to initial state
void SplitStrobiePattern::init()
{
  HybridPattern::init();

  // reset switch
  m_switch = false;

  // timer for switch
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepDuration * 100);
  m_stepTimer.start();

  // initialize the sub patterns one time first
  setEvensOdds(PATTERN_STROBE, PATTERN_DOPS);
}

void SplitStrobiePattern::play()
{
  if (m_stepTimer.alarm() == 0) {
    // switch which patterns are displayed
    m_switch = !m_switch;
    // update the tip/top patterns based on the switch
    setEvensOdds(m_switch ? PATTERN_DOPS : PATTERN_TRACER,
                m_switch ? PATTERN_TRACER : PATTERN_DOPS,
                m_switch ? &m_firstPatternArgs : &m_secondPatternArgs,
                m_switch ? &m_secondPatternArgs : &m_firstPatternArgs);
  }
  HybridPattern::play();
}
