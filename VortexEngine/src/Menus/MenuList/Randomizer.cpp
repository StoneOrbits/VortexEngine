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

Randomizer::Randomizer(const RGBColor &col, bool advanced) :
  Menu(col, advanced),
  m_lastRandomization(0),
  m_flags(advanced ? RANDOMIZE_COLORSET : RANDOMIZE_BOTH),
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
  // grab the multi ld pattern colorset crc if it's present
  Mode *cur = Modes::curMode();
  // initialize the randomseed of each led with the
  // CRC of the colorset on the respective LED
  for (LedPos l = LED_FIRST; l < LED_COUNT; ++l) {
    ByteStream ledData;
    Pattern *pat = cur->getPattern(l);
    if (pat) {
      pat->serialize(ledData);
    }
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
  // if the user fast-clicks 3 times then toggle automode
  if (g_pButton->onRelease() && g_pButton->onConsecutivePresses(AUTO_CYCLE_RANDOMIZER_CLICKS)) {
    // toggle the auto cycle flag
    m_autoCycle = !m_autoCycle;
    // display a quick flash of either green or red to indicate whether auto mode is on or not
    Leds::holdAll(m_autoCycle ? RGB_GREEN : RGB_RED);
    return MENU_CONTINUE;
  }
  uint32_t now = Time::getCurtime();
  if (m_autoCycle && (m_lastRandomization + AUTO_RANDOM_DELAY_TICKS < now)) {
    m_lastRandomization = now;
    reRoll();
  }
  // display the randomized mode
  m_previewMode.play();
  // show the selection
  Menus::showSelection();
  // return true to continue staying in randomizer menu
  return MENU_CONTINUE;
}

void Randomizer::onShortClick()
{
  if (m_needToSelect) {
    if (m_flags == RANDOMIZE_BOTH) {
      m_flags = RANDOMIZE_COLORSET;
    } else {
      m_flags = (RandomizeFlags)(m_flags + 1);
    }
    return;
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
  // update the current mode with the new randomized mode
  Modes::updateCurMode(&m_previewMode);
  // then done here, save if the mode was different
  leaveMenu(true);
}

void Randomizer::showRandomizationSelect()
{
  // show iterating rainbow if they are randomizing color, otherwise 0 sat if they
  // are only randomizing the pattern
  Leds::setAll(HSVColor(m_displayHue++, (m_flags & RANDOMIZE_COLORSET) * 255, 84));
  if (m_flags & RANDOMIZE_PATTERN) {
    // this is blinking the light to off so the params are switched but still effectively correct
    Leds::blinkAll(DOPS_ON_DURATION, DOPS_OFF_DURATION);
  }
  // indicate on the 2nd led whether the button is pressed
  Leds::setIndex(LED_1, g_pButton->isPressed() ? RGB_OFF : RGB_WHITE1);
  // render the click selection blink
  Menus::showSelection();
}

Colorset Randomizer::rollColorset(Random &ctx)
{
  Colorset randomSet;
  // pick a random type of randomizer to use then use
  // the randomizer to generate a random colorset
  uint8_t randType = ctx.next8(0, 8);
  switch (randType) {
  default:
  case 0:
    randomSet.randomize(ctx);
    break;
  case 1:
    randomSet.randomizeColors(ctx, 0, Colorset::ColorMode::THEORY);
    break;
  case 2:
    randomSet.randomizeColors(ctx, 0, Colorset::ColorMode::MONOCHROMATIC);
    break;
  case 3:
    randomSet.randomizeColors2(ctx, Colorset::ColorMode2::DOUBLE_SPLIT_COMPLIMENTARY);
    break;
  case 4:
    randomSet.randomizeColors2(ctx, Colorset::ColorMode2::TETRADIC);
    break;
  case 5:
    randomSet.randomize(ctx, 1);
    break;
  case 6:
    randomSet.randomizeColors(ctx, 0, Colorset::ColorMode::EVENLY_SPACED);
    break;
  case 7:
    randomSet.randomizeColors(ctx, 2, Colorset::ColorMode::EVENLY_SPACED);
    break;
  case 8:
    randomSet.randomizeColors(ctx, 3, Colorset::ColorMode::EVENLY_SPACED);
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
  PatternArgs args;
  // pick a random type of randomizer to use then use
  // the randomizer to generate a random pattern
  uint8_t patternType = ctx.next8(0, 3);
  switch (patternType) {
  default:
  case 0:
    traditionalPattern(ctx, args);
    break;
  case 1:
    gapPattern(ctx, args);
    break;
  case 2:
    dashPattern(ctx, args);
    break;
  case 3:
    crushPattern(ctx, args);
    break;
  }
  PatternID newPat = PATTERN_STROBE;
  // 1/3 chance to roll a blend pattern instead which will animate between
  // colors instead of blinking each color in the set
  if (!ctx.next8(0, 3)) {
    newPat = PATTERN_BLEND;
    // this is the number of blinks to a complementary color
    args.arg7 = ctx.next8(0, 3);
    // up to arg7 is filled now
    args.numArgs = 7;
  }
  return pMode->setPattern(newPat, pos, &args);
}

void Randomizer::traditionalPattern(Random &ctx, PatternArgs &outArgs)
{
  // call next8 explicitly in this order because the order they
   // are called is undefined when called as parameters to another function.
   // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
   // So different compilers may produce different results,
   // but like this it is explicit
  uint8_t off = ctx.next8(8, 60);   // off duration 0 -> 60
  uint8_t on = ctx.next8(1, 20);    // on duration 1 -> 20
  outArgs.init(on, off);
}

void Randomizer::gapPattern(Random &ctx, PatternArgs &outArgs)
{
  // call next8 explicitly in this order because the order they
  // are called is undefined when called as parameters to another function.
  // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
  // So different compilers may produce different results,
  // but like this it is explicit
  uint8_t gap = ctx.next8(40, 100); // gap duration 40 -> 100
  uint8_t off = ctx.next8(0, 6);    // off duration 0 -> 6
  uint8_t on = ctx.next8(1, 10);    // on duration 1 -> 10
  outArgs.init(on, off, gap);
}

void Randomizer::dashPattern(Random &ctx, PatternArgs &outArgs)
{
  // call next8 explicitly in this order because the order they
  // are called is undefined when called as parameters to another function.
  // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
  // So different compilers may produce different results,
  // but like this it is explicit
  uint8_t dash = ctx.next8(20, 30); // dash duration 20 -> 30
  uint8_t gap = ctx.next8(20, 30);  // need gap 20 -> 30
  uint8_t off = ctx.next8(0, 10);   // off duration 0 -> 10
  uint8_t on = ctx.next8(1, 10);    // on duration 1 -> 10
  outArgs.init(on, off, gap, dash);
}

void Randomizer::crushPattern(Random &ctx, PatternArgs &outArgs)
{
  // call next8 explicitly in this order because the order they
   // are called is undefined when called as parameters to another function.
   // ex: f(a,b,c) may call in the order a,b,c or c,b,a depending on compiler.
   // So different compilers may produce different results,
   // but like this it is explicit
  uint8_t group = ctx.next8(0, 8);  // groupsize 0 to 8
  uint8_t dash = 0;                 // dash 0
  uint8_t gap = ctx.next8(20, 40);  // need gap 20 -> 40
  uint8_t off = ctx.next8(0, 10);   // off duration 0 -> 5
  uint8_t on = ctx.next8(1, 10);    // on duration 1 -> 10
  outArgs.init(on, off, gap, dash, group);
}

PatternID Randomizer::rollPatternID(Random &ctx)
{
  PatternID newPat;
  // the random range begin/end
  do {
    // continuously re-randomize the pattern so we don't get undesirable patterns
    newPat = (PatternID)ctx.next8(PATTERN_SINGLE_FIRST, PATTERN_SINGLE_LAST);
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
      if (m_advanced) {
        if (!rollPattern(ctx, &m_previewMode, pos)) {
          ERROR_LOG("Failed to roll new pattern");
          return false;
        }
      } else {
        if (!m_previewMode.setPattern(rollPatternID(ctx), pos)) {
          ERROR_LOG("Failed to roll new pattern");
          return false;
        }
      }
    }
    if (m_flags & RANDOMIZE_COLORSET) {
      // roll a new colorset
      if (!m_previewMode.setColorset(rollColorset(ctx), pos)) {
        ERROR_LOG("Failed to roll new colorset");
        return false;
      }
    }
  }
  // initialize the mode with the new pattern and colorset
  m_previewMode.init();
  DEBUG_LOGF("Randomized Led %u set with randomization technique %u, %u colors, and Pattern number %u",
    pos, randType, randomSet.numColors(), newPat);
  return true;
}
