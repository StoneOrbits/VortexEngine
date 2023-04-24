#include "PatternSelect.h"

#include "../../Log/Log.h"

PatternSelect::PatternSelect(const RGBColor &col) :
  Menu(col)
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
  if (!m_pCurMode) {
    return false;
  }
  m_pCurMode->init();
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
  m_pCurMode->play();
  return MENU_CONTINUE;
}

void PatternSelect::onShortClick()
{
  LedPos srcLed = LED_MULTI;
  if (!m_pCurMode->isMultiLed()) {
    srcLed = mapGetFirstLed(m_targetLeds);
  }
  PatternID newID = (PatternID)(m_pCurMode->getPatternID(srcLed) + 1);
  if (newID == PATTERN_SOLID) {
    ++newID;
  }
  PatternID endList = PATTERN_SINGLE_LAST;
  PatternID beginList = PATTERN_SINGLE_FIRST;
  if (m_targetLeds == MAP_LED_ALL || m_targetLeds == MAP_LED(LED_MULTI)) {
    endList = PATTERN_MULTI_LAST;
  }
  if (m_targetLeds == MAP_LED(LED_MULTI)) {
    beginList = PATTERN_MULTI_FIRST;
  }
  if (newID > endList || newID < beginList) {
    newID = beginList;
  }
  // set the new pattern id
  if (isMultiLedPatternID(newID)) {
    m_pCurMode->setPattern(newID);
  } else {
    m_pCurMode->setPatternMap(m_targetLeds, newID);
  }
  m_pCurMode->init();
  DEBUG_LOGF("Iterated to pattern id %d", newID);
}

void PatternSelect::onLongClick()
{
  leaveMenu(true);
}
