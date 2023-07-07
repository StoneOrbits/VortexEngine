#include "PatternSelect.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/PatternArgs.h"
#include "../../Patterns/Pattern.h"
#include "../../Serial/ByteStream.h"
#include "../../Time/TimeControl.h"
#include "../../Buttons/Button.h"
#include "../../Random/Random.h"
#include "../../Time/Timings.h"
#include "../../Modes/Modes.h"
#include "../../Menus/Menus.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

PatternSelect::PatternSelect(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_patternMode(),
  m_srcLed(LED_FIRST),
  m_argIndex(0)
{
}

PatternSelect::~PatternSelect()
{
}

bool PatternSelect::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entered pattern select");
  return true;
}

Menu::MenuAction PatternSelect::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  // run the current mode
  m_patternMode.play();
  if (!m_advanced) {
    // show selections
    Menus::showSelection();
  }
  return MENU_CONTINUE;
}

void PatternSelect::onLedSelected()
{
  m_patternMode = *m_pCurMode;
  m_patternMode.setPatternMap(m_targetLeds, PATTERN_FIRST);
  m_patternMode.init();
  m_srcLed = mapGetFirstLed(m_targetLeds);
}

void PatternSelect::onShortClick()
{
  if (m_advanced) {
    MAP_FOREACH_LED(m_targetLeds) {
      //m_patternMode.getPattern(pos)->setArg(m_argIndex, m_patternMode.getPattern(pos)->getArg(pos) + 1);
      m_patternMode.getPattern(pos)->argRef(m_argIndex)+=5;
      m_patternMode.init();
      Leds::holdAll(200, RGB_WHITE1);
    }
    return;
  }

  PatternID srcID = m_patternMode.getPatternID(m_srcLed);
  PatternID newID = (PatternID)(srcID + 1);
  PatternID endList = PATTERN_SINGLE_LAST;
  PatternID beginList = PATTERN_SINGLE_FIRST;
#if VORTEX_SLIM == 0
  if (m_targetLeds == MAP_LED_ALL || m_targetLeds == MAP_LED(LED_MULTI)) {
    endList = PATTERN_MULTI_LAST;
  }
  if (m_targetLeds == MAP_LED(LED_MULTI)) {
    beginList = PATTERN_MULTI_FIRST;
  }
#endif
  if (newID > endList || newID < beginList) {
    newID = beginList;
  }

  // set the new pattern id
  if (isMultiLedPatternID(newID)) {
    m_patternMode.setPattern(newID);
  } else {
    m_patternMode.setPatternMap(m_targetLeds, newID);
  }
  m_patternMode.init();
  DEBUG_LOGF("Iterated to pattern id %d", newID);
}

void PatternSelect::onLongClick()
{
  if (m_advanced) {
    m_argIndex++;
    if (m_argIndex < m_patternMode.getPattern(m_srcLed)->getNumArgs()) {
      Leds::holdAll(200, HSVColor(m_argIndex * 28, 255, 60));
      // if we haven't reached number of args yet then just return and kee pgoing
      return;
    }
  }
  // store the mode as current mode
  Modes::updateCurMode(&m_patternMode);
  leaveMenu(true);
}
