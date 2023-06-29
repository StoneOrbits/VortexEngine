#include "Randomizer.h"

#include "../../Memory/Memory.h"

#include "../../Patterns/PatternBuilder.h"
#include "../../Patterns/Pattern.h"
#include "../../Colors/Colorset.h"
#include "../../Random/Random.h"
#include "../../Time/Timings.h"
#include "../../Buttons/Button.h"
#include "../../Menus/Menus.h"
#include "../../Modes/Modes.h"
#include "../../Modes/Mode.h"
#include "../../Leds/Leds.h"
#include "../../Log/Log.h"

// toggle this to 0 to disable smart pattern arg generation and just generate random pattern ids
#define USE_SMART_PATTERNS 1

Randomizer::Randomizer(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_lastRandomization(0),
  m_flags(RANDOMIZE_BOTH),
  m_displayHue(0),
  m_needToSelect(advanced),
  m_autoCycle(false)
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

  // if the ranomization flags haven't been set yet just show a selection
  if (m_needToSelect) {
    // display the randomization selection menu
    showRandomizationSelect();
    return MENU_CONTINUE;
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
    // toggle the auto cycle flag
    m_autoCycle = !m_autoCycle;
    // display a quick flash of either green or red to indicate whether auto mode is on or not
    Leds::holdIndex(LED_ALL, 250, (m_autoCycle ? RGB_GREEN : RGB_RED));
    return MENU_CONTINUE;
  }

  if (m_autoCycle && (m_lastRandomization + AUTO_RANDOM_DELAY_TICKS < Time::getCurtime())) {
    m_lastRandomization = Time::getCurtime();
    reRoll();
  }

  // display the randomized mode
  if (m_pCurMode) {
    m_pCurMode->play();
  }

  // show the selection
  Menus::showSelection();

  // return true to continue staying in randomizer menu
  return MENU_CONTINUE;
}

void Randomizer::onShortClick()
{
  if (m_needToSelect) {
    if (m_flags == RANDOMIZE_BOTH) {
      m_flags = RANDOMIZE_PATTERN;
    } else {
      m_flags = (RandomizeFlags)(m_flags + 1);
    }
  }
  // shortClick re-roll the randomization
  reRoll();
}

void Randomizer::onLongClick()
{
  // if done the randomization selection part
  if (m_needToSelect) {
    // we are complete the randomization selection stage
    m_needToSelect = false;
    return;
  }
  // then done here, save if the mode was different
  leaveMenu(true);
}

void Randomizer::showRandomizationSelect()
{
  // show iterating rainbow if they are randomizing color, otherwise 0 sat if they
  // are only randomizing the pattern
  Leds::setAll(HSVColor(m_displayHue++, (m_flags & RANDOMIZE_COLORSET) * 255, 84));
  if (m_flags & RANDOMIZE_PATTERN) {
    // if they are randomizing the pattern strobe on/off
    Leds::blinkAll(Time::getCurtime(), 8, 15);
  }
  // render the click selection blink
  Menus::showSelection();
}

Colorset Randomizer::rollColorset(Random &ctx)
{
  Colorset randomSet;
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
  return randomSet;
}

bool Randomizer::rollPattern(Random &ctx, Mode *pMode, LedPos pos)
{
  uint8_t numCols = pMode->getColorset(pos).numColors();
  PatternArgs args(
    ctx.next8(3, 20),  // on duration 3 -> 20
    ctx.next8(0, 50),  // off duration 0 -> 50
    ctx.next8(0, 140) & 0xF8,  // gap duration 0 -> 100
    ctx.next8(0, 15),  // dash duration 0 -> 15
    ctx.next8(0, numCols >> 1) // group 0 -> numColors / 2
  );
  PatternID newPat = PATTERN_BASIC;
  // 1/5 chance for blend, 1/5 chance for solid, 3/5 chance for strobe
  switch (ctx.next8() % 3) {
  case 0:
    newPat = PATTERN_BLEND;
    args.arg7 = ctx.next8(); // hue offset? kinda pointless
    args.arg8 = ctx.next8(0, 3); // num flips 0 to 3
    break;
  case 1:
    newPat = PATTERN_SOLID;
    args.arg7 = ctx.next8(0, numCols); // solid index
    break;
  default:
    break;
  }
  return pMode->setPattern(newPat, pos, &args);
}

PatternID Randomizer::rollPatternID(Random &ctx)
{
  PatternID newPat;
  // the random range begin/end
  do {
    // continuously re-randomize the pattern so we don't get undesirable patterns
    newPat = (PatternID)ctx.next(PATTERN_SINGLE_FIRST, PATTERN_SINGLE_LAST);
  } while (newPat == PATTERN_SOLID || newPat == PATTERN_RIBBON || newPat == PATTERN_MINIRIBBON);
  return newPat;
}

bool Randomizer::reRoll()
{
  MAP_FOREACH_LED(m_targetLeds) {
    // grab local reference to the target random context
    Random &ctx = m_singlesRandCtx[pos];
    if (m_flags & RANDOMIZE_PATTERN) {
      // roll a new pattern
#if USE_SMART_PATTERNS == 1
      if (!rollPattern(ctx, m_pCurMode, pos)) {
        ERROR_LOG("Failed to roll new pattern");
        return false;
      }
#else
      if (!m_pCurMode->setPattern(rollPatternID(ctx), pos)) {
        ERROR_LOG("Failed to roll new pattern");
        return false;
      }
#endif
    }
    if (m_flags & RANDOMIZE_COLORSET) {
      // roll a new colorset
      if (!m_pCurMode->setColorset(rollColorset(ctx), pos)) {
        ERROR_LOG("Failed to roll new colorset");
        return false;
      }
    }
    // initialize the mode with the new pattern and colorset
    m_pCurMode->init();
    DEBUG_LOGF("Randomized Led %u set with randomization technique %u, %u colors, and Pattern number %u",
      pos, randType, randomSet.numColors(), newPat);
  }
  return true;
}
