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
  m_curSelection(0),
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

  switch (m_state) {
  case STATE_INIT:
    // this is separate from the init function because the target led
    // hasn't been chosen yet at the time of the init function running
    // where as this will run after the target led has been chosen and
    // we can fetch the correct colorset to work with
    m_newColor.clear();
    m_curSelection = 0;
    m_targetSlot = 0;
    // grab the colorset from our selected target led
    m_colorset = *m_pCurMode->getColorset(m_targetLed);
    // move on to picking slot
    m_state = STATE_PICK_SLOT;
    break;
  case STATE_PICK_SLOT:
    showSlotSelection();
    break;
  case STATE_PICK_HUE1:
    showHueSelection1();
    break;
  case STATE_PICK_HUE2:
    showHueSelection2();
    break;
  case STATE_PICK_SAT:
    showSatSelection();
    break;
  case STATE_PICK_VAL:
    showValSelection();
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
      // if our selection is on the exit index then check if the
      // colorset has been changed and save if necessary
      Pattern *pat = m_pCurMode->getPattern(m_targetLed);
      bool needssave = false;
      if (pat && !m_colorset.equals(pat->getColorset())) {
        m_pCurMode->setColorsetAt(&m_colorset, m_targetLed);
        m_pCurMode->init();
        needsSave = true;
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
    m_state = STATE_PICK_HUE1;
    break;
  case STATE_PICK_HUE1:
    m_newColor.hue = m_curSelection * (255 / 4);
    m_state = STATE_PICK_HUE2;
    break;
  case STATE_PICK_HUE2:
    m_newColor.hue += m_curSelection * (255 / 16);
    m_state = STATE_PICK_SAT;
    break;
  case STATE_PICK_SAT:
    m_newColor.sat = sats[m_curSelection];
    m_state = STATE_PICK_VAL;
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
  m_curSelection = 0;
}

void ColorSelect::showSlotSelection()
{
  // selecting an led regular
  if (m_curSelection < m_colorset.numColors()) {
    if (g_pButton->isPressed() && g_pButton->holdDuration() >= DELETE_THRESHOLD_TICKS) {
      if ((g_pButton->holdDuration() % (DELETE_CYCLE_TICKS * 2)) > DELETE_CYCLE_TICKS) {
        // breath red instead of white blink
        Leds::breathIndex(LED_COUNT, 0, g_pButton->holdDuration());
        return;
      }
    }
    // just render that led
    Leds::clearAll();
    Leds::blinkIndex(LED_COUNT, Time::getCurtime(), 150, 650, m_colorset[m_curSelection]);
  }
  // exit index is num colors unless we have less than 8 colors
  uint8_t exitIndex = m_colorset.numColors();
  // otherwise check if we're on add-color or exit
  if (m_colorset.numColors() < MAX_COLOR_SLOTS) {
    // selecting an led to add a new color
    if (m_curSelection == m_colorset.numColors()) {
      // blinks to indicate this will be a new color added here
      Leds::clearAll();
      Leds::blinkAll(Time::getCurtime(), 100, 150, RGB_BLANK);
    }
    // if less than 8 colors the exit index is the num colors plus 1
    exitIndex = m_colorset.numColors() + 1;
  }
  // selecting the exit
  if (m_curSelection == exitIndex) {
    showFullSet(LED_COUNT, Time::getCurtime(), 50, 100);
  }
}

void ColorSelect::showHueSelection1()
{
  if (m_curSelection >= 4) {
    showExit();
    return;
  }
  uint8_t hue1 = m_curSelection * (255 / 4);
  uint8_t hue2 = hue1 + (255 / 8);
  Leds::breathIndex(LED_COUNT, hue1, (uint32_t)(Time::getCurtime() / 2) + 62, 22, 255, 180);
}

void ColorSelect::showHueSelection2()
{
  if (m_curSelection >= 4) {
    showExit();
    return;
  }
  uint8_t hue = m_newColor.hue + (m_curSelection * (255 / 16));
  Leds::setAll(HSVColor(hue, 255, 255));
}

void ColorSelect::showSatSelection()
{
  if (m_curSelection >= 4) {
    showExit();
    return;
  }

  Leds::setIndex(LED_COUNT, HSVColor(m_newColor.hue, sats[m_curSelection], 255));
}

void ColorSelect::showValSelection()
{
  if (m_curSelection >= 4) {
    showExit();
    return;
  }
  Leds::setIndex(LED_COUNT, HSVColor(m_newColor.hue, m_newColor.sat, vals[m_curSelection]));
}

void ColorSelect::showFullSet(LedPos target, uint64_t time, uint32_t offMs, uint32_t onMs)
{
  Leds::clearAll();
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    Leds::setIndex(LED_COUNT, m_colorset.get(((time / Time::msToTicks(offMs + onMs))) % m_colorset.numColors()));
  }
}
