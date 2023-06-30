#include "ColorSelect.h"

#include "../../Time/TimeControl.h"
#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Colors/Colorset.h"
#include "../../Buttons/Button.h"
#include "../../Random/Random.h"
#include "../../Time/Timings.h"
#include "../../Menus/Menus.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

ColorSelect::ColorSelect(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_state(STATE_PICK_SLOT),
  m_newColor(),
  m_colorset(),
  m_targetSlot(0),
  m_targetHue1(0),
  m_targetHue2(0),
  m_targetSat(0),
  m_pattern(nullptr)
{
  // NOTE! Specifically using hsv_to_rgb_rainbow instead of generic because
  // it will generate nicer looking colors and a nicer rainbow to select
  // from than a regular hsv conversion will.  This is because the rainbow
  // function uses a different algorithm to generate the colors that results
  // in a smaller color space with less bright colors. The tradeoff is you lose
  // the bright colors but the rainbow looks a lot better
  g_hsv_rgb_alg = HSV_TO_RGB_RAINBOW;
}

ColorSelect::~ColorSelect()
{
  // revert the hsv to rgb algorithm to normal
  g_hsv_rgb_alg = HSV_TO_RGB_GENERIC;
}

bool ColorSelect::init()
{
  if (!Menu::init()) {
    return false;
  }
  if (m_pCurMode->isMultiLed()) {
    m_ledSelected = true;
  }
  m_state = STATE_INIT;
  if (m_advanced) {
    m_pattern = PatternBuilder::make(PATTERN_BLEND);
    if (!m_pattern) {
      return false;
    }
    m_pattern->setColorset(m_pCurMode->getColorset());
    m_pattern->init();
  }
  DEBUG_LOG("Entered color select");
  return true;
}

Menu::MenuAction ColorSelect::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }

  if (m_advanced && m_pattern) {
    m_pattern->setArg(6, g_pButton->isPressed() ? 2 : 1);
    if (g_pButton->consecutivePresses() > 5) {
      return MENU_QUIT;
    }
    // just render the current pattern for active color picking
    // iterate all patterns and plkay
    m_pattern->play();
    return MENU_CONTINUE;
  }

  // all states start with a blank slate
  Leds::clearAll();

  switch (m_state) {
  case STATE_INIT:
    // this is separate from the init function because the target led
    // hasn't been chosen yet at the time of the init function running
    // where as this will run after the target led has been chosen and
    // we can fetch the correct colorset to work with
    //m_newColor.clear();
    m_newColor = HSVColor(0, 255, 255);
    m_targetSlot = 0;
    m_targetHue1 = 0;
    m_targetHue2 = 0;
    m_targetSat = 0;
    m_curSelection = 0;
    m_targetSlot = 0;
    // grab the colorset from our selected target led
    if (m_targetLeds == MAP_LED_ALL) {
      m_colorset = m_pCurMode->getColorset();
    } else {
      m_colorset = m_pCurMode->getColorset(mapGetFirstLed(m_targetLeds));
    }
    // move on to picking slot
    m_state = STATE_PICK_SLOT;
    break;
  case STATE_PICK_SLOT:
    showSlotSelection();
    break;
  case STATE_PICK_HUE1:
  case STATE_PICK_HUE2:
  case STATE_PICK_SAT:
  case STATE_PICK_VAL:
    showSelection(m_state);
    break;
  }

  // show selections
  Menus::showSelection();

  return MENU_CONTINUE;
}

void ColorSelect::onShortClick()
{
  m_curSelection = (m_curSelection + 1);
  switch (m_state) {
  case STATE_PICK_SLOT:
    m_curSelection %= (m_colorset.numColors() + 1 + (m_colorset.numColors() < MAX_COLOR_SLOTS));
    break;
  case STATE_PICK_HUE1:
  case STATE_PICK_HUE2:
  case STATE_PICK_SAT:
  case STATE_PICK_VAL:
  default:
    m_curSelection %= 5;
    break;
  }
}

void ColorSelect::onLongClick()
{
  if (m_advanced) {
    return;
  }
  bool needsSave = false;
  // if we're on 'exit' and we're on any menu past the slot selection
  if (m_curSelection == 4 && m_state > STATE_PICK_SLOT) {
    // move back to the previous selection
    m_state = (ColorSelectState)(m_state - 1);
    if (m_state == STATE_PICK_SLOT) {
      // if we're back to the slot selection then set our position
      // back to the slot we selected
      m_curSelection = m_targetSlot;
    } else {
      // otherwise reset our selection position to 0
      // NOTE: we could try to reset our position to whatever we selected
      //       on the previous menu but we found that it's actually more
      //       visually appealing to return to the first selection on the
      //       previous menu, otherwise it can be harder to tell whether
      //       you have gone back a menu or not
      m_curSelection = 0;
    }
    return;
  }
  switch (m_state) {
  case STATE_INIT:
    // nothing
    return;
  case STATE_PICK_SLOT:
    // if the exit is selected then save and exit, this depends on the
    // number of colors in the colorset. If the colorset is full then
    // the exit is just the number of colors (8) but if it's not full
    // then the exit is the number of colors + 1. Example: with 4 cols,
    // cols are on 0, 1, 2, 3, add-color is 4, and exit is 5
    if (m_curSelection == m_colorset.numColors() + (m_colorset.numColors() < MAX_COLOR_SLOTS)) {
      // if we're targetting more than one led then screw
      // checking if the colorset has changed because it's
      // not worth the effort
      needsSave = (!MAP_IS_ONE_LED(m_targetLeds) ||
          !m_colorset.equals(m_pCurMode->getColorset(mapGetFirstLed(m_targetLeds))));
      if (needsSave) {
        m_pCurMode->setColorsetMap(m_targetLeds, m_colorset);
        m_pCurMode->init();
      }
      leaveMenu(needsSave);
      return;
    }
    // handle if user releases during the delete option
    if (m_curSelection < m_colorset.numColors() &&
        g_pButton->holdDuration() >= DELETE_THRESHOLD_TICKS &&
       (g_pButton->holdDuration() % (DELETE_CYCLE_TICKS * 2)) > (DELETE_CYCLE_TICKS)) {
      // delete current slot
      m_colorset.removeColor(m_curSelection);
      if (m_curSelection > m_colorset.numColors()) {
        m_curSelection--;
      }
      return;
    }
    // otherwise store the target slot continue onto the hue selection
    m_targetSlot = m_curSelection;
    break;
  case STATE_PICK_HUE1:
    m_targetHue1 = m_curSelection;
    m_newColor.hue = m_targetHue1 * (255 / 4);
    break;
  case STATE_PICK_HUE2:
    m_targetHue2 = m_curSelection;
    m_newColor.hue = (m_targetHue1 * (255 / 4)) + m_targetHue2 * (255 / 16);
    break;
  case STATE_PICK_SAT:
    m_targetSat = m_curSelection;
    m_newColor.sat = sats[m_targetSat];
    break;
  case STATE_PICK_VAL:
    // no m_targetVal because you can't go back after this
    m_newColor.val = vals[m_curSelection];
    // specifically using hsv to rgb rainbow to generate the color
    m_colorset.set(m_targetSlot, m_newColor);
    m_newColor.clear();
    m_curSelection = m_targetSlot;
    m_state = STATE_PICK_SLOT;
    return;
  }
  m_state = (ColorSelectState)(m_state + 1);
  m_curSelection = 0;
}

void ColorSelect::showSlotSelection()
{
  uint8_t exitIndex = m_colorset.numColors();
  uint32_t holdDur = g_pButton->holdDuration();

  bool withinNumColors = m_curSelection < m_colorset.numColors();
  bool holdDurationCheck = g_pButton->isPressed() && holdDur >= DELETE_THRESHOLD_TICKS;
  bool holdDurationModCheck = (holdDur % (DELETE_CYCLE_TICKS * 2)) > DELETE_CYCLE_TICKS;

  if (withinNumColors && holdDurationCheck && holdDurationModCheck) {
    // breath red for delete slot
    Leds::breathIndex(LED_COUNT, 0, holdDur);
  } else if (withinNumColors) {
    // blink the selected slot color
    Leds::blinkIndex(LED_COUNT, Time::getCurtime(), 150, 650, m_colorset[m_curSelection]);
  } else if (m_colorset.numColors() < MAX_COLOR_SLOTS) {
    if (m_curSelection == m_colorset.numColors()) {
      // blink both leds and blink faster to indicate 'add' new color
      Leds::blinkAll(Time::getCurtime(), 100, 150, RGB_WHITE2);
    }
    exitIndex = m_colorset.numColors() + 1;
  }

  if (m_curSelection == exitIndex) {
    showFullSet(LED_0, Time::getCurtime(), 50, 100);
    // set LED_1 to green to indicate save and exit
    Leds::setIndex(LED_1, RGB_GREEN2);
    // if not on exitIndex or add new color set LED_1 based on button state
  } else if (m_curSelection != m_colorset.numColors()) {
    Leds::setIndex(LED_1, g_pButton->isPressed() ? RGB_OFF : RGB_WHITE2);
  }
}

void ColorSelect::showSelection(ColorSelectState mode)
{
  if (m_curSelection >= 4) {
    showExit();
    return;
  }

  uint8_t hue = m_newColor.hue;
  uint8_t sat = m_newColor.sat;
  uint8_t val = 255;

  switch (mode) {
  default:
    return;
  case STATE_PICK_HUE1:
    hue = m_curSelection * (255 / 4);
    Leds::breathIndex(LED_0, hue, (uint32_t)(Time::getCurtime() / 2), 22, 255, 180);
    Leds::breathIndex(LED_1, hue, (uint32_t)(Time::getCurtime() / 2) + 125, 22, 255, 180);
    // force sat at hue level1
    sat = 255;
    // NOTE: return here
    return;
  case STATE_PICK_HUE2:
    hue = m_targetHue1 * (255 / 4) + (m_curSelection * (255 / 16));
    Leds::setIndex(LED_1, RGB_WHITE0);
    // force sat at hue level2
    sat = 255;
    break;
  case STATE_PICK_SAT:
    sat = sats[m_curSelection];
    Leds::breathIndexSat(LED_1, hue, (uint32_t)(Time::getCurtime() / 10), 50, 255, 150);
    break;
  case STATE_PICK_VAL:
    val = vals[m_curSelection];
    Leds::breathIndexVal(LED_1, hue, (uint32_t)(Time::getCurtime() / 10), 50, sat, 150);
    break;
  }
  Leds::setMap(MAP_PAIR_EVENS, HSVColor(hue, sat, val));
}

void ColorSelect::showFullSet(LedPos target, uint32_t time, uint32_t offMs, uint32_t onMs)
{
  if (!m_colorset.numColors()) {
    // wat do?
    return;
  }
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    int divisor = Time::msToTicks(offMs + onMs);
    if (!divisor) {
      divisor = 1;
    }
    Leds::setIndex(LED_COUNT, m_colorset.get(((time / divisor)) % m_colorset.numColors()));
  }
  Leds::setIndex(LED_1, 0x001000);
}
