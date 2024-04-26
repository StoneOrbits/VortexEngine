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
  m_newPatternID(PATTERN_FIRST),
  m_started(false)
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
    Leds::breatheIndex(fingerTop(f), f * (255/4), (uint32_t)Time::getCurtime() / 3, 10, 255, 255);
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
  // whether targeting all or multi, otherwise just targeting some mapping of singles
  m_targetMulti = m_targetLeds == MAP_LED(LED_MULTI);
  m_targetAll = m_targetLeds == MAP_LED_ALL;

  // need to ready up the preview mode for picking patterns, this can look different based on
  // which pattern was already on this mode, and which leds they decided to pick
  // for example if they had a multi-led pattern and they are targetting some grouping of singles now
  // then we need to convert the multi into singles, maybe in the future we can allow singles to overlay
  if (m_previewMode.isMultiLed() && !m_targetAll && !m_targetMulti) {
    Colorset curSet = m_previewMode.getColorset();
    m_previewMode.setPattern(PATTERN_FIRST, LED_ALL_SINGLE, nullptr, &curSet);
    // todo: clear multi a better way, automatically when setting singles?
    m_previewMode.clearPattern(LED_MULTI);
    m_previewMode.init();
    DEBUG_LOG("Converted existing multi-led pattern to singles for given led selection");
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
  PatternID endList = PATTERN_SINGLE_LAST;
  PatternID beginList = PATTERN_SINGLE_FIRST;
#if VORTEX_SLIM == 0
  // if targeted multi led or all singles, iterate through multis
  if (m_targetAll || m_targetMulti) {
    endList = PATTERN_MULTI_LAST;
  }
  // if targeted multi then start at multis and only iterate multis
  if (m_targetMulti) {
    beginList = PATTERN_MULTI_FIRST;
  }
#endif
  m_newPatternID = (PatternID)((m_newPatternID + 1) % endList);
  if (m_newPatternID > endList || m_newPatternID < beginList) {
    m_newPatternID = beginList;
  }
  if (!m_started) {
    m_started = true;
    m_newPatternID = beginList;
  }
  // set the new pattern id
  if (isMultiLedPatternID(m_newPatternID)) {
    m_previewMode.setPattern(m_newPatternID);
  } else {
    // if the user selected multi then singles just map to all
    LedMap setLeds = m_targetMulti ? LED_ALL : m_targetLeds;
    m_previewMode.setPatternMap(setLeds, m_newPatternID);
    // TODO: clear multi a better way
    m_previewMode.clearPattern(LED_MULTI);
  }
  m_previewMode.init();
  DEBUG_LOGF("Iterated to pattern id %d", m_newPatternID);
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
    break;
  case STATE_PICK_PATTERN:
    // need to save the new pattern if it's different from current
    needsSave = (Modes::curMode()->getPatternID() != m_previewMode.getPatternID());
    // update the current mode with the new pattern
    Modes::updateCurMode(&m_previewMode);
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
