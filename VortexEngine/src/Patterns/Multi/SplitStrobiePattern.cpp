#include "SplitStrobiePattern.h"

#include "../PatternBuilder.h"
#include "../../Serial/ByteStream.h"
#include "../../Colors/Colorset.h"
#include "../../Log/Log.h"

SplitStrobiePattern::SplitStrobiePattern(uint8_t stepDuration100ms) :
  HybridPattern(),
  m_stepDuration(stepDuration100ms * 100),
  m_stepTimer(),
  m_switch(false)
{
  m_patternID = PATTERN_SPLITSTROBIE;
}

SplitStrobiePattern::SplitStrobiePattern(const PatternArgs &args) :
  SplitStrobiePattern(args.arg1)
{
}

SplitStrobiePattern::~SplitStrobiePattern()
{
}

// init the pattern to initial state
void SplitStrobiePattern::init()
{
  HybridPattern::init();

  // timer for switch
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepDuration);
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
                m_switch ? PATTERN_TRACER : PATTERN_DOPS);
  }
  HybridPattern::play();
}

// must override the serialize routine to save the pattern
void SplitStrobiePattern::serialize(ByteStream& buffer) const
{
  HybridPattern::serialize(buffer);
  buffer.serialize(m_stepDuration);
}

void SplitStrobiePattern::unserialize(ByteStream& buffer)
{
  HybridPattern::unserialize(buffer);
  buffer.unserialize(&m_stepDuration);
}

void SplitStrobiePattern::setArgs(const PatternArgs &args)
{
  HybridPattern::setArgs(args);
  m_stepDuration = args.arg1;
}

void SplitStrobiePattern::getArgs(PatternArgs &args) const
{
  HybridPattern::getArgs(args);
  args.arg1 = m_stepDuration;
  args.numArgs += 1;
}

#if SAVE_TEMPLATE == 1
void SplitStrobiePattern::saveTemplate(int level) const
{
  HybridPattern::saveTemplate(level);
  IndentMsg(level + 1, "\"StepDuration\": %d,", m_stepDuration);
}
#endif

