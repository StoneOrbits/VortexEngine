#include "PatternSelect.h"

#include "../patterns/Pattern.h"
#include "../Mode.h"

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
  return true;
}

bool PatternSelect::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  // handle base menu logic
  if (!Menu::run(timeControl, button, ledControl)) {
    return false;
  }

  switch (m_state) {
  case STATE_PICK_LIST:
    // display lists
    pickList(timeControl, ledControl);
    break;
  case STATE_PICK_PATTERN:
    // display patterns
    pickPattern(timeControl, ledControl);
    break;
  }

  // continue
  return true;
}

void PatternSelect::pickList(const TimeControl *timeControl, LedControl *ledControl)
{
  // TODO: how to lists?
}

void PatternSelect::pickPattern(const TimeControl *timeControl, LedControl *ledControl)
{
  // run the new pattern on all of the LEDs
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    m_pNewPattern->play(timeControl, ledControl, m_pColorset, pos);
  }
}

void PatternSelect::onShortClick()
{
  switch (m_state) {
  case STATE_PICK_LIST:
    // only 4 options for list
    m_curSelection = (m_curSelection + 1) % 4;
    break;
  case STATE_PICK_PATTERN:
    // TODO: implement a pattern list?
    // m_pNewPattern = nextPattern();
    m_curSelection = (m_curSelection + 1) % 10;
    break;
  }
}

void PatternSelect::onLongClick()
{
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
  m_curSelection = 0;
}
