#include "ColorSelect.h"

#include "../TimeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Button.h"
#include "../Mode.h"
#include "../Log.h"

// the number of slots in a page
#define PAGE_SIZE 4
// the number of pages
#define NUM_PAGES 2

ColorSelect::ColorSelect() :
  Menu(),
  m_state(STATE_PICK_SLOT),
  m_colorset(),
  m_curPage(0),
  m_slot(0),
  m_quadrant(0),
  m_newColor()
{
}

bool ColorSelect::init()
{
  if (!Menu::init()) {
    return false;
  }
  // copy the colorset from the current mode
  m_colorset = *m_pCurMode->getSingleColorset();
  DEBUG("Entered color select");
  return true;
}

bool ColorSelect::run()
{
  // handle base menu logic
  if (!Menu::run()) {
    return false;
  }

  // display different leds based on the state of the color select
  switch (m_state) {
  case STATE_PICK_SLOT:
    showSlotSelection();
    break;
  case STATE_PICK_HUE1:
    showHueSelection(4);
    break;
  case STATE_PICK_HUE2:
    showHueSelection(16);
    break;
  case STATE_PICK_HUE3:
    showHueSelection(64);
    break;
  case STATE_PICK_SAT1:
    showSatSelection(4);
    break;
  case STATE_PICK_SAT2:
    showSatSelection(16);
    break;
  case STATE_PICK_VAL1:
    showValSelection(4);
    break;
  case STATE_PICK_VAL2:
    showValSelection(16);
    break;
  }

  // blink whichever slot is currently selected regardless of state
  blinkSelection();

  return true;
}

void ColorSelect::onShortClick()
{
  // TODO: proper paging support where it shrinks to the number of colors

  // keep track of pages when in slot selection
  if (m_state == STATE_PICK_SLOT && m_curSelection == FINGER_INDEX) {
    if (m_colorset.numColors() > 3) {
      m_curPage = (m_curPage + 1) % NUM_PAGES;
    }
  }
  // iterate selection forward 
  m_curSelection = (Finger)((m_curSelection + 1) % FINGER_THUMB);
  // wrap if at end of selection
  uint32_t slot = (uint32_t)m_curSelection + (m_curPage * PAGE_SIZE);
  if (slot > m_colorset.numColors()) {
    m_curPage = 0;
    m_curSelection = FINGER_FIRST;
  }
}

void ColorSelect::onLongClick()
{
  switch (m_state) {
  case STATE_PICK_SLOT:
    // store the slot selection
    m_slot = (uint32_t)m_curSelection + (m_curPage * PAGE_SIZE);
    m_state = STATE_PICK_HUE1;
    // reset current page for next time
    m_curPage = 0;
    break;
  case STATE_PICK_HUE1:
    // pick a hue1
    m_newColor.hue = genValue(0, 4, m_curSelection);
    m_state = STATE_PICK_HUE2;
    break;
  case STATE_PICK_HUE2:
    // pick a hue2
    m_newColor.hue = genValue(m_newColor.hue, 16, m_curSelection);
    m_state = STATE_PICK_HUE3;
    break;
  case STATE_PICK_HUE3:
    // pick a hue3
    m_newColor.hue = genValue(m_newColor.hue, 64, m_curSelection);
    m_state = STATE_PICK_SAT1;
    break;
  case STATE_PICK_SAT1:
    // pick a saturation
    m_newColor.sat = genValue(m_newColor.sat, 4, m_curSelection);
    m_state = STATE_PICK_SAT2;
    break;
  case STATE_PICK_SAT2:
    // pick a saturation
    m_newColor.sat = genValue(m_newColor.sat, 16, m_curSelection);
    m_state = STATE_PICK_VAL1;
    break;
  case STATE_PICK_VAL1:
    // pick a value
    m_newColor.val = genValue(m_newColor.val, 4, m_curSelection);
    // go back to beginning for next time
    m_state = STATE_PICK_VAL2;
    break;
  case STATE_PICK_VAL2:
    // pick a value
    m_newColor.val = genValue(m_newColor.val, 16, m_curSelection);
    // replace the slot with the new color
    m_colorset.set(m_slot, m_newColor);
    // switch all colorsets to a copy of m_colorset
    m_pCurMode->changeAllColorsets(&m_colorset);
    // go back to beginning for next time
    m_state = STATE_PICK_SLOT;
    // reset the color
    m_newColor = RGB_OFF;
    // done in the color select menu
    //leaveMenu();
    break;
  }
  // reset selection after choosing anything
  m_curSelection = FINGER_FIRST;
}

void ColorSelect::showSlotSelection()
{
  // the index of the first color to show changes based on the page
  // will be either 0 or 4 for the two page color select
  uint32_t colIndex = (m_curPage * PAGE_SIZE);
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // set the current colorset slot color on the current finger
    Leds::setFinger(f, m_colorset[colIndex++]);
  }
}

void ColorSelect::showHueSelection(uint32_t divisions)
{
  uint32_t hueStart = m_newColor.hue;
  for (LedPos p = PINKIE_TIP; p <= INDEX_TOP; ++p) {
    // generate a hue from the current finger
    Leds::setIndex(p, HSVColor(genValue(m_newColor.hue, divisions * 2, p), 255, 255));
  }
}

void ColorSelect::showSatSelection(uint32_t divisions)
{
  for (LedPos p = PINKIE_TIP; p <= INDEX_TOP; ++p) {
    // generate saturate on current hue from current finger
    Leds::setIndex(p, HSVColor(m_newColor.hue, genValue(m_newColor.sat, divisions * 2, p), 255));
  }
}

void ColorSelect::showValSelection(uint32_t divisions)
{
  for (LedPos p = PINKIE_TIP; p <= INDEX_TOP; ++p) {
    // generate saturate on current hue from current finger
    Leds::setIndex(p, HSVColor(m_newColor.hue, m_newColor.sat, genValue(m_newColor.val, divisions * 2, p)));
  }
}

uint32_t ColorSelect::genValue(uint32_t start, uint32_t divisions, uint32_t amount)
{
  // make a value between 0-255 with even divisions of 255 starting from an offset
  return start + (amount * (255 / divisions));
}
