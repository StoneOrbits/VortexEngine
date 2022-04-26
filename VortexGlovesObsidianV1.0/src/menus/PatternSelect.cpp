#include "PatternSelect.h"

#include "../patterns/Pattern.h"
#include "../Mode.h"

#include "../Log.h"

PatternSelect::PatternSelect() :
  Menu(),
  m_state(STATE_PICK_LIST),
  m_list(0),
  m_pColorset(nullptr),
  m_pNewPattern(nullptr)
{
}

bool PatternSelect::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }
  // fetch the colorset currently used so we can display demo
  // patterns with the same colorset
  m_pColorset = curMode->getColorset();
  DEBUG("Entered pattern select");
  return true;
}

bool PatternSelect::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  switch (m_state) {
  case STATE_PICK_LIST:
    // display lists
    showListSelection();
    blinkSelection();
    break;
  case STATE_PICK_PATTERN:
    // display patterns
    showPatternSelection();
    break;
  }

  // continue
  return true;
}

void PatternSelect::showListSelection()
{
  // TODO: how to lists?
}

void PatternSelect::showPatternSelection()
{
  // run the new pattern on all of the LEDs
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    m_pNewPattern->play(m_pColorset, pos);
  }
}

void PatternSelect::onShortClick()
{
    leaveMenu();
  switch (m_state) {
  case STATE_PICK_LIST:
    // wrap at the thumb back to pinkie
    m_curSelection = (Finger)((m_curSelection + 1) % FINGER_THUMB);
    break;
  case STATE_PICK_PATTERN:
    // TODO: implement a pattern list?
    // m_pNewPattern = nextPattern();
    //m_curSelection = (Finger)(((uint32_t)m_curSelection + 1) % 10);
    break;
  }
}

void PatternSelect::onLongClick()
{
    leaveMenu();
  switch (m_state) {
  case STATE_PICK_LIST:
    // store the list selection
    m_list = m_curSelection;
    m_state = STATE_PICK_PATTERN;
    break;
  case STATE_PICK_PATTERN:
    // store the new pattern in the mode
    m_pCurMode->setPattern(m_pNewPattern);
    // go back to beginning for next time
    m_state = STATE_PICK_LIST;
    // done in the pattern select menu
    leaveMenu();
    break;
  }
  // reset selection after choosing anything
  m_curSelection = FINGER_FIRST;
}
