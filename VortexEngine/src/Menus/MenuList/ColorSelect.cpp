#include "ColorSelect.h"

#include "../../Time/TimeControl.h"
#include "../../Time/Timings.h"
#include "../../Colors/Colorset.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

// the number of slots in a page
#define PAGE_SIZE 4
// the number of pages
#define NUM_PAGES 2

ColorSelect::ColorSelect(const RGBColor &col) :
  Menu(col),
  m_state(STATE_PICK_SLOT),
  m_curPage(0),
  m_slot(0),
  m_quadrant(0),
  m_newColor()
{
}

ColorSelect::~ColorSelect()
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

Menu::MenuAction ColorSelect::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
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

  return MENU_CONTINUE;
}

void ColorSelect::onShortClick()
{
  // keep track of pages when in slot selection
  if (m_state == STATE_PICK_SLOT) {
    // if the current selection is on the index finger then it's at the
    // end of the current page and we might need to go to the next page
    if ((m_curSelection == QUADRANT_4 && (m_curPage == 0 && m_colorset.numColors() > 3))) {
      // increase the page number to 1
      m_curPage = (m_curPage + 1) % NUM_PAGES;
      // skip past the thumb if we're on index
      m_curSelection = QUADRANT_LAST;
      // clear all leds because we went to the next page
      Leds::clearAll();
    } else if (m_curSelection == QUADRANT_LAST && (m_curPage == 1 || (m_curPage == 0 && m_colorset.numColors() <= 3))) {
      m_curPage = 0;
      // skip past the thumb if we're on index
      m_curSelection = QUADRANT_LAST;
      // clear all leds because we went to the next page
      Leds::clearAll();
    }
  }
  // iterate selection forward and wrap after the thumb
  m_curSelection = (Quadrant)((m_curSelection + 1) % (QUADRANT_LAST + 1));
  // only when we're not on thumb calculate the current 'slot' based on page
  if (m_curSelection != QUADRANT_LAST && m_state == STATE_PICK_SLOT) {
    // the slot is an index in the colorset, where as curselection is a finger index
    m_slot = (uint32_t)m_curSelection + (m_curPage * PAGE_SIZE);
  }
  if (m_slot > m_colorset.numColors()) {
    if (m_curSelection != QUADRANT_LAST) {
      m_curSelection = QUADRANT_LAST;
    } else {
      m_curPage = m_slot = 0;
      m_curSelection = QUADRANT_FIRST;
    }
  }
}

void ColorSelect::onShortClick2()
{
  // keep track of pages when in slot selection
  if (m_state == STATE_PICK_SLOT) {
    // if the current selection is on the index finger then it's at the
    // end of the current page and we might need to go to the next page
    if (m_curSelection == QUADRANT_1 && m_curPage > 0) {
      // increase the page number to 1
      if (!m_curPage) {
        m_curPage = NUM_PAGES - 1;
      } else {
        --m_curPage;
      }
      // skip past the
      m_curSelection = QUADRANT_5;
      // clear all leds because we went to the next page
      Leds::clearAll();
    } else if (m_curSelection == QUADRANT_1 && (m_curPage > 0 || (m_curPage == 0 && m_colorset.numColors() > 3))) {
      if (!m_curPage) {
        m_curPage = NUM_PAGES - 1;
      } else {
        --m_curPage;
      }
      // skip past the thumb if we're on index
      m_curSelection = QUADRANT_5;
      // clear all leds because we went to the next page
      Leds::clearAll();
      // if we are moving backwards from the first slot in a 4 color set
    } else if (m_curSelection == QUADRANT_5) {
      m_curSelection = (Quadrant)((m_colorset.numColors() + 1) % PAGE_SIZE);
      Leds::clearAll();
    }
  }
  // iterate selection backward and wrap after the thumb
  if (!m_curSelection) {
    if (!m_curPage) {
      m_curSelection = QUADRANT_5;
    } else {
      m_curSelection = QUADRANT_LAST;
    }
  } else {
    m_curSelection = m_curSelection - 1;
  }
  if (m_curSelection != QUADRANT_5 && m_state == STATE_PICK_SLOT) {
    // the slot is an index in the colorset, where as curselection is a finger index
    m_slot = (uint32_t)m_curSelection + (m_curPage * PAGE_SIZE);
  }
  if (m_slot > m_colorset.numColors()) {
    if (m_curSelection != QUADRANT_5) {
      m_curSelection = QUADRANT_5;
    } else {
      m_curPage = m_slot = 0;
      m_curSelection = QUADRANT_FIRST;
    }
  }
}

void ColorSelect::onLongClick()
{
  bool needsSave = false;
  // if we're exiting a menu
  if (m_curSelection == QUADRANT_LAST) {
    // leaving a menu, clear everything
    Leds::clearAll();
    switch (m_state) {
    case STATE_PICK_SLOT:
    default:
      // need to save if the colorset is not equal
      needsSave = !m_colorset.equals(m_pCurMode->getColorset());
      // if we need to save, then actually update the colorset
      if (needsSave) {
        // save the colorset
        m_pCurMode->setColorset(&m_colorset);
        m_pCurMode->init();
      }
      // leave menu and save if we made changes
      leaveMenu(needsSave);
      return;
    case STATE_PICK_HUE1:
      m_state = STATE_PICK_SLOT;
      // reset selection and page based on chosen slot
      m_curSelection = (Quadrant)((m_slot % PAGE_SIZE));
      m_curPage = m_slot / PAGE_SIZE;
      return;
    case STATE_PICK_HUE2:
    case STATE_PICK_SAT:
    case STATE_PICK_VAL:
      m_state = (ColorSelectState)(m_state - 1);
      m_curSelection = QUADRANT_LAST;
      return;
    }
  }
  switch (m_state) {
  case STATE_PICK_SLOT:
    if (m_slot < m_colorset.numColors() &&
      g_pButton->holdDuration() >= DELETE_THRESHOLD_TICKS &&
      (g_pButton->holdDuration() % (DELETE_CYCLE_TICKS * 2)) >(DELETE_CYCLE_TICKS)) {
      // delete current slot
      m_colorset.removeColor(m_slot);
      if (m_slot > m_colorset.numColors()) {
        m_slot--;
      }
      m_curSelection = (Quadrant)((m_slot % PAGE_SIZE));
      m_curPage = m_slot / PAGE_SIZE;
      return;
    }
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
    // replace the slot with the new color, this might
    // result in adding a new color if the slot is at
    // hightest color index + 1
    m_colorset.set(m_slot, m_newColor);
    // go back to beginning for next time
    m_state = STATE_PICK_SLOT;
    // reset the color
    m_newColor.clear();
    // go back to the slot we just added
    m_curSelection = (Quadrant)((m_slot % PAGE_SIZE));
    m_curPage = m_slot / PAGE_SIZE;

    // return instead of break so the cur selection isn't reset
    return;
  }
  // reset selection after choosing anything
  m_curSelection = QUADRANT_FIRST;
}

void ColorSelect::onLongClick2()
{
  // leaving a menu, clear everything
  Leds::clearAll();
  switch (m_state) {
  case STATE_PICK_SLOT:
  default:
    // leave menu without saving
    leaveMenu();
    return;
  case STATE_PICK_HUE1:
    m_state = STATE_PICK_SLOT;
    // reset selection and page based on chosen slot
    m_curSelection = (Quadrant)((m_slot % PAGE_SIZE));
    m_curPage = m_slot / PAGE_SIZE;
    return;
  case STATE_PICK_HUE2:
  case STATE_PICK_SAT:
  case STATE_PICK_VAL:
    m_state = (ColorSelectState)(m_state - 1);
    return;
  }
}

void ColorSelect::showSlotSelection()
{
  // the index of the first color to show changes based on the page
  // will be either 0 or 4 for the two page color select
  uint32_t idx = (m_curPage * PAGE_SIZE);
  for (Quadrant f = QUADRANT_FIRST; f <= QUADRANT_4; ++f) {
    // set the current colorset slot color on the current finger
    // display the extra slots as solid blank
    Leds::setQuadrant(f, (idx >= m_colorset.numColors()) ? RGB_BLANK : m_colorset[idx]);
    idx++;
  }
}

void ColorSelect::showHueSelection1()
{
  for (Pair p = PAIR_FIRST; p < PAIR_COUNT; ++p) {
    Leds::setPair(p, HSVColor((256 / PAIR_COUNT) * p, 255, 255));
  }
}

void ColorSelect::showHueSelection2()
{
  for (Quadrant f = QUADRANT_FIRST; f <= QUADRANT_4; ++f) {
    Leds::setQuadrant(f, HSVColor(m_newColor.hue + ((255 / 16) * f), 255, 255));
  }
}

void ColorSelect::showSatSelection()
{
  for (Quadrant f = QUADRANT_FIRST; f <= QUADRANT_4; ++f) {
    Leds::setQuadrant(f, HSVColor(m_newColor.hue, sats[f], 255));
  }
}

void ColorSelect::showValSelection()
{
  for (Quadrant f = QUADRANT_FIRST; f <= QUADRANT_4; ++f) {
    Leds::setQuadrant(f, HSVColor(m_newColor.hue, m_newColor.sat, vals[f]));
  }
}

void ColorSelect::blinkSelection(uint32_t offMs, uint32_t onMs)
{
  // if we're in the slot selection
  if (m_state == STATE_PICK_SLOT) {
    // and the current selected slot is the end slot, except for when we're on the thumb
    if (m_slot == m_colorset.numColors() && m_curSelection != QUADRANT_LAST) {
      // clear the finger so it turns off, then blink this slot to either
      // white or dim white to indicate we can add a color here
      Leds::clearQuadrant(m_curSelection);
      Leds::blinkQuadrant(m_curSelection, Time::getCurtime(), 150, 350,
        g_pButton->isPressed() ? RGB_DIM_WHITE1 : RGB_DIM_WHITE2);
      return;
    } else if (m_slot < m_colorset.numColors() &&
      g_pButton->isPressed() &&
      g_pButton->holdDuration() >= DELETE_THRESHOLD_TICKS) {
      // if we're pressing down on a slot then glow the tip white/red
      if ((g_pButton->holdDuration() % (DELETE_CYCLE_TICKS * 2)) > DELETE_CYCLE_TICKS) {
        // breath red instead of white blink
        Leds::breathQuadrant(m_curSelection, 0, g_pButton->holdDuration());
        return;
      }
    } else if (m_slot == m_colorset.numColors() + 1) {
      Leds::blinkQuadrantFive(Time::getCurtime(), 150, 350, RGB_WHITE);
    }
  }
  // otherwise run the default blink logic
  Menu::blinkSelection(offMs, onMs);
}
