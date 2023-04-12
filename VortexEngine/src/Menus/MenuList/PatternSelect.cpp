#include "PatternSelect.h"

#include "../../Log/Log.h"

PatternSelect::PatternSelect(const RGBColor &col) :
  Menu(col),
  m_targetLed(LED_0)
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
  // todo: get the target led from the menu
  m_targetLed = LED_0;
  m_pCurMode->setSinglePat(m_targetLed, PATTERN_FIRST);
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
  m_pCurMode->play();
  return MENU_CONTINUE;
}

void PatternSelect::onShortClick()
{
  PatternID newID = (PatternID)(m_pCurMode->getPatternID(m_targetLed) + 1);
  if (newID == PATTERN_SOLID) {
    ++newID;
  }
  if (newID > PATTERN_SINGLE_LAST) {
    newID = PATTERN_SINGLE_FIRST;
  }
  m_pCurMode->setSinglePat(m_targetLed, newID);
  m_pCurMode->init();
}

void PatternSelect::onLongClick()
{
  leaveMenu(true);
}
