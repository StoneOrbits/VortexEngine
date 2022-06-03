#include "ColorSelect.h"

#include "../TimeControl.h"
#include "../Colorset.h"
#include "../Button.h"
#include "../Mode.h"
#include "../Leds.h"
#include "../Log.h"

// the number of slots in a page
#define PAGE_SIZE 4
// the number of pages
#define NUM_PAGES 2

ColorSelect::ColorSelect() :
  Menu(),
  m_state(STATE_PICK_SLOT),
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
  m_state = STATE_PICK_SLOT;
  m_curPage = 0;
  m_slot = 0;
  m_quadrant = 0;
  // There is no way to change the colorsets of the rest of the leds past LED_FIRST, that
  // would need a more advanced menu.  So we make a copy of the primary colorset, this is
  // either the colorset of the first individual pattern or if the mode has a multi-led
  // pattern then it's the multi-led pattern's colorset
  m_colorset = *m_pCurMode->getColorset();
  DEBUG_LOG("Entered color select");
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

  // blink whichever slot is currently selected regardless of state
  blinkSelection();

  return true;
}

void ColorSelect::onShortClick()
{
  // keep track of pages when in slot selection
  if (m_state == STATE_PICK_SLOT) {
    // if the current selection is on the index finger then it's at the
    // end of the current page and we might need to go to the next page
    if (m_curSelection == FINGER_THUMB) {
      // even if there is only 4 colors then we need to allow going to the 5th
      // slot on the next page in order to add a color
      if (m_colorset.numColors() > 3) {
        // increase the page number but wrap at max pages which is by default 2
        m_curPage = (m_curPage + 1) % NUM_PAGES;
      }
    }
  }
  // iterate selection forward and wrap after the thumb
  m_curSelection = (Finger)((m_curSelection + 1) % (FINGER_THUMB + 1));
  // only when we're not on thumb calculate the current 'slot' based on page
  if (m_curSelection != FINGER_THUMB && m_state == STATE_PICK_SLOT) {
    // the slot is an index in the colorset, where as curselection is a finger index
    m_slot = (uint32_t)m_curSelection + (m_curPage * PAGE_SIZE);
  }
  if (m_slot > m_colorset.numColors()) {
    if (m_curSelection != FINGER_THUMB) {
      m_curSelection = FINGER_THUMB;
    } else {
      m_curPage = m_slot = 0;
      m_curSelection = FINGER_FIRST;
    }
  }

}

void ColorSelect::onLongClick()
{
  // if we're exiting a menu
  if (m_curSelection == FINGER_THUMB) {
    switch (m_state) {
    case STATE_PICK_SLOT:
      // save the colorset
      m_pCurMode->setColorset(&m_colorset);
      m_pCurMode->init();
      // leave menu
      leaveMenu();
      return;
    case STATE_PICK_HUE1:
      // delete current slot
      m_colorset.removeColor(m_slot);
      if (m_slot > 0) {
        m_slot--;
      }
      m_curSelection = FINGER_FIRST;
      m_state = STATE_PICK_SLOT;
      return;
    case STATE_PICK_HUE2:
    case STATE_PICK_SAT:
    case STATE_PICK_VAL:
    default:
      // bail out without deletion
      m_state = STATE_PICK_SLOT;
      m_curSelection = FINGER_FIRST;
      return;
    }
  }
  switch (m_state) {
  case STATE_PICK_SLOT:
    m_state = STATE_PICK_HUE1;
    // the page is only used for slot selection so reset current page
    // for next time they use the color select
    m_curPage = 0;
    break;
  case STATE_PICK_HUE1:
    // pick a hue1
    m_newColor.hue = m_curSelection * (255 / 4);
    m_state = STATE_PICK_HUE2;
    break;
  case STATE_PICK_HUE2:
    // pick a hue2
    m_newColor.hue = m_newColor.hue + ((255 / 16) * m_curSelection);
    m_state = STATE_PICK_SAT;
    break;
  case STATE_PICK_SAT:
    // pick a saturation
    m_newColor.sat = sats[m_curSelection];
    m_state = STATE_PICK_VAL;
    break;
  case STATE_PICK_VAL:
    // pick a value
    m_newColor.val = vals[m_curSelection];
    // replace the slot with the new color
    m_colorset.set(m_slot, m_newColor);
    // switch all colorsets to a copy of m_colorset
    m_pCurMode->setColorset(&m_colorset);
    // go back to beginning for next time
    m_state = STATE_PICK_SLOT;
    // reset the color
    m_newColor.clear();
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

void ColorSelect::showHueSelection1()
{
  for (LedPos p = PINKIE_TIP; p <= INDEX_TOP; ++p) {
    // generate a hue from the current finger
    Leds::setIndex(p, HSVColor((255 / 8) * p, 255, 255));
  }
}

void ColorSelect::showHueSelection2()
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // generate a hue from the current finger
    Leds::setFinger(f, HSVColor(m_newColor.hue + ((255 / 16) * f), 255, 255));
  }
}

void ColorSelect::showSatSelection()
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // generate saturate on current hue from current finger
    Leds::setFinger(f, HSVColor(m_newColor.hue, sats[f], 255));
  }
}

void ColorSelect::showValSelection()
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // generate saturate on current hue from current finger
    Leds::setFinger(f, HSVColor(m_newColor.hue, m_newColor.sat, vals[f]));
  }
}

uint32_t ColorSelect::genValue(uint32_t start, uint32_t divisions, uint32_t amount)
{
  // make a value between 0-255 with even divisions of 255 starting from an offset
  return start + (amount * (255 / divisions));
}

void ColorSelect::blinkSelection(uint32_t offMs, uint32_t onMs)
{
  // if we're in the slot selection
  if (m_state == STATE_PICK_SLOT) {
    // and the current selected slot is the end slot, except for when we're on the thumb
    if (m_slot == m_colorset.numColors() && m_curSelection != FINGER_THUMB) {
      // clear the finger so it turns off, then blink this slot green
      // to indicate we can add a color here
      Leds::clearFinger(m_curSelection);
      Leds::blinkFinger(m_curSelection, 150, 350, RGB_GREEN);
      return;
    }
  }
  // otherwise run the default blink logic
  Menu::blinkSelection(offMs, onMs);
}
