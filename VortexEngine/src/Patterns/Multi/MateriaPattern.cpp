#include "MateriaPattern.h"

#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

MateriaPattern::MateriaPattern(const PatternArgs &args) :
  MultiLedPattern(args),
  m_onDuration1(0),
  m_offDuration1(0),
  m_onDuration2(0),
  m_offDuration2(0),
  m_stepSpeed(0),
  m_stepTimer(),
  m_ledMap(0),
  m_switch(false)
{
  m_patternID = PATTERN_MATERIA;
  REGISTER_ARG(m_onDuration1);
  REGISTER_ARG(m_offDuration1);
  REGISTER_ARG(m_onDuration2);
  REGISTER_ARG(m_offDuration2);
  REGISTER_ARG(m_stepSpeed);
  setArgs(args);
}

MateriaPattern::~MateriaPattern()
{
}

// init the pattern to initial state
void MateriaPattern::init()
{
  MultiLedPattern::init();

  // start on index 1
  m_colorset.setCurIndex(1);
  // reset the blink timer entirely
  m_blinkTimer1.reset();
  // dops timing
  m_blinkTimer1.addAlarm(m_onDuration1);
  m_blinkTimer1.addAlarm(m_offDuration1);
  // start the blink timer from the next frame
  m_blinkTimer1.start();

  // reset the blink timer entirely
  m_blinkTimer2.reset();
  // dops timing
  m_blinkTimer2.addAlarm(m_onDuration2);
  m_blinkTimer2.addAlarm(m_offDuration2);
  // start the blink timer from the next frame
  m_blinkTimer2.start();

  // reset and add alarm
  m_stepTimer.reset();
  m_stepTimer.addAlarm(m_stepSpeed * 100);
  m_stepTimer.start();

  // map of the Thumb, Index, Ring, and Pinkie Tops
  m_ledMap = MAP_LED(LED_9) | MAP_LED(LED_7) | MAP_LED(LED_3) | MAP_LED(LED_1);

  //Set the middle top to solid color 0
  Leds::setIndex(LED_5, m_colorset.get(0));
}

// pure virtual must override the play function
void MateriaPattern::play()
{
  // get next color on stepTimer alarm, skip color 0
  if (m_stepTimer.alarm() == 0) {
    m_colorset.getNext();
    if (m_colorset.curIndex() == 0) {
      m_colorset.getNext();
    }
  }
  switch (m_blinkTimer1.alarm()) {
  case -1: // just break and still run post-step
    break;
  case 0: // turn on the leds for given mapping
    m_switch = !m_switch;
    // alternate between showing the current and next color
    if (m_switch) {
      // if this is the last color of the set, skip color 0 (color 0 is for Middle top only)
      if (m_colorset.curIndex() != m_colorset.numColors() - 1) {
        Leds::setAllEvens(m_colorset.peekNext());
      } else {
        Leds::setAllEvens(m_colorset.peek(2));
      }
    }
    else {
      Leds::setAllEvens(m_colorset.cur());
    }
    break;
  case 1: // turn off the leds
    Leds::clearAllEvens();
    break;
  }

  switch (m_blinkTimer2.alarm()) {
  case -1: // just break and still run post-step
    break;
  case 0: // turn on the leds for given mapping
    Leds::setMap(m_ledMap, m_colorset.cur());
    break;
  case 1: // turn off the leds
    Leds::clearMap(m_ledMap);
    break;
  }
 }
