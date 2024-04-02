#include "ChromaPuzzle.h"

#include "../../Buttons/Buttons.h"
#include "../../Modes/Modes.h"
#include "../../Menus/Menus.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

// number of levels of difficulty
#define NUM_LEVELS 5

// binary bitmasks that describe the swapping pattern for each level
static const uint16_t swapPatterns[NUM_LEVELS] = {
  0b0010010010,
  0b0000000001,
  0b1000000011,
  0b0100000010,
  0b1100000110,
};

ChromaPuzzle::ChromaPuzzle(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_currentLevel(0),
  m_selectedRingInner(false),
  m_ledValuesInner(),
  m_ledValuesOuter(),
  m_swapPattern(0),
  m_rand()
{
}

ChromaPuzzle::~ChromaPuzzle()
{
}

bool ChromaPuzzle::init()
{
  if (!Menu::init()) {
    return false;
  }
  DEBUG_LOG("Entered global brightness");
  // skip led selection
  m_ledSelected = true;
  // initialize the swap pattern on the first level
  m_swapPattern = swapPatterns[m_currentLevel];
  clearRings();
  scramble();
  return true;
}

Menu::MenuAction ChromaPuzzle::run()
{
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }
  // Handle left button press
  if (g_pButtonL->onShortClick()) {
    pressLeft();
  }

  // Handle middle button press
  if (g_pButtonM->onShortClick()) {
    select();
  }

  // Handle right button press
  if (g_pButtonR->onShortClick()) {
    pressRight();
  }

  show();

  // continue
  return MENU_CONTINUE;
}

void ChromaPuzzle::show()
{
  for (uint8_t i = 0; i < LED_COUNT / 2; i++) {
    RGBColor outer = m_ledValuesOuter[i];
    RGBColor inner = m_ledValuesInner[i];
    Leds::setIndex((LedPos)i, outer);
    Leds::setIndex((LedPos)(LED_10 + i), inner);
    if (m_swapPattern & (1 << i)) {
      Leds::blinkIndex((LedPos)i, 300, 50, outer.adjustBrightness(200));
      Leds::blinkIndex((LedPos)(LED_10 + i), 300, 50, inner.adjustBrightness(200));
    }
  }
}

void ChromaPuzzle::pressLeft()
{
  rotateSwapPattern(false);
}

void ChromaPuzzle::pressRight()
{
  rotateSwapPattern(true);
}

void ChromaPuzzle::select()
{
  swapLEDs();
  toggleRingSelection();
  if (checkWinCondition()) {
    if (m_currentLevel == NUM_LEVELS) { // Ensure we don't exceed our pattern array bounds
      // completed all levels
      win();
      return;
    }
    m_currentLevel++;
    m_swapPattern = swapPatterns[m_currentLevel]; // Update the pattern for the new level
    scramble();
    // Other level-up logic...
  }
}

void ChromaPuzzle::clearRings()
{
  for (uint8_t i = 0; i < LED_COUNT / 2; i++) {
    m_ledValuesInner[i] = RGB_BLUE;
    m_ledValuesOuter[i] = RGB_RED;
  }
}

void ChromaPuzzle::rotateRing(bool clockwise)
{
  RGBColor temp;
  RGBColor *selectedRing = m_selectedRingInner ? m_ledValuesInner : m_ledValuesOuter;
  if (clockwise) {
    // Rotate right (clockwise)
    temp = selectedRing[(LED_COUNT / 2) - 1];
    for (uint8_t i = (LED_COUNT / 2) - 1; i > 0; i--) {
      selectedRing[i] = selectedRing[i - 1];
    }
    selectedRing[0] = temp;
  } else {
    // Rotate left (counterclockwise)
    temp = selectedRing[0];
    for (uint8_t i = 0; i < (LED_COUNT / 2) - 1; i++) {
      selectedRing[i] = selectedRing[i + 1];
    }
    selectedRing[(LED_COUNT / 2) - 1] = temp;
  }
}

void ChromaPuzzle::rotateSwapPattern(bool clockwise)
{
  if (clockwise) {
    bool wrap = m_swapPattern & 0b1000000000;
    m_swapPattern <<= 1;
    if (wrap) {
      m_swapPattern |= 1;
    }
  } else {
    bool wrap = m_swapPattern & 0x1;
    m_swapPattern >>= 1;
    if (wrap) {
      m_swapPattern |= 0b1000000000;
    }
  }
}

void ChromaPuzzle::toggleRingSelection()
{
  m_selectedRingInner = !m_selectedRingInner;
}

void ChromaPuzzle::swapLEDs()
{
  for (uint32_t i = 0; i < LED_COUNT / 2; ++i) {
    if (m_swapPattern & (1 << i)) {
      // Swap the ith LED between inner and outer rings
      RGBColor temp = m_ledValuesInner[i];
      m_ledValuesInner[i] = m_ledValuesOuter[i];
      m_ledValuesOuter[i] = temp;
    }
  }
}

void ChromaPuzzle::scramble()
{
  uint32_t scrambleMoves = 25 + (m_currentLevel * 5);

  for (uint32_t i = 0; i < scrambleMoves; ++i) {
    if (m_rand.next8(0, 1)) {
      rotateSwapPattern(m_rand.next8(0, 1));
    } else {
      swapLEDs();
    }
  }

  // reset the swap pattern to start the level
  m_swapPattern = swapPatterns[m_currentLevel];
}

bool ChromaPuzzle::checkWinCondition()
{
  for (uint8_t i = 1; i < LED_COUNT / 2; ++i) {
    if (m_ledValuesInner[i] != m_ledValuesInner[i - 1]) {
      return false;
    }
    if (m_ledValuesOuter[i] != m_ledValuesOuter[i - 1]) {
      return false;
    }
  }
  return true;
}


void ChromaPuzzle::win()
{
  leaveMenu();
}
