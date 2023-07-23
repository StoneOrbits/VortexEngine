#include "PatternSelect.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Menus/Menus.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

PatternSelect::PatternSelect(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_state(STATE_PICK_LIST),
  m_newPatternID(PATTERN_FIRST)
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
  m_state = STATE_PICK_LIST;
  m_newPatternID = PATTERN_FIRST;
  DEBUG_LOG("Entered pattern select");
  return true;
}

Menu::MenuAction PatternSelect::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
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

  // show selections
  Menus::showSelection();
  return MENU_CONTINUE;
}

void PatternSelect::showListSelection()
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // hue split into 4 quadrants of 90
    Leds::breathIndex(fingerTop(f), f * (255/4), (uint32_t)Time::getCurtime() / 3, 10, 255, 255);
    Leds::setIndex(fingerTip(f), RGB_WHITE6);
  }
}

void PatternSelect::showPatternSelection()
{
  m_previewMode.play();
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds::setAll(RGB_WHITE4);
  }
}

void PatternSelect::onLedSelected()
{
  m_previewMode.setPatternMap(m_targetLeds, PATTERN_FIRST);
  m_previewMode.init();
}

void PatternSelect::onShortClick()
{
  switch (m_state) {
  case STATE_PICK_LIST:
    // wrap at the index back to pinkie
    m_curSelection = (Finger)((m_curSelection + 1) % (FINGER_THUMB + 1));
    break;
  case STATE_PICK_PATTERN:
    nextPattern();
    break;
  }
}

void PatternSelect::nextPattern()
{
  // increment to next pattern
  m_newPatternID = (PatternID)((m_newPatternID + 1) % PATTERN_COUNT);
  // change the pattern of demo mode
  m_previewMode.setPattern(m_newPatternID);
  m_previewMode.init();
  DEBUG_LOGF("Demoing Pattern %u", m_newPatternID);
}

void PatternSelect::onLongClick()
{
  bool needsSave = false;
  Mode *cur = Modes::curMode();
  switch (m_state) {
  case STATE_PICK_LIST:
    if (m_curSelection == FINGER_THUMB) {
      leaveMenu();
      return;
    }
    m_state = STATE_PICK_PATTERN;
    // start the new pattern ID selection based on the chosen list
    m_newPatternID = (PatternID)(PATTERN_FIRST + (m_curSelection * (PATTERN_COUNT / 4)));
    m_previewMode.setPattern(m_newPatternID);
    m_previewMode.init();
    DEBUG_LOGF("Started picking pattern at %u", m_newPatternID);
    break;
  case STATE_PICK_PATTERN:
    // need to save the new pattern if it's different from current
    needsSave = (cur->getPatternID() != m_newPatternID);
    // store the new pattern in the mode
    cur->setPattern(m_newPatternID);
    cur->init();
    DEBUG_LOGF("Saving pattern %u", m_newPatternID);
    // go back to beginning for next time
    m_state = STATE_PICK_LIST;
    // done in the pattern select menu
    leaveMenu(needsSave);
    break;
  }
  // reset selection after choosing anything
  m_curSelection = FINGER_FIRST;
}

void PatternSelect::showExit()
{
  // don't show the exit when picking pattern
  if (m_state == STATE_PICK_PATTERN) {
    return;
  }
  Menu::showExit();
}
