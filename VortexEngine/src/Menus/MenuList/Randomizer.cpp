#include "Randomizer.h"

#include "../../Memory/Memory.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Colors/Colorset.h"
#include "../../Random/Random.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

#include <Arduino.h>

Randomizer::Randomizer(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_lastRandomization(0),
  m_autoMode(false)
{
}

Randomizer::~Randomizer()
{
}

bool Randomizer::init()
{
  if (!Menu::init()) {
    return false;
  }

  if (!m_pCurMode) {
    return false;
  }

  // grab the multi ld pattern colorset crc if it's present
  if (m_pCurMode->hasMultiLed()) {
    ByteStream ledData;
    m_pCurMode->getColorset(LED_MULTI).serialize(ledData);
    m_multiRandCtx.seed(ledData.recalcCRC());
  }

  // initialize the randomseed of each led with the
  // CRC of the colorset on the respective LED
  for (LedPos l = LED_FIRST; l < LED_COUNT; ++l) {
    ByteStream ledData;
    m_pCurMode->getColorset(l).serialize(ledData);
    m_singlesRandCtx[l].seed(ledData.recalcCRC());
  }

  DEBUG_LOG("Entered randomizer");
  return true;
}

Menu::MenuAction Randomizer::run()
{
  // run the base menu logic
  MenuAction result = Menu::run();
  if (result != MENU_CONTINUE) {
    return result;
  }

  // if they are trying to randomize a multi-led pattern just convert
  // the pattern to all singles with the same colorset upon entry
  if (m_pCurMode->isMultiLed() && m_targetLeds != MAP_LED(LED_MULTI)) {
    // convert the pattern to a single led pattern, this will map
    // all multi led patterns to single led patterns using modulo
    // so no matter which multi-led pattern they have selected it
    // will convert to a single led pattern of some kind
    PatternID newID = (PatternID)((m_pCurMode->getPatternID() - PATTERN_MULTI_FIRST) % PATTERN_SINGLE_COUNT);
    // solid sucks
    if (newID == PATTERN_SOLID) ++newID;
    m_pCurMode->setPattern(newID);
    m_pCurMode->init();
  }

  // if the user fast-clicks 3 times then toggle automode
  if (g_pButton->onRelease() && g_pButton->consecutivePresses() == 3) {
    m_autoMode = !m_autoMode;
    // display a quick flash of either green or red to indicate whether auto mode is on or not
    Leds::setAll((m_autoMode ? RGB_GREEN : RGB_RED));
    Leds::update();
    delay(250);
    return MENU_CONTINUE;
  }

  if (m_autoMode && (m_lastRandomization + AUTO_RANDOM_DELAY_TICKS < Time::getCurtime())) {
    m_lastRandomization = Time::getCurtime();
    reRoll();
  }

  // display the randomized mode
  if (m_pCurMode) {
    m_pCurMode->play();
  }

  // show the selection
  showSelect();

  // return true to continue staying in randomizer menu
  return MENU_CONTINUE;
}

void Randomizer::onShortClick()
{
  // shortClick re-roll the randomization
  reRoll();
}

void Randomizer::onLongClick()
{
  // then done here, save if the mode was different
  leaveMenu(true);
}

bool Randomizer::reRoll(LedPos pos)
{
  // colorset that will be filled with random colors
  Colorset randomSet;
  // grab local reference to the target random context
  Random &ctx = (pos < LED_COUNT) ? m_singlesRandCtx[pos] : m_multiRandCtx;
  // pick a random type of randomizer to use then use
  // the randomizer to generate a random colorset
  uint8_t randType = (uint8_t)ctx.next(0, 9);
  switch (randType) {
  default:
  case 0:
    randomSet.randomize(ctx);
    break;
  case 1:
    randomSet.randomizeColorTheory(ctx);
    break;
  case 2:
    randomSet.randomizeMonochromatic(ctx);
    break;
  case 3:
    randomSet.randomizeDoubleSplitComplimentary(ctx);
    break;
  case 4:
    randomSet.randomizeTetradic(ctx);
    break;
  case 5:
    randomSet.randomize(ctx, 1);
    break;
  case 6:
    randomSet.randomizeEvenlySpaced(ctx);
    break;
  case 7:
    randomSet.randomizeEvenlySpaced(ctx, 2);
    break;
  case 8:
    randomSet.randomizeEvenlySpaced(ctx, 3);
    break;
  }

  // take the randomized colorset and repeate colors to make an additional pattern
  if (randomSet.numColors() <= 4 && ctx.next8(0, 1) != 0) {
    // using uint8_t here actually costs more bytes than int16_t
    int16_t startingNumColors = randomSet.numColors();
    for (int16_t i = 0; i < startingNumColors - 1; ++i) {
      // add a duplicate of each color in the colorset to in reverse
      randomSet.addColor(randomSet.get(startingNumColors - (i + 2)));
    }
  }

  // the new pattern id to set
  PatternID newPat;
  // if normal mode generate a random pattern id, advanced don't change the pat id
  if (!m_advanced) {
    // the random range begin/end
    PatternID rbegin = PATTERN_SINGLE_FIRST;
    PatternID rend = PATTERN_SINGLE_LAST;
    // is the multi led present in the target led map
    if (m_targetLeds & MAP_LED(LED_MULTI)) {
      // if so enable that one
      rend = PATTERN_MULTI_LAST;
      if (m_targetLeds == MAP_LED(LED_MULTI)) {
        rbegin = PATTERN_MULTI_FIRST;
      }
    }
    do {
      // continuously re-randomize the pattern so we don't get undesirable patterns
      newPat = (PatternID)ctx.next(rbegin, rend);
    } while (newPat == PATTERN_SOLID || newPat == PATTERN_RIBBON || newPat == PATTERN_MINIRIBBON);
    // update the led with the new random
    m_pCurMode->setPattern(newPat, pos, nullptr, &randomSet);
  } else {
    // update the led with just the colorset
    m_pCurMode->setColorset(randomSet, pos);
  }
  // initialize the mode with the new pattern and colorset
  m_pCurMode->init();
  DEBUG_LOGF("Randomized Led %u set with randomization technique %u, %u colors, and Pattern number %u",
    pos, randType, randomSet.numColors(), newPat);
  return true;
}

bool Randomizer::reRoll()
{
  if (m_targetLeds == MAP_LED(LED_MULTI)) {
    return reRoll(LED_MULTI);
  }
  MAP_FOREACH_LED(m_targetLeds) {
    if (!reRoll(pos)) {
      return false;
    }
  }
  return true;
}
