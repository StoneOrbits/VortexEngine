#include "ColorSelect.h"

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
  m_slot(0),
  m_quadrant(0),
  m_newColor()
{
}

bool ColorSelect::init(Mode *curMode)
{
  if (!Menu::init(curMode)) {
    return false;
  }
  // copy the colorset from the current mode
  m_colorset = *curMode->getColorset();
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
  case STATE_PICK_QUAD:
    showQuadSelection();
    break;
  case STATE_PICK_HUE:
    showHueSelection();
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
  // TODO: proper paging support where it shrinks to the number of colors

  // keep track of pages when in slot selection
  if (m_state == STATE_PICK_SLOT && m_curSelection == FINGER_INDEX) {
    m_curPage = (m_curPage + 1) % NUM_PAGES;
  }
  // iterate selection forward wrap at thumb
  m_curSelection = (Finger)((m_curSelection + 1) % FINGER_THUMB);
}

void ColorSelect::onLongClick()
{
  switch (m_state) {
  case STATE_PICK_SLOT:
    // store the slot selection
    m_slot = m_curSelection;
    m_state = STATE_PICK_QUAD;
    // reset current page for next time
    m_curPage = 0;
    break;
  case STATE_PICK_QUAD:
    // pick a quadrant, technically there is 8 options ranging
    // from 0 to 7 and the user can put 0, 2, 4, or 6
    m_quadrant = m_curSelection * 2;
    if (g_pButton->holdDuration() > 350) {
        m_quadrant++;
    }
    m_state = STATE_PICK_HUE;
    break;
  case STATE_PICK_HUE:
    // pick a hue
    m_newColor.hue = makeHue(m_quadrant, m_curSelection);
    m_state = STATE_PICK_SAT;
    break;
  case STATE_PICK_SAT:
    // pick a saturation
    m_newColor.sat = makeSat(m_curSelection);
    m_state = STATE_PICK_VAL;
    break;
  case STATE_PICK_VAL:
    // pick a value
    m_newColor.val = makeVal(m_curSelection);
    // replace the slot with the new color
    m_colorset.set(m_slot, m_newColor);
    // switch all colorsets to a copy of m_colorset
    m_pCurMode->changeAllColorsets(&m_colorset);
    // go back to beginning for next time
    m_state = STATE_PICK_SLOT;
    // done in the color select menu
    leaveMenu();
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
    g_pLedControl->setFinger(f, m_colorset[colIndex++]);
  }
}

void ColorSelect::showQuadSelection()
{
  for (LedPos p = PINKIE_TIP; p <= INDEX_TOP; ++p) {
    g_pLedControl->setIndex(p, HSVColor(makeQuad(p), 255, 255));
  }
}

void ColorSelect::showHueSelection()
{
  for (LedPos p = PINKIE_TIP; p <= INDEX_TOP; ++p) {
    // generate a hue from the current finger
    g_pLedControl->setIndex(p, HSVColor(makeHue(m_quadrant, p), 255, 255));
  }
}

void ColorSelect::showSatSelection()
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // generate saturate on current hue from current finger
    g_pLedControl->setFinger(f, HSVColor(m_newColor.hue, makeSat(f), 255));
  }
}

void ColorSelect::showValSelection()
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // generate value on current color and current finger
    g_pLedControl->setFinger(f, HSVColor(m_newColor.hue, m_newColor.sat, makeVal(f)));
  }
}

uint32_t ColorSelect::makeQuad(uint32_t pos)
{
  // quadrant is 90 degrees of hue, which is 63.75 so we divide that by
  // the number of leds to get the amount to increment per led
  return (pos * (255 / 8)); // + offset
}

uint32_t ColorSelect::makeHue(uint32_t pos, uint32_t selection)
{
  // quadrant is base multiple of 90 and hue selection is 0 28 56 84
  return makeQuad(pos) + (selection * ((255 / 8) / 8));
}

uint32_t ColorSelect::makeSat(uint32_t selection)
{
  // 135 base saturation plus increments of 40 to 255
  return 135 + (selection * 40);
}

uint32_t ColorSelect::makeVal(uint32_t selection)
{
  // 135 base value plus increments of 40 to 255
  return 135 + (selection * 40);
}
