#include <math.h>

#include "LedStash.h"
#include "Leds.h"

#include "../Time/TimeControl.h"
#include "../Modes/Modes.h"

#include "../VortexConfig.h"

#ifdef VORTEX_LIB
#include "../../VortexLib/VortexLib.h"
#endif

#ifdef VORTEX_EMBEDDED
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#include "driver/rmt_tx.h"
#define LED_PIN     10
#endif

// global brightness
uint8_t Leds::m_brightness = DEFAULT_BRIGHTNESS;
// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };

#ifdef VORTEX_EMBEDDED
// The engine works in logical led order (see LedTypes.h) but the physical
// data chain is ordered differently, so the colors are re-ordered into this
// buffer right before they are sent to the leds
static RGBColor physicalColors[LED_COUNT] = { RGB_OFF };

// The leds are driven with the RMT peripheral directly instead of FastLED so
// the data can be clocked faster than the stock 800kHz (see LED_BIT_NS etc in
// VortexConfig.h) and so the frame is sent in the background while the engine
// works on the next tick. The RMT reads txBuffer while it sends, so it is only
// refilled after the previous frame is finished.
static uint8_t txBuffer[LED_COUNT * 3];
static rmt_channel_handle_t ledChannel = nullptr;
static rmt_encoder_handle_t bytesEncoder = nullptr;
static rmt_encoder_handle_t copyEncoder = nullptr;
// a single low symbol that holds the line low long enough to latch a frame
static rmt_symbol_word_t latchSymbol;
#define LED_RMT_RESOLUTION_HZ 40000000
#define NS_TO_RMT_TICKS(ns) ((uint16_t)(((ns) + 12) / 25))

// The current the leds would draw for the frame in milliwatts at full
// brightness, same model as FastLED's power management
static uint32_t estimatePowerMW()
{
  uint32_t red = 0, green = 0, blue = 0;
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    red += physicalColors[i].red;
    green += physicalColors[i].green;
    blue += physicalColors[i].blue;
  }
  return ((red * 80) + (green * 55) + (blue * 75)) >> 8;
}
#endif

bool Leds::init()
{
#ifdef VORTEX_EMBEDDED
  rmt_tx_channel_config_t chanConfig = {};
  chanConfig.gpio_num = (gpio_num_t)LED_PIN;
  chanConfig.clk_src = RMT_CLK_SRC_DEFAULT;
  chanConfig.resolution_hz = LED_RMT_RESOLUTION_HZ;
  // both of the C3's tx memory blocks, fewer refill interrupts per frame
  chanConfig.mem_block_symbols = 96;
  // the frame and the latch gap are queued together
  chanConfig.trans_queue_depth = 4;
  if (rmt_new_tx_channel(&chanConfig, &ledChannel) != ESP_OK) {
    return false;
  }
  rmt_bytes_encoder_config_t bytesConfig = {};
  bytesConfig.bit0.level0 = 1;
  bytesConfig.bit0.duration0 = NS_TO_RMT_TICKS(LED_T0H_NS);
  bytesConfig.bit0.level1 = 0;
  bytesConfig.bit0.duration1 = NS_TO_RMT_TICKS(LED_BIT_NS - LED_T0H_NS);
  bytesConfig.bit1.level0 = 1;
  bytesConfig.bit1.duration0 = NS_TO_RMT_TICKS(LED_T1H_NS);
  bytesConfig.bit1.level1 = 0;
  bytesConfig.bit1.duration1 = NS_TO_RMT_TICKS(LED_BIT_NS - LED_T1H_NS);
  bytesConfig.flags.msb_first = 1;
  rmt_copy_encoder_config_t copyConfig = {};
  if (rmt_new_bytes_encoder(&bytesConfig, &bytesEncoder) != ESP_OK ||
      rmt_new_copy_encoder(&copyConfig, &copyEncoder) != ESP_OK ||
      rmt_enable(ledChannel) != ESP_OK) {
    return false;
  }
  // the latch is split across both halves of the symbol (15 bit durations)
  latchSymbol.level0 = 0;
  latchSymbol.duration0 = (LED_RESET_US * (LED_RMT_RESOLUTION_HZ / 1000000)) / 2;
  latchSymbol.level1 = 0;
  latchSymbol.duration1 = (LED_RESET_US * (LED_RMT_RESOLUTION_HZ / 1000000)) / 2;
#endif
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsInit(m_ledColors, LED_COUNT);
#endif
  return true;
}

void Leds::cleanup()
{
  for (uint8_t i = 0; i < LED_COUNT; ++i) {
    m_ledColors[i].clear();
  }
}

void Leds::setIndex(LedPos target, RGBColor col)
{
  if (target >= LED_COUNT) {
    setAll(col);
    return;
  }
  led(target) = col;
}

void Leds::setRange(LedPos first, LedPos last, RGBColor col)
{
  for (LedPos pos = first; pos <= last; pos++) {
    setIndex(pos, col);
  }
}

void Leds::setAll(RGBColor col)
{
  setRange(LED_FIRST, LED_LAST, col);
}

void Leds::setPair(Pair pair, RGBColor col)
{
  if (pair >= PAIR_COUNT) {
    return;
  }
  // the top and bottom led at this position (the same led for a tip)
  setIndex(pairTop(pair), col);
  setIndex(pairBot(pair), col);
}

void Leds::setPairs(Pair first, Pair last, RGBColor col)
{
  for (Pair pair = first; pair <= last && pair < PAIR_COUNT; ++pair) {
    setPair(pair, col);
  }
}

void Leds::setQuadrant(Quadrant quadrant, RGBColor col)
{
  if (quadrant == QUADRANT_5) {
    // the 'fifth quadrant' is the four tips
    setRing(RING_LAST, col);
    return;
  }
  if (quadrant >= QUADRANT_5) {
    return;
  }
  setRange(quadrantFirstLed(quadrant), quadrantLastLed(quadrant), col);
}

void Leds::setQuadrants(Quadrant first, Quadrant last, RGBColor col)
{
  for (Quadrant quad = first; quad <= last && quad < QUADRANT_COUNT; quad = (Quadrant)(quad + 1)) {
    setQuadrant(quad, col);
  }
}

void Leds::setRing(Ring ring, RGBColor col)
{
  if (ring >= RING_COUNT) {
    return;
  }
  // the pair at this ring in each quadrant
  for (uint32_t quad = 0; quad < NUM_QUADRANTS; ++quad) {
    setPair((Pair)((quad * PAIRS_PER_QUADRANT) + ring), col);
  }
}

void Leds::setRings(Ring first, Ring last, RGBColor col)
{
  for (Ring ring = first; ring <= last && ring < RING_COUNT; ring = (Ring)(ring + 1)) {
    setRing(ring, col);
  }
}

void Leds::setRangeEvens(Pair first, Pair last, RGBColor col)
{
  for (Pair pos = first; pos <= last; pos++) {
    setIndex(pairEven(pos), col);
  }
}

void Leds::setAllEvens(RGBColor col)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (isEven(pos)) {
      setIndex(pos, col);
    }
  }
}

void Leds::setRangeOdds(Pair first, Pair last, RGBColor col)
{
  for (Pair pos = first; pos <= last; pos++) {
    setIndex(pairOdd(pos), col);
  }
}

void Leds::setAllOdds(RGBColor col)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (isOdd(pos)) {
      setIndex(pos, col);
    }
  }
}

void Leds::clearRangeEvens(Pair first, Pair last)
{
  for (Pair pos = first; pos <= last; pos++) {
    clearIndex(pairEven(pos));
  }
}

void Leds::clearAllEvens()
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (isEven(pos)) {
      clearIndex(pos);
    }
  }
}

void Leds::clearRangeOdds(Pair first, Pair last)
{
  for (Pair pos = first; pos <= last; pos++) {
    clearIndex(pairOdd(pos));
  }
}

void Leds::clearAllOdds()
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (isOdd(pos)) {
      clearIndex(pos);
    }
  }
}

void Leds::setMap(LedMap map, RGBColor col)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (ledmapCheckLed(map, pos)) {
      setIndex(pos, col);
    }
  }
}

void Leds::clearMap(LedMap map)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    if (ledmapCheckLed(map, pos)) {
      clearIndex(pos);
    }
  }
}

void Leds::stashAll(LedStash &stash)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    stash.m_ledColorsStash[pos] = led(pos);
  }
}

void Leds::restoreAll(const LedStash &stash)
{
  for (LedPos pos = LED_FIRST; pos <= LED_LAST; pos++) {
    led(pos) = stash.m_ledColorsStash[pos];
  }
}

void Leds::adjustBrightnessIndex(LedPos target, uint8_t fadeBy)
{
  led(target).adjustBrightness(fadeBy);
}

void Leds::adjustBrightnessRange(LedPos first, LedPos last, uint8_t fadeBy)
{
  for (LedPos pos = first; pos <= last; pos++) {
    adjustBrightnessIndex(pos, fadeBy);
  }
}

void Leds::adjustBrightnessAll(uint8_t fadeBy)
{
  adjustBrightnessRange(LED_FIRST, LED_LAST, fadeBy);
}

// blinkIndex with the additional optional time offset
void Leds::blinkIndexOffset(LedPos target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((time % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setIndex(target, col);
  }
}

void Leds::blinkRangeOffset(LedPos first, LedPos last, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((time % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(first, last, col);
  }
}

void Leds::blinkIndex(LedPos target, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setIndex(target, col);
  }
}

void Leds::blinkRange(LedPos first, LedPos last, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(first, last, col);
  }
}

void Leds::blinkMap(LedMap targets, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    for (LedPos pos = LED_FIRST; pos < LED_COUNT; pos++) {
      if (ledmapCheckLed(targets, pos)) {
        setIndex(pos, col);
      }
    }
  }
}

void Leds::blinkAll(uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(LED_FIRST, LED_LAST, col);
  }
}

void Leds::blinkPair(Pair pair, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(pairEven(pair), pairOdd(pair), col);
  }
}

void Leds::blinkPairs(Pair first, Pair last, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  if ((Time::getCurtime() % MS_TO_TICKS(offMs + onMs)) < MS_TO_TICKS(onMs)) {
    setRange(pairEven(first), pairOdd(last), col);
  }
}

void Leds::breatheIndex(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
}

void Leds::breatheRange(LedPos first, LedPos last, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setRange(first, last, HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
}

void Leds::breatheIndexSat(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor(hue, 255 - (uint8_t)(sat + 128 + ((sin(variance * 0.0174533) + 1) * magnitude)), val));
}

void Leds::breatheIndexVal(LedPos target, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor(hue, sat, 255 - (uint8_t)(val + 128 + ((sin(variance * 0.0174533) + 1) * magnitude))));
}

void Leds::holdAll(RGBColor col)
{
  setAll(col);
  update();
  Time::delayMilliseconds(250);
}

void Leds::update()
{
#ifdef VORTEX_EMBEDDED
  // re-order the logical leds into the physical chain order
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; ++pos) {
    RGBColor col = m_ledColors[pos];
    if (!isPairSide(pos)) {
      // the 64 small XL-1010 leds take green first (GRB) while the four large
      // tip leds (WS2812D-F5) on the same data line take red first (RGB)
      uint8_t red = col.red;
      col.red = col.green;
      col.green = red;
    }
    physicalColors[ledToPhysical(pos)] = col;
  }
  // cap the total current the leds can draw (see VortexConfig.h)
  uint32_t brightness = m_brightness;
  uint32_t maxPowerMW = (uint32_t)LED_POWER_LIMIT_VOLTS * LED_POWER_LIMIT_MA;
  uint32_t powerMW = ((estimatePowerMW() * brightness) >> 8) + (5 * LED_COUNT);
  if (powerMW > maxPowerMW) {
    brightness = (brightness * maxPowerMW) / powerMW;
  }
  // wait for the previous frame and its latch gap to finish sending, the
  // engine has been running the next tick meanwhile
  rmt_tx_wait_all_done(ledChannel, -1);
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    txBuffer[(i * 3) + 0] = (uint8_t)((physicalColors[i].red * (brightness + 1)) >> 8);
    txBuffer[(i * 3) + 1] = (uint8_t)((physicalColors[i].green * (brightness + 1)) >> 8);
    txBuffer[(i * 3) + 2] = (uint8_t)((physicalColors[i].blue * (brightness + 1)) >> 8);
  }
  rmt_transmit_config_t txConfig = {};
  rmt_transmit(ledChannel, bytesEncoder, txBuffer, sizeof(txBuffer), &txConfig);
  rmt_transmit(ledChannel, copyEncoder, &latchSymbol, sizeof(latchSymbol), &txConfig);
#endif
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsShow();
#endif
}
