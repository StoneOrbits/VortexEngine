#include "PatternSelect.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

PatternSelect::PatternSelect(const RGBColor &col) :
  Menu(col),
  m_state(STATE_PICK_LIST),
  m_demoMode(),
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
  m_demoMode.setPattern(m_newPatternID, nullptr, m_pCurMode->getColorset());
  m_demoMode.init();
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

  // continue
  return MENU_CONTINUE;
}

void PatternSelect::showListSelection()
{
  for (Quadrant f = QUADRANT_FIRST; f <= QUADRANT_4; ++f) {
    // hue split into 4 quadrants of 90
    Leds::breathQuadrant(f, f * (255 / 4), (uint32_t)Time::getCurtime(), 10, 255, 255);
  }
  Leds::blinkQuadrantFive((uint32_t)Time::getCurtime(), 10, 50, RGB_OFF);
}

void PatternSelect::showPatternSelection()
{
  m_demoMode.play();
  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds::setAll(RGB_DIM_WHITE2);
  }
}

void PatternSelect::onShortClick()
{
  switch (m_state) {
  case STATE_PICK_LIST:
    // wrap at the index back to pinkie
    m_curSelection = (Quadrant)((m_curSelection + 1) % (QUADRANT_COUNT));
    break;
  case STATE_PICK_PATTERN:
    nextPattern();
    break;
  }
}

void PatternSelect::onShortClick2()
{
  switch (m_state) {
  case STATE_PICK_LIST:
    // wrap at the index back to pinkie
    if (!m_curSelection) {
      m_curSelection = QUADRANT_LAST;
    } else {
      m_curSelection = m_curSelection - 1;
    }
    break;
  case STATE_PICK_PATTERN:
    previousPattern();
    break;
  }
}

void PatternSelect::nextPattern()
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

void PatternSelect::previousPattern()
{
  LedPos srcLed = LED_MULTI;
  if (!m_pCurMode->isMultiLed()) {
    srcLed = mapGetFirstLed(m_targetLeds);
  }
  PatternID newID = (PatternID)(m_pCurMode->getPatternID(srcLed) - 1);
  if (newID == PATTERN_SOLID) {
    --newID;
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
    newID = endList;
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
  bool needsSave = false;
  switch (m_state) {
  case STATE_PICK_LIST:
    if (m_curSelection == QUADRANT_LAST) {
      leaveMenu();
      return;
    }
    m_state = STATE_PICK_PATTERN;
    // start the new pattern ID selection based on the chosen list
    m_newPatternID = (PatternID)(PATTERN_FIRST + (m_curSelection * (PATTERN_COUNT / 4)));
    m_demoMode.setPattern(m_newPatternID);
    m_demoMode.init();
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
  m_curSelection = QUADRANT_FIRST;
}

void PatternSelect::onLongClick2()
{
  leaveMenu();
}
