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
  m_previewMode.play();
  // show dimmer selections in advanced mode
  Menus::showSelection(m_advanced ? RGB_GREEN0 : RGB_WHITE5);
  return MENU_CONTINUE;
}

void PatternSelect::onLedSelected()
{
  if (!m_advanced) {
    // if not in advanced then change the starting pattern, otherwise start
    // on the pattern we already had
    m_previewMode.setPatternMap(m_targetLeds, PATTERN_FIRST);
    m_previewMode.init();
  }
  m_srcLed = mapGetFirstLed(m_targetLeds);
}

void PatternSelect::onShortClick()
{
  if (m_advanced) {
    // double click = skip 10
    bool doSkip = (g_pButton->consecutivePresses() >= 2);
    if (doSkip) {
      g_pButton->resetConsecutivePresses();
    }
    MAP_FOREACH_LED(m_targetLeds) {
      uint8_t &arg = m_previewMode.getPattern(pos)->argRef(m_argIndex);
      if (doSkip) {
        arg += 10 - (arg % 10);
      } else {
        arg++;
      }
      // on/off/gap/dash duration max 100
      uint8_t max = 100;
      // limit to a max value based on the argument
      if (m_argIndex == 4) {
        // group size max 20
        max = 20;
      } else if (m_argIndex > 4) {
        // blend = hue offset, and num flips
        // solid = col index
        // all cases just max it at 8
        max = 8;
      }
      arg %= max;
      // do not let argument0 be reset to 0
      if (!m_argIndex && !arg) {
        arg = 1;
      }
    }
    m_previewMode.init();
    if (doSkip) {
      // hold white for a moment to show they are skipping 25
      Leds::holdAll(250, RGB_YELLOW1);
    }
    return;
  }

  PatternID newID = (PatternID)(m_previewMode.getPatternID(m_srcLed) + 1);
  if (newID > PATTERN_SINGLE_LAST) {
    newID = PATTERN_SINGLE_FIRST;
  }

  // set the new pattern id
  if (isMultiLedPatternID(newID)) {
    m_previewMode.setPattern(newID);
  } else {
    m_previewMode.setPatternMap(m_targetLeds, newID);
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
    Leds::holdAll(200, m_menuColor);
  }
  // store the mode as current mode
  Modes::updateCurMode(&m_previewMode);
  leaveMenu(true);
}
