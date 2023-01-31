#include "SplitStrobiePattern.h"

#include "../PatternBuilder.h"
#include "../../Serial/ByteStream.h"
#include "../../Colors/Colorset.h"
#include "../../Log/Log.h"

SplitStrobiePattern::SplitStrobiePattern(uint8_t onDuration, uint8_t offDuration, uint8_t gapDuration,
  uint8_t dashDuration, uint8_t dotDuration, uint8_t stepDuration100ms) :
  HybridPattern(),
  m_stepDuration(stepDuration100ms),
  m_stepTimer(),
  m_switch(false),
  m_firstPatternArgs(onDuration, offDuration, gapDuration),
  m_secondPatternArgs(dashDuration, dotDuration)
{
  m_patternID = PATTERN_SPLITSTROBIE;
}

SplitStrobiePattern::SplitStrobiePattern(const PatternArgs &args) :
  SplitStrobiePattern()
{
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
  setTipsTops(PATTERN_STROBE, PATTERN_DOPS);
}

void SplitStrobiePattern::play()
{
  if (m_stepTimer.alarm() == 0) {
    // switch which patterns are displayed
    m_switch = !m_switch;
    // update the tip/top patterns based on the switch
    setTipsTops(m_switch ? PATTERN_DOPS : PATTERN_TRACER,
                m_switch ? PATTERN_TRACER : PATTERN_DOPS,
                m_switch ? &m_firstPatternArgs : &m_secondPatternArgs,
                m_switch ? &m_secondPatternArgs : &m_firstPatternArgs);
  }
  HybridPattern::play();
}

void SplitStrobiePattern::setArgs(const PatternArgs &args)
{
  HybridPattern::setArgs(args);
  m_firstPatternArgs.arg1 = args.arg1;
  m_firstPatternArgs.arg2 = args.arg2;
  m_firstPatternArgs.arg3 = args.arg3;
  m_secondPatternArgs.arg1 = args.arg4;
  m_secondPatternArgs.arg2 = args.arg5;
  m_stepDuration = args.arg6;
}

void SplitStrobiePattern::getArgs(PatternArgs &args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_firstPatternArgs.arg1;
  args.arg2 = m_firstPatternArgs.arg2;
  args.arg3 = m_firstPatternArgs.arg3;
  args.arg4 = m_secondPatternArgs.arg1;
  args.arg5 = m_secondPatternArgs.arg2;
  args.arg6 = m_stepDuration;
  args.numArgs += 6;
}
