#include "PatternSelect.h"

#include "../patterns/Pattern.h"
#include "../PatternBuilder.h"
#include "../LedControl.h"
#include "../Mode.h"

#include "../Log.h"

PatternSelect::PatternSelect() :
  Menu(),
  m_state(STATE_PICK_LIST),
  m_colorset(),
  m_pNewPattern(nullptr)
{
}

bool PatternSelect::init()
{
  if (!Menu::init()) {
    return false;
  }
  // grab a copy of current colorset
  m_colorset = *m_pCurMode->getColorset();
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
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // hue split into 4 quadrants of 90
    Leds::setFinger(f, HSVColor(f * 90, 255, 255));
  }
}

void PatternSelect::showPatternSelection()
{
  // run the new pattern on all of the LEDs
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    // TODO: separate colorset for each?
    m_pNewPattern->play();
  }
}

void PatternSelect::onShortClick()
{
  switch (m_state) {
  case STATE_PICK_LIST:
    // wrap at the index back to pinkie
    m_curSelection = (Finger)((m_curSelection + 1) % FINGER_INDEX);
    break;
  case STATE_PICK_PATTERN:
    nextPattern();
    break;
  }
}

void PatternSelect::nextPattern()
{
  // store the ID of the next pattern
  PatternID newID = (PatternID)((m_pNewPattern->getPatternID() + 1) % PATTERN_COUNT);
  // delete the current pattern
  delete m_pNewPattern;
  // create the new pattern from stored ID
  m_pNewPattern = PatternBuilder::make(newID);
}

void PatternSelect::onLongClick()
{
  switch (m_state) {
  case STATE_PICK_LIST:
    // create the new pattern from stored ID
    m_pNewPattern = PatternBuilder::make((PatternID)((PATTERN_COUNT / 4) * m_curSelection));
    m_state = STATE_PICK_PATTERN;
    break;
  case STATE_PICK_PATTERN:
    // store the new pattern in the mode
    // TODO: Fix this
    //m_pCurMode->changeAllPatterns(m_pNewPattern);
    delete m_pNewPattern;
    // go back to beginning for next time
    m_state = STATE_PICK_LIST;
    // done in the pattern select menu
    leaveMenu();
    break;
  }
  // reset selection after choosing anything
  m_curSelection = FINGER_FIRST;
}
