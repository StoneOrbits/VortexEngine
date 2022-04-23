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

  // display different results based on the state of the color select
  switch (m_state) {
  case STATE_PICK_SLOT:
    pickSlot(timeControl, button, ledControl);
    break;
  case STATE_PICK_QUAD:
    pickQuad(timeControl, button, ledControl);
    break;
  case STATE_PICK_HUE:
    pickHue(timeControl, button, ledControl);
    break;
  case STATE_PICK_SAT:
    pickSat(timeControl, button, ledControl);
    break;
  case STATE_PICK_VAL:
    pickVal(timeControl, button, ledControl);
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
    m_newColor.hue = make_hue(m_quadrant, m_curSelection);
    m_state = STATE_PICK_SAT;
    break;
  case STATE_PICK_SAT:
    // pick a saturation
    m_newColor.sat = make_sat(m_curSelection);
    m_state = STATE_PICK_VAL;
    break;
  case STATE_PICK_VAL:
    // pick a value
    m_newColor.val = make_val(m_curSelection);
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

void ColorSelect::pickSlot(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  // only show PAGE_SIZE slots per page
  uint32_t page = m_curSelection % PAGE_SIZE;
  for (uint32_t i = 0; i < PAGE_SIZE; ++i) {
    // there is 2 leds per finger
    LedPos start = (LedPos)(i * 2);
    LedPos end = start + 1;
    // the index
    uint32_t colIndex = (page * PAGE_SIZE) + i;
    // set the two leds on the finger to the color slot
    ledControl->setRange(start, end, m_pColorset->get(colIndex));
  }
}

void ColorSelect::pickQuad(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  for (uint32_t i = 0; i < PAGE_SIZE; ++i) {
    // there is 2 leds per finger
    LedPos start = (LedPos)(i * 2);
    LedPos end = start + 1;
    // hue split into 4 quadrants of 90
    HSVColor col(i * 90, 255, 255);
    // set the two leds on the finger to the color slot
    ledControl->setRange(start, end, col);
  }
}

void ColorSelect::pickHue(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  for (uint32_t i = 0; i < PAGE_SIZE; ++i) {
    // there is 2 leds per finger
    LedPos start = (LedPos)(i * 2);
    LedPos end = start + 1;
    // split the quadrant into several hues
    HSVColor col(make_hue(m_quadrant, i), 255, 255);
    // set the two leds on the finger to the color slot
    ledControl->setRange(start, end, col);
  }
}

void ColorSelect::pickSat(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  for (uint32_t i = 0; i < PAGE_SIZE; ++i) {
    // there is 2 leds per finger
    LedPos start = (LedPos)(i * 2);
    LedPos end = start + 1;
    // use the new color created by choosing hue
    HSVColor col = m_newColor;
    // adjust it's sat based on the index
    col.sat = make_sat(i);
    // set the two leds on the finger to the color slot
    ledControl->setRange(start, end, col);
  }
}

void ColorSelect::pickVal(const TimeControl *timeControl, const Button *button, LedControl *ledControl)
{
  for (uint32_t i = 0; i < PAGE_SIZE; ++i) {
    // there is 2 leds per finger
    LedPos start = (LedPos)(i * 2);
    LedPos end = start + 1;
    // use the new color created by choosing hue
    HSVColor col = m_newColor;
    // adjust it's val based on the index
    col.val = make_val(i);
    // set the two leds on the finger to the color slot
    ledControl->setRange(start, end, col);
  }
}

uint32_t ColorSelect::make_hue(uint32_t quad, uint32_t selection)
{
  // quadrant is base multiple of 90 and hue selection is 0 28 56 84
  return (quad * 90) + (selection * 28);
}

uint32_t ColorSelect::make_sat(uint32_t selection)
{
  // 135 base saturation plus increments of 40 to 255
  return 135 + (selection * 40);
}

uint32_t ColorSelect::make_val(uint32_t selection)
{
  // 135 base value plus increments of 40 to 255
  return 135 + (selection * 40);
}

