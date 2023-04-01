#include "BackStrobePattern.h"

BackStrobePattern::BackStrobePattern(const PatternArgs &args) :
  HybridPattern(args),
  m_stepSpeed(0),
  m_stepTimer(),
  m_switch(),
  m_firstPatternArgs(0, 0, 0),
  m_secondPatternArgs(0, 0, 0)
{
  m_patternID = PATTERN_BACKSTROBE;
  REGISTER_ARG(m_firstPatternArgs.arg1);
  REGISTER_ARG(m_firstPatternArgs.arg2);
  REGISTER_ARG(m_firstPatternArgs.arg3);
  REGISTER_ARG(m_secondPatternArgs.arg1);
  REGISTER_ARG(m_secondPatternArgs.arg2);
  REGISTER_ARG(m_secondPatternArgs.arg3);
  REGISTER_ARG(m_stepSpeed);
  setArgs(args);
}

BackStrobePattern::~BackStrobePattern()
{
}

// init the pattern to initial state
void BackStrobePattern::init()
{
  HybridPattern::init();

  // timer for switch
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepSpeed * 100);
  m_stepTimer.start();

  // initialize the sub patterns one time first
  setEvensOdds(PATTERN_STROBE, PATTERN_DOPS);
}

void BackStrobePattern::play()
{
  if (m_stepTimer.alarm() == 0) {
    // switch which patterns are displayed
    m_switch = !m_switch;
    // update the tip/top patterns based on the switch
    setEvensOdds(m_switch ? PATTERN_DOPS : PATTERN_HYPERSTROBE,
                m_switch ? PATTERN_HYPERSTROBE : PATTERN_DOPS,
                m_switch ? &m_firstPatternArgs : &m_secondPatternArgs,
                m_switch ? &m_secondPatternArgs : &m_firstPatternArgs);
  }
  HybridPattern::play();
}