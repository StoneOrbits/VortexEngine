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
  LedPos srcLed = LED_MULTI;
  if (!m_previewMode.isMultiLed()) {
    srcLed = mapGetFirstLed(m_targetLeds);
  }
  PatternID newID = (PatternID)(m_previewMode.getPatternID(srcLed) + 1);
  PatternID endList = PATTERN_SINGLE_LAST;
  PatternID beginList = PATTERN_SINGLE_FIRST;
#if VORTEX_SLIM == 0
  if (m_targetLeds == MAP_LED_ALL || m_targetLeds == MAP_LED(LED_MULTI)) {
    endList = PATTERN_MULTI_LAST;
  }
  if (m_targetLeds == MAP_LED(LED_MULTI)) {
    beginList = PATTERN_MULTI_FIRST;
  }
#endif
  if (newID > endList || newID < beginList) {
    newID = beginList;
  }
  if (!m_started) {
    m_started = true;
    m_newPatternID = PATTERN_FIRST;
  }
  // set the new pattern id
  if (isMultiLedPatternID(newID)) {
    m_previewMode.setPattern(newID);
  } else {
    // TODO: clear multi a better way
    m_previewMode.setPatternMap(m_targetLeds, newID);
    m_previewMode.clearPattern(LED_MULTI);
  }
  m_previewMode.init();
  DEBUG_LOGF("Iterated to pattern id %d", newID);
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
    // need to ready up the preview mode for picking patterns, this can look different based on
    // which pattern was already on this mode, and which leds they decided to pick
    // for example if they had a multi-led pattern and they are targetting some grouping of singles now
    // then we need to convert the multi into singles, maybe in the future we can allow singles to overlay
    if (m_previewMode.isMultiLed() && m_targetLeds != MAP_LED_ALL && m_targetLeds != MAP_LED(LED_MULTI)) {
      Colorset curSet = m_previewMode.getColorset();
      m_previewMode.clearPattern(LED_MULTI);
      m_previewMode.setPattern(PATTERN_FIRST, LED_ALL_SINGLE, nullptr, &curSet);
    }
    m_previewMode.setPatternMap(m_targetLeds, m_newPatternID);
    m_previewMode.init();
    DEBUG_LOGF("Started picking pattern at %u", m_newPatternID);
    break;
  case STATE_PICK_PATTERN:
    // need to save the new pattern if it's different from current
    needsSave = (cur->getPatternID() != m_previewMode.getPatternID());
    // update the current mode with the new pattern
    Modes::updateCurMode(&m_previewMode);
    // then done here, save if the mode was different
    leaveMenu(needsSave);
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
