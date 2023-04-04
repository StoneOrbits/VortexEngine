#include "PatternSelect.h"

#include "../../Log/Log.h"

PatternSelect::PatternSelect() :
  Menu(),
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
  DEBUG_LOG("Entered pattern select");
  return true;
}

bool PatternSelect::run()
{
  if (!Menu::run()) {
    return false;
  }
  m_pCurMode->play();
  return true;
}

void PatternSelect::onShortClick()
{
  m_pCurMode->setSinglePat(m_targetLed, (PatternID)(m_pCurMode->getPatternID(m_targetLed) + 1));
  m_pCurMode->init();
}

void PatternSelect::onLongClick()
{
  leaveMenu(true);
}
