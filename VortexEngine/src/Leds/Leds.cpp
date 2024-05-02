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
#include <Arduino.h>
#include <SPI.h>
#define LED_DATA_PIN  4
#define ONBOARD_LED_SCK 8
#define ONBOARD_LED_MOSI 7
static void transfer(uint8_t byte)
{
  uint8_t startbit = 0x80;
  bool lastmosi = !(byte & startbit);
  for (uint8_t b = startbit; b != 0; b = b >> 1) {
    delayMicroseconds(4);
    bool towrite = byte & b;
    if (lastmosi != towrite) {
      digitalWrite(ONBOARD_LED_MOSI, towrite);
      lastmosi = towrite;
    }
    digitalWrite(ONBOARD_LED_SCK, HIGH);
    delayMicroseconds(4);
    digitalWrite(ONBOARD_LED_SCK, LOW);
  }
}
static void turnOffOnboardLED()
{
  // spi device begin
  pinMode(ONBOARD_LED_SCK, OUTPUT);
  digitalWrite(ONBOARD_LED_SCK, LOW);
  pinMode(ONBOARD_LED_MOSI, OUTPUT);
  digitalWrite(ONBOARD_LED_MOSI, HIGH);
  SPI.begin();
  // Begin transaction, setting SPI frequency
  static const SPISettings mySPISettings(8000000, MSBFIRST, SPI_MODE0);
  SPI.beginTransaction(mySPISettings);
  for (uint8_t i = 0; i < 4; i++) {
    transfer(0x00); // begin frame
  }
  transfer(0xFF); //  Pixel start
  for (uint8_t i = 0; i < 3; i++) {
    transfer(0x00); // R,G,B
  }
  transfer(0xFF); // end frame
  SPI.endTransaction();

  SPI.end();
}
#endif

// global brightness
uint8_t Leds::m_brightness = DEFAULT_BRIGHTNESS;
// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };


void setupTimer() {
  // Configure TC3 for a suitable frequency (e.g., 400 Hz for LED update)
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3;
  while (GCLK->STATUS.bit.SYNCBUSY);

  TC3->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16;  // Set the counter to 16-bit mode
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY);

  TC3->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;  // Set the wave generation to match frequency
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY);

  TC3->COUNT16.CC[0].reg = 46875;  // Set the compare match value for a 400 Hz frequency
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY);

  TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;  // Enable TC3
  while (TC3->COUNT16.STATUS.bit.SYNCBUSY);

  TC3->COUNT16.INTENSET.bit.MC0 = 1;  // Enable the compare match interrupt
  NVIC_EnableIRQ(TC3_IRQn);  // Enable TC3 interrupts in the Nested Vector Interrupt Controller
}

void TC3_Handler() {
  // Interrupt service routine for TC3
  if (TC3->COUNT16.INTFLAG.bit.MC0) {
    Leds::update();  // Call your function to update LEDs
    TC3->COUNT16.INTFLAG.bit.MC0 = 1;  // Clear the interrupt flag
  }
}



bool Leds::init()
{
#ifdef VORTEX_EMBEDDED
  turnOffOnboardLED();
  pinMode(LED_DATA_PIN, OUTPUT);
  setupTimer();
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
  // start from tip and go to top
  setRange(pairEven(pair), pairOdd(pair), col);
}

void Leds::setPairs(Pair first, Pair last, RGBColor col)
{
  // start from tip and go to top
  setRange(pairEven(first), pairOdd(last), col);
}

void Leds::setFinger(Finger finger, RGBColor col)
{
  setRange(fingerTip(finger), fingerTop(finger), col);
}

void Leds::setRangeEvens(Pair first, Pair last, RGBColor col)
{
  for (Pair pos = first; pos <= last; pos++) {
    setIndex(pairEven(pos), col);
  }
}

void Leds::setAllEvens(RGBColor col)
{
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    setIndex(pairEven(pos), col);
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
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    setIndex(pairOdd(pos), col);
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
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    clearIndex(pairEven(pos));
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
  for (Pair pos = PAIR_FIRST; pos <= PAIR_LAST; pos++) {
    clearIndex(pairOdd(pos));
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

void Leds::blinkFingerOffset(Finger target, uint32_t time, uint16_t offMs, uint16_t onMs, RGBColor col)
{
  blinkRangeOffset(fingerTip(target), fingerTop(target), time, offMs, onMs, col);
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

void Leds::breatheFinger(Finger finger, uint8_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  breatheRange(fingerTip(finger), fingerTop(finger), hue, variance, magnitude, sat, val);
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

#ifdef VORTEX_EMBEDDED

// idk how correct this is but it works
#define CPU_FREQUENCY_MHZ 48
#define NS_TO_CYCLES(ns) (((ns) * CPU_FREQUENCY_MHZ) / 1000)

// these timings were found through trial and error
#define T0H 1
#define T0L 50
#define T1H 50
#define T1L 1

inline void delay_loop(uint32_t loop)
{
  while (loop--) {
    __asm__ __volatile__("nop");
  }
}

inline void sendBit(bool bitVal)
{
  uint32_t pinMask = (1ul << g_APinDescription[LED_DATA_PIN].ulPin);
  volatile uint32_t *outSet = &(PORT->Group[g_APinDescription[LED_DATA_PIN].ulPort].OUTSET.reg);
  volatile uint32_t *outClr = &(PORT->Group[g_APinDescription[LED_DATA_PIN].ulPort].OUTCLR.reg);
  if (bitVal) { // Send 1 bit
    *outSet = pinMask; // Set the output bit
    delay_loop(NS_TO_CYCLES(T1H)); // Delay for T1H
    *outClr = pinMask; // Clear the output bit
    delay_loop(NS_TO_CYCLES(T0L)); // Delay for T0L (since T1L can lead to early latch)
  } else { // Send 0 bit
    *outSet = pinMask; // Set the output bit
    delay_loop(NS_TO_CYCLES(T0H)); // Delay for T0H
    *outClr = pinMask; // Clear the output bit
    delay_loop(NS_TO_CYCLES(T1L)); // Delay for T1L
  }
}

inline void sendByte(unsigned char byte)
{
  for (unsigned char bit = 0; bit < 8; bit++) {
    sendBit(bitRead(byte, 7));                // Neopixel wants bit in highest-to-lowest order
    // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
    byte <<= 1;                                    // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
  }
}
#endif

void Leds::update()
{
#ifdef VORTEX_EMBEDDED
  // Important: need to disable interrupts during the transmission
  noInterrupts();
  for (LedPos pos = LED_FIRST; pos < LED_COUNT; pos++) {
    const RGBColor &col = m_ledColors[pos];
    sendByte((col.green * m_brightness) >> 8);
    sendByte((col.red * m_brightness) >> 8);
    sendByte((col.blue * m_brightness) >> 8);
  }
  // Re-enable interrupts
  interrupts();
  // Required to latch the LEDs
  //delayMicroseconds(10);
#endif
#ifdef VORTEX_LIB
  Vortex::vcallbacks()->ledsShow();
#endif
}
