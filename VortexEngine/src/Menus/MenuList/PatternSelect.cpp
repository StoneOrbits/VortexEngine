#include "PatternSelect.h"

#include "../../Modes/Modes.h"
#include "../../Menus/Menus.h"
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
  // show selections
  Menus::showSelection();
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
}

void PatternSelect::onShortClick()
{
  LedPos srcLed = LED_MULTI;
  if (!m_patternMode.isMultiLed()) {
    srcLed = mapGetFirstLed(m_targetLeds);
  }
  PatternID newID = (PatternID)(m_patternMode.getPatternID(srcLed) + 1);
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
    m_previewMode.setPattern(newID);
  } else {
    m_previewMode.setPatternMap(m_targetLeds, newID);
  }
  m_previewMode.init();
  DEBUG_LOGF("Iterated to pattern id %d", newID);
}

void PatternSelect::onLongClick()
{
  // store the mode as current mode
  Modes::updateCurMode(&m_previewMode);
  leaveMenu(true);
}
