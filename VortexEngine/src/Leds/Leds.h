#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <inttypes.h>

#include "../Colors/ColorTypes.h"
#include "../VortexConfig.h"

#if FIXED_LED_COUNT == 0
#include <vector>
// drop-in LedPos constant replacements
#define LED_COUNT Leds::ledCount()
#define LED_LAST Leds::ledLast()
#define LED_ALL LED_COUNT
#define LED_MULTI Leds::ledMulti()
#define LED_ALL_SINGLE Leds::ledAllSingle()
#define LED_ANY Leds::ledAny()
#endif

// Defined the LED positions, their order, and index
enum LedPos : uint8_t
{
  // this should always be first
  LED_FIRST = 0,

  // LED constants to supplement pattern code
  LED_0 = LED_FIRST,
  LED_1,
  LED_2,
  LED_3,
  LED_4,
  LED_5,
  LED_6,
  LED_7,
  LED_8,
  LED_9,

#if FIXED_LED_COUNT == 1
  // the number of entries above
  LED_COUNT,

  // the last LED index
  LED_LAST = (LED_COUNT - 1),

  // target all leds (multi and single)
  // When fetching this the same as LED_ANY
  // When setting this will set all of the leds
  LED_ALL = LED_COUNT,

  // target the multi led slot
  //
  // When fetching this will return the multi led slot
  // When setting this will set the multi led slot
  LED_MULTI = (LED_COUNT + 1),

  // target all single led slots
  //
  // When fetching this will return the first single led slot
  // When setting this will set all single led slots
  LED_ALL_SINGLE = (LED_COUNT + 2),

  // Target the 'effective' led slot (any slot)
  //
  // When fetching this will:
  //    1. return the multi led slot if it exists
  //    2. otherwise the first single led slot
  // 
  // When setting this will:
  //    1. if setting single led pattern will set all
  //    2. if setting multi led pattern will set multi
  LED_ANY = (LED_COUNT + 3),

  // other customs?
  // LED_EVENS = (LED_COUNT + 2),
  // LED_ODDS = (LED_COUNT + 3),
#endif
};

enum Pair : uint8_t
{
  PAIR_FIRST = 0,

  // one pair for each pair of leds, adjust this to be 2x the LED_COUNT
  PAIR_0 = PAIR_FIRST,
  PAIR_1,
  PAIR_2,
  PAIR_3,
  PAIR_4,

  PAIR_COUNT,
  PAIR_LAST = (PAIR_COUNT - 1),
};

// LedMap is a bitmap of leds, used for expressing whether to turn certain leds on
// or off with a single integer
typedef uint64_t LedMap;

class VortexEngine;
class LedStash;

class Leds
{
public:
  Leds(VortexEngine &engine);
  ~Leds();

  // opting for class here because there should only ever be one
  // Led control object and I don't like singletons
  bool init();
  void cleanup();

  // control individual LED, these are appropriate to use in internal pattern logic
  void setIndex(LedPos target, RGBColor col);
  void setRange(LedPos first, LedPos last, RGBColor col);
  void setAll(RGBColor col);

  // Turn off individual LEDs, these are appropriate to use in internal pattern logic
  void clearIndex(LedPos target) { setIndex(target, HSV_OFF); }
  void clearRange(LedPos first, LedPos last) { setRange(first, last, HSV_OFF); }
  void clearAll() { setAll(HSV_OFF); }

  // control two LEDs on a pair, these are appropriate for use in internal pattern logic
  void setPair(Pair pair, RGBColor col);
  void setPairs(Pair first, Pair last, RGBColor col);

  // Turn off both LEDs on a pair, these are appropriate for use in internal pattern logic
  void clearPair(Pair pair) { setPair(pair, HSV_OFF); }
  void clearPairs(Pair first, Pair last) { setPairs(first, last, HSV_OFF); }

  // Controll pair evens
  void setRangeEvens(Pair first, Pair last, RGBColor);
  void setAllEvens(RGBColor col);
  // Controll pair odds
  void setRangeOdds(Pair first, Pair last, RGBColor);
  void setAllOdds(RGBColor col);

  // Turn off tips
  void clearRangeEvens(Pair first, Pair last);
  void clearAllEvens();
  // Turn off tops
  void clearRangeOdds(Pair first, Pair last);
  void clearAllOdds();

  // Turn on/off a mapping of leds with a color
  void setMap(LedMap map, RGBColor col);
  void clearMap(LedMap map);

  // stores Led for later use
  void stashAll(LedStash &stash);

  // restores Leds from stash
  void restoreAll(const LedStash &stash);

  // Dim individual LEDs, these are appropriate to use in internal pattern logic
  void adjustBrightnessIndex(LedPos target, uint8_t fadeBy);
  void adjustBrightnessRange(LedPos first, LedPos last, uint8_t fadeBy);
  void adjustBrightnessAll(uint8_t fadeBy);

  // Blink an led to blank or a color
  //
  // These APIs work by checking if the current time is within the
  // 'on duration' then they apply the color to the target, otherwise they do
  // nothing. The goal of this behaviour is to allow them to be used in any
  // situation to 'blink' an led to either off or some color.
  //
  // However since these APIs modulate current time to check if within the 'on'
  // threshold that makes them unsuitable for internal pattern usage because it
  // is unpredictable whether they will blink on or off first
  void blinkIndexOffset(LedPos target, uint32_t time, uint16_t offMs = 250, uint16_t onMs = 500, RGBColor col = RGB_OFF);
  void blinkIndex(LedPos target, uint16_t offMs = 250, uint16_t onMs = 500, RGBColor col = RGB_OFF);
  void blinkRange(LedPos first, LedPos last, uint16_t offMs = 250, uint16_t onMs = 500, RGBColor col = RGB_OFF);
  void blinkMap(LedMap targets, uint16_t offMs = 250, uint16_t onMs = 500, RGBColor col = RGB_OFF);
  void blinkAll(uint16_t offMs = 250, uint16_t onMs = 500, RGBColor col = RGB_OFF);
  // Blink both LEDs on a pair
  void blinkPair(Pair pair, uint16_t offMs = 250, uint16_t onMs = 500, RGBColor col = RGB_OFF);
  void blinkPairs(Pair first, Pair last, uint16_t offMs = 250, uint16_t onMs = 500, RGBColor col = RGB_OFF);

  // breath the hue on an index
  // warning: these use hsv to rgb in realtime!
  void breathIndex(LedPos target, uint8_t hue, uint32_t variance,
    uint32_t magnitude = 15, uint8_t sat = 255, uint8_t val = 210);
  void breathIndexSat(LedPos target, uint8_t hue, uint32_t variance,
    uint32_t magnitude = 15, uint8_t sat = 255, uint8_t val = 210);
  void breathIndexVal(LedPos target, uint8_t hue, uint32_t variance,
    uint32_t magnitude = 15, uint8_t sat = 255, uint8_t val = 210);

  // a very specialized api to hold all leds on a color for 250ms
  void holdAll(RGBColor col);

  // get the RGBColor of an Led index
  RGBColor getLed(LedPos pos) { return led(pos); }

  // global brightness
  uint8_t getBrightness() { return m_brightness; }
  void setBrightness(uint8_t brightness) { m_brightness = brightness; }

  // actually update the LEDs and show the changes
  void update();

#if FIXED_LED_COUNT == 0
  void setLedCount(uint8_t leds);
  LedPos ledCount() { return (LedPos)m_ledCount; }
  LedPos ledLast() { return (LedPos)(m_ledCount - 1); }
  LedPos ledMulti() { return (LedPos)(m_ledCount + 1); }
  LedPos ledAllSingle() { return (LedPos)(m_ledCount + 2); }
  LedPos ledAny() { return (LedPos)(m_ledCount + 3); }
  RGBColor *ledData() { return m_ledColors.data(); }
#else
  RGBColor *ledData() { return m_ledColors; }
#endif

private:
  // accessor for led colors, use this for all access to allow for mapping
  inline RGBColor &led(LedPos pos)
  {
    if (pos > LED_LAST) {
      pos = LED_LAST;
    }
    return m_ledColors[pos];
  }

  // reference to engine
  VortexEngine &m_engine;

  // the global brightness
  uint8_t m_brightness;

#if FIXED_LED_COUNT == 0
  // dynamic led count
  uint8_t m_ledCount;
  // array of led color values
  std::vector<RGBColor> m_ledColors;
#else
  RGBColor m_ledColors[LED_COUNT];
#endif
};

#endif
