#include "ColorSelect.h"

#include "../timeControl.h"
#include "../LedControl.h"
#include "../Colorset.h"
#include "../Mode.h"

// color select only allows for up to 8 slots to be changed
#define NUM_SLOTS 8

// the number of slots in a page
#define PAGE_SIZE 4

ColorSelect::ColorSelect() :
  Menu(),
  m_state(STATE_PICK_SLOT),
  m_pColorset(nullptr),
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
  // grab a pointer to the colorset of the mode
  m_pColorset = curMode->getColorset();
  return true;
}

bool ColorSelect::run(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  // handle base menu logic
  if (!Menu::run(timeControl, button, ledControl)) {
    return false;
  }

  // display different leds based on the state of the color select
  switch (m_state) {
  case STATE_PICK_SLOT:
    showSlotSelection(timeControl, button, ledControl);
    break;
  case STATE_PICK_QUAD:
    showQuadSelection(timeControl, button, ledControl);
    break;
  case STATE_PICK_HUE:
    showHueSelection(timeControl, button, ledControl);
    break;
  case STATE_PICK_SAT:
    showSatSelection(timeControl, button, ledControl);
    break;
  case STATE_PICK_VAL:
    showValSelection(timeControl, button, ledControl);
    break;
  }

  // blink whichever slot is currently selected regardless of state
  blinkSelection(timeControl, ledControl);

  return true;
}

void ColorSelect::onShortClick()
{
  switch (m_state) {
  case STATE_PICK_SLOT:
    // iterate to the next slot
    m_curSelection = (m_curSelection + 1) % NUM_SLOTS;
    break;
  case STATE_PICK_QUAD:
  case STATE_PICK_HUE:
  case STATE_PICK_SAT:
  case STATE_PICK_VAL:
    // only 4 options for quad/hue/sat/val
    m_curSelection = (m_curSelection + 1) % 4;
    break;
  }
}

void ColorSelect::onLongClick()
{
  switch (m_state) {
  case STATE_PICK_SLOT:
    // store the slot selection
    m_slot = m_curSelection;
    m_state = STATE_PICK_QUAD;
    break;
  case STATE_PICK_QUAD:
    // pick a quadrant
    m_quadrant = m_curSelection;
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
    // store the new color
    m_pColorset->set(m_slot, m_newColor);
    // go back to beginning for next time
    m_state = STATE_PICK_SLOT;
    // done in the color select menu
    leaveMenu();
    break;
  }
  // reset selection after choosing anything
  m_curSelection = 0;
}

// TODO: make this part of Menu? (need to standardize selections)
void ColorSelect::blinkSelection(const TimeControl *timeControl, LedControl *ledControl)
{
  // only blink off for 250ms per second
  if ((timeControl->getCurtime() % 1000) < 750) {
    return;
  }
  // everything in colorselect is on pages of 4
  switch (m_curSelection % 4) {
  case 0: // pinkie
    ledControl->clearRange(PINKIE_TOP, PINKIE_TIP);
    break;
  case 1: // ring
    ledControl->clearRange(RING_TOP, RING_TIP);
    break;
  case 2: // mid
    ledControl->clearRange(MIDDLE_TOP, MIDDLE_TIP);
    break;
  case 3: // index
    ledControl->clearRange(INDEX_TOP, INDEX_TIP);
    break;
  }
}

void ColorSelect::showSlotSelection(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  // the index of the first color to show changes based on the page
  // will be either 0 or 4 for the two page color select
  uint32_t colIndex = (curPage() * PAGE_SIZE);
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // set the current colorset slot color on the current finger
    ledControl->setFinger(f, m_pColorset->get(colIndex++));
  }
}

void ColorSelect::showQuadSelection(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // hue split into 4 quadrants of 90
    ledControl->setFinger(f, HSVColor(f * 90, 255, 255));
  }
}

void ColorSelect::showHueSelection(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // generate a hue from the current finger
    ledControl->setFinger(f, HSVColor(makeHue(m_quadrant, f), 255, 255));
  }
}

void ColorSelect::showSatSelection(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // generate saturate on current hue from current finger
    ledControl->setFinger(f, HSVColor(m_newColor.hue, makeSat(f), 255));
  }
}

void ColorSelect::showValSelection(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  for (Finger f = FINGER_PINKIE; f <= FINGER_INDEX; ++f) {
    // generate value on current color and current finger
    ledControl->setFinger(f, HSVColor(m_newColor.hue, m_newColor.sat, makeVal(f)));
  }
}

uint32_t ColorSelect::makeHue(uint32_t quad, uint32_t selection)
{
  // quadrant is base multiple of 90 and hue selection is 0 28 56 84
  return (quad * 90) + (selection * 28);
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

uint32_t ColorSelect::curPage()
{
  // pages start at 0
  return (m_curSelection / PAGE_SIZE) - 1;
}
