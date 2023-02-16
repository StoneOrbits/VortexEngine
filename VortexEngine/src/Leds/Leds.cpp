#include <math.h>

#include <Arduino.h>

#include "Leds.h"

#include "../Time/TimeControl.h"

#define LED_DATA_PIN  9

#define POWER_LED_PIN 7
#define POWER_LED_CLK 8

// array of led color values
RGBColor Leds::m_ledColors[LED_COUNT] = { RGB_OFF };
// the onboard LED on the adafruit board
//Adafruit_DotStar Leds::m_onboardLED(1, POWER_LED_PIN, POWER_LED_CLK, DOTSTAR_BGR);
// global brightness
uint32_t Leds::m_brightness = DEFAULT_BRIGHTNESS;

// Output PORT register
volatile uint8_t *Leds::m_port = nullptr;
// Output PORT bitmask
uint8_t Leds::m_pinMask = 0;

bool Leds::init()
{
  // clear the onboard led so it displays nothing
  // tiny neo pixels
  pinMode(LED_DATA_PIN, OUTPUT);
  // register ouput port
  m_port = portOutputRegister(digitalPinToPort(LED_DATA_PIN));
  // create a pin mask to use later
  m_pinMask = digitalPinToBitMask(LED_DATA_PIN);
  return true;
}

void Leds::cleanup()
{
  for (uint32_t i = 0; i < LED_COUNT; ++i) {
    m_ledColors[i].clear();
  }
}

void Leds::setIndex(LedPos target, RGBColor col)
{
  // safety
  if (target > LED_LAST) {
    target = LED_LAST;
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

void Leds::setFinger(Finger finger, RGBColor col)
{
  // start from tip and go to top
  setRange(fingerTip(finger), fingerTop(finger), col);
}

void Leds::setFingers(Finger first, Finger last, RGBColor col)
{
  // start from tip and go to top
  setRange(fingerTip(first), fingerTop(last), col);
}

void Leds::setRangeTips(Finger first, Finger last, RGBColor col)
{
  for (Finger pos = first; pos <= last; pos++) {
    setIndex(fingerTip(pos), col);
  }
}

void Leds::setAllTips(RGBColor col)
{
  for (Finger pos = FINGER_FIRST; pos <= FINGER_LAST; pos++) {
    setIndex(fingerTip(pos), col);
  }
}

void Leds::setRangeTops(Finger first, Finger last, RGBColor col)
{
  for (Finger pos = first; pos <= last; pos++) {
    setIndex(fingerTop(pos), col);
  }
}

void Leds::setAllTops(RGBColor col)
{
  for (Finger pos = FINGER_FIRST; pos <= FINGER_LAST; pos++) {
    setIndex(fingerTop(pos), col);
  }
}

void Leds::clearRangeTips(Finger first, Finger last) {
  for (Finger pos = first; pos <= last; pos++) {
    clearIndex(fingerTip(pos));
  }
}

void Leds::clearAllTips()
{
  for (Finger pos = FINGER_FIRST; pos <= FINGER_LAST; pos++) {
    clearIndex(fingerTip(pos));
  }
}

void Leds::clearRangeTops(Finger first, Finger last) {
  for (Finger pos = first; pos <= last; pos++) {
    clearIndex(fingerTop(pos));
  }
}

void Leds::clearAllTops()
{
  for (Finger pos = FINGER_FIRST; pos <= FINGER_LAST; pos++) {
    clearIndex(fingerTop(pos));
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

void Leds::blinkIndex(LedPos target, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setIndex(target, col);
  }
}

void Leds::blinkRange(LedPos first, LedPos last, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(first, last, col);
  }
}

void Leds::blinkAll(uint64_t time, int32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(LED_FIRST, LED_LAST, col);
  }
}

void Leds::blinkFinger(Finger finger, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(fingerTip(finger), fingerTop(finger), col);
  }
}

void Leds::blinkFingers(Finger first, Finger last, uint64_t time, uint32_t offMs, uint32_t onMs, RGBColor col)
{
  if ((time % Time::msToTicks(offMs + onMs)) < Time::msToTicks(onMs)) {
    setRange(fingerTip(first), fingerTop(last), col);
  }
}

void Leds::breathIndex(LedPos target, uint32_t hue, uint32_t variance, uint32_t magnitude, uint8_t sat, uint8_t val)
{
  setIndex(target, HSVColor((uint8_t)(hue + ((sin(variance * 0.0174533) + 1) * magnitude)), sat, val));
}

void Leds::update()
{
  // Thanks to TinyNeoPixel for this code
#ifdef VORTEX_ARDUINO
  noInterrupts();
  volatile uint16_t
    i = LED_COUNT * sizeof(RGBColor); // Loop counter
  volatile uint8_t
    *ptr = (volatile uint8_t *)m_ledColors,   // Pointer to next byte
    b = *ptr++,   // Current byte value
    hi,             // PORT w/output bit set high
    lo;             // PORT w/output bit set low

  // 25 inst. clocks per bit: HHHHHHHxxxxxxxxLLLLLLLLLL
  // ST instructions:         ^      ^       ^       (T=0,7,15)

  volatile uint8_t next, bit;

  hi = *m_port | m_pinMask;
  lo = *m_port & ~m_pinMask;
  next = lo;
  bit = 8;

  asm volatile(
    "head20:"                   "\n\t" // Clk  Pseudocode    (T =  0)
    "st   %a[port],  %[hi]"    "\n\t" // 1    PORT = hi     (T =  1)
    "sbrc %[byte],  7"         "\n\t" // 1-2  if (b & 128)
    "mov  %[next], %[hi]"      "\n\t" // 0-1   next = hi    (T =  3)
    "dec  %[bit]"              "\n\t" // 1    bit--         (T =  4)
    "nop"                      "\n\t" // 1    nop           (T =  5)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T =  7)
    "st   %a[port],  %[next]"  "\n\t" // 1    PORT = next   (T =  8)
    "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T =  9)
    "breq nextbyte20"          "\n\t" // 1-2  if (bit == 0) (from dec above)
    "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 11)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 13)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 15)
    "st   %a[port],  %[lo]"    "\n\t" // 1    PORT = lo     (T = 16)
    "nop"                      "\n\t" // 1    nop           (T = 17)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 19)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 21)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 23)
    "rjmp head20"              "\n\t" // 2    -> head20 (next bit out)
    "nextbyte20:"              "\n\t" //                    (T = 11)
    "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 12)
    "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 14)
    "nop"                      "\n\t" // 1    nop           (T = 15)
    "st   %a[port], %[lo]"     "\n\t" // 1    PORT = lo     (T = 16)
    "nop"                      "\n\t" // 1    nop           (T = 17)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 19)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 21)
    "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 23)
    "brne head20"              "\n"   // 2    if (i != 0) -> (next byte)  ()
    : [ptr] "+e" (ptr),
    [byte]  "+r" (b),
    [bit]   "+d" (bit),
    [next]  "+r" (next),
    [count] "+w" (i)
    : [port] "e" (m_port),
    [hi]     "r" (hi),
    [lo]     "r" (lo));

  interrupts();
#endif
}
