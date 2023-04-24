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

Randomizer::Randomizer(const RGBColor &col) :
  Menu(col)
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

  // if they are trying to randomize a multi-led pattern just convert
  // the pattern to all singles with the same colorset upon entry
  if (m_pCurMode->isMultiLed()) {
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

  // initialize the randomseed of each led with the
  // CRC of the colorset on the respective LED
  for (LedPos l = LED_FIRST; l < LED_COUNT; ++l) {
    ByteStream ledData;
    m_pCurMode->getColorset(l).serialize(ledData);
    m_randCtx[l].seed(ledData.recalcCRC());
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

  // display the randomized mode
  if (m_pCurMode) {
    m_pCurMode->play();
  }

  if (g_pButton->isPressed() && g_pButton->holdDuration() > SHORT_CLICK_THRESHOLD_TICKS) {
    Leds::setAll(RGB_DIM_WHITE2);
  }

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
  Random &ctx = m_randCtx[pos];
  // pick a random type of randomizer to use then use
  // the randomizer to generate a random colorset
  uint32_t randType = ctx.next(0, 9);
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
  // create a random pattern ID from all patterns
  PatternID newPat;
  do {
    // continuously re-randomize the pattern so we don't get undesirable patterns
    newPat = (PatternID)ctx.next(PATTERN_FIRST, PATTERN_SINGLE_LAST);
  } while (newPat == PATTERN_SOLID || newPat == PATTERN_RIBBON || newPat == PATTERN_MINIRIBBON);
  // update the led with the new random
  m_pCurMode->setPattern(newPat, pos, nullptr, &randomSet);
  // initialize the mode with the new pattern and colorset
  m_pCurMode->init();
  DEBUG_LOGF("Randomized set with randomization technique %u, %u colors, and Pattern number %u",
    randType, randomSet.numColors(), newPat);
  return true;
}

bool Randomizer::reRoll()
{
  MAP_FOREACH_LED(m_targetLeds) {
    if (!reRoll(pos)) {
      return false;
    }
  }
  return true;
}
