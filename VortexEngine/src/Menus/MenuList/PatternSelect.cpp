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
  m_srcLed(LED_FIRST),
  m_argIndex(0),
  m_started(false)
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
  m_previewMode.play();
  // show dimmer selections in advanced mode
  Menus::showSelection(m_advanced ? RGB_GREEN0 : RGB_WHITE5);
  return MENU_CONTINUE;
}

void PatternSelect::onLedSelected()
{
  m_srcLed = mapGetFirstLed(m_targetLeds);
}

void PatternSelect::onShortClick()
{
  if (m_advanced) {
    // double click = skip 10
    bool doSkip = g_pButtonM->onConsecutivePresses(2);
    MAP_FOREACH_LED(m_targetLeds) {
      Pattern *pat = m_previewMode.getPattern(pos);
      if (pat->getNumArgs() <= m_argIndex) {
        continue;
      }
      uint8_t &arg = pat->argRef(m_argIndex);
      if (doSkip) {
        arg += 10 - (arg % 10);
      } else {
        arg++;
      }
      // on/off/gap/dash duration max 100
      uint8_t max = 100;
      if (m_argIndex == 6) {
        // blend number of numflips
        max = 4;
      } else if (m_argIndex > 3) {
        // group size, solid index, blendspeed
        max = 20;
      }
      if (arg > max) {
        // red flash indicates reaching end
        Leds::holdAll(RGB_RED);
        arg %= (max + 1);
      }
      // do not let argument0 be reset to 0
      if (!m_argIndex && !arg) {
        arg = 1;
      }
    }
    m_previewMode.init();
    if (doSkip) {
      // hold white for a moment to show they are skipping 25
      Leds::holdAll(RGB_YELLOW1);
    }
    return;
  }
  PatternID newID = (PatternID)(m_previewMode.getPatternID(m_srcLed) + 1);
  PatternID maxID = PATTERN_SINGLE_LAST;
#if VORTEX_SLIM == 0
  if (m_targetLeds == MAP_LED_ALL || m_targetLeds == MAP_LED(LED_MULTI)) {
    maxID = PATTERN_MULTI_LAST;
  }
#endif
  if (newID > maxID) {
    newID = maxID;
  }
  if (!m_started) {
    m_started = true;
    newID = PATTERN_FIRST;
  }
  // set the new pattern id
  if (isMultiLedPatternID(newID)) {
    m_previewMode.setPattern(newID);
  } else {
    // TODO: clear multi a better way
    m_previewMode.setPatternMap(m_targetLeds, newID);
    m_previewMode.clearPattern(LED_MULTI);
  }
  m_previewMode.init();
  DEBUG_LOGF("Iterated to pattern id %d", newID);
}

void PatternSelect::onLongClick()
{
  if (m_advanced) {
    m_argIndex++;
    if (m_argIndex < m_previewMode.getPattern(m_srcLed)->getNumArgs()) {
      // if we haven't reached number of args yet then just return and kee pgoing
      return;
    }
    Leds::holdAll(m_menuColor);
  }
  // store the mode as current mode
  Modes::updateCurMode(&m_previewMode);
  leaveMenu(true);
}
