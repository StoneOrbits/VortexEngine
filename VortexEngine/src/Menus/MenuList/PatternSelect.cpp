#include "PatternSelect.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

PatternSelect::PatternSelect() :
  Menu(),
  m_state(STATE_PICK_LIST),
  m_pDemoMode(nullptr),
  m_newPatternID(PATTERN_FIRST)
{
}

PatternSelect::~PatternSelect()
{
  if (m_pDemoMode) {
    delete m_pDemoMode;
    m_pDemoMode = nullptr;
  }
}

bool PatternSelect::init()
{
  if (!Menu::init()) {
    return false;
  }
  m_state = STATE_PICK_LIST;
  m_newPatternID = PATTERN_FIRST;
  if (!m_pDemoMode) {
    m_pDemoMode = new Mode(m_newPatternID, nullptr, m_pCurMode->getColorset());
  } else {
    m_pDemoMode->setPattern(m_newPatternID);
  }
  if (!m_pDemoMode) {
    DEBUG_LOG("Failed to build demo mode for pattern select");
    return false;
  }
  m_pDemoMode->init();
  DEBUG_LOG("Entered pattern select");
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
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // hue split into 4 quadrants of 90
    Leds::breathIndex(fingerTop(f), f * (255/4), (uint32_t)Time::getCurtime() / 3, 10, 255, 255);
    Leds::setIndex(fingerTip(f), RGB_DIM_WHITE1);
  }
}

void PatternSelect::showPatternSelection()
{
  if (m_pDemoMode) {
    m_pDemoMode->play();
  }
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds::setAll(RGB_DIM_WHITE2);
  }
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
  m_pDemoMode->setPattern(m_newPatternID);
  m_pDemoMode->init();
  DEBUG_LOGF("Demoing Pattern %u", m_newPatternID);
}

void PatternSelect::onLongClick()
{
  bool needsSave = false;
  switch (m_state) {
  case STATE_PICK_LIST:
    if (m_curSelection == FINGER_THUMB) {
      leaveMenu();
      return;
    }
    m_state = STATE_PICK_PATTERN;
    // start the new pattern ID selection based on the chosen list
    m_newPatternID = (PatternID)(PATTERN_FIRST + (m_curSelection * (PATTERN_COUNT / 4)));
    m_pDemoMode->setPattern(m_newPatternID);
    m_pDemoMode->init();
    DEBUG_LOGF("Started picking pattern at %u", m_newPatternID);
    break;
  case STATE_PICK_PATTERN:
    // need to save the new pattern if it's different from current
    needsSave = (m_pCurMode->getPatternID() != m_newPatternID);
    // store the new pattern in the mode
    m_pCurMode->setPattern(m_newPatternID);
    m_pCurMode->init();
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
