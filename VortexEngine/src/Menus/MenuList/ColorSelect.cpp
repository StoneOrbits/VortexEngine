#include "ColorSelect.h"

#include "../../Time/TimeControl.h"
#include "../../Patterns/Pattern.h"
#include "../../Colors/Colorset.h"
#include "../../Buttons/Button.h"
#include "../../Time/Timings.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

ColorSelect::ColorSelect(const RGBColor &col) :
  Menu(col),
  m_state(STATE_PICK_SLOT),
  m_newColor(),
  m_colorset(),
  m_targetSlot(0)
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
  DEBUG_LOG("Entered color select");
  return true;
}

Menu::MenuAction ColorSelect::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
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
  showSelect();

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
      //       on the previous menu but it's not worth the effort
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
    m_newColor.hue = m_curSelection * (255 / 4);
    break;
  case STATE_PICK_HUE2:
    m_newColor.hue += m_curSelection * (255 / 16);
    break;
  case STATE_PICK_SAT:
    m_newColor.sat = sats[m_curSelection];
    break;
  case STATE_PICK_VAL:
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
      // blink faster to indicate 'add' new color
      Leds::blinkAll(Time::getCurtime(), 100, 150, RGB_BLANK);
    }
    exitIndex = m_colorset.numColors() + 1;
  }

  if (m_curSelection == exitIndex) {
    showFullSet(LED_0, Time::getCurtime(), 50, 100);
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
    MAP_FOREACH_LED(MAP_PAIR_EVENS) {
      Leds::breathIndex(pos, hue, (uint32_t)(Time::getCurtime() / 2), 22, 255, 180);
    }
    MAP_FOREACH_LED(MAP_PAIR_ODDS) {
      Leds::breathIndex(pos, hue, (uint32_t)(Time::getCurtime() / 2) + 125, 22, 255, 180);
    }
    return;
  case STATE_PICK_HUE2:
    hue += (m_curSelection * (255 / 16));
    break;
  case STATE_PICK_SAT:
    sat = sats[m_curSelection];
    break;
  case STATE_PICK_VAL:
    val = vals[m_curSelection];
    break;
  }

  Leds::setMap(MAP_PAIR_ODDS, HSVColor(hue, sat, val));
  uint8_t satt = (mode == STATE_PICK_SAT) ? m_newColor.sat : 30;
  MAP_FOREACH_LED(MAP_PAIR_EVENS) {
    Leds::breathIndex(pos, hue, (uint32_t)(Time::getCurtime() / 3), 100, satt, 30);
  }
}

void ColorSelect::showFullSet(LedPos target, uint64_t time, uint32_t offMs, uint32_t onMs)
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
