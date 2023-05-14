#include "TimeControl.h"

#include <math.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif

#include "../Memory/Memory.h"
#include "../Log/Log.h"

#include "Timings.h"

#include "../Leds/Leds.h"

#ifdef VORTEX_ARDUINO
#include <avr/sleep.h>
#include <avr/interrupt.h>
// This is how long it takes for the attiny to go to sleep and wakeup when
// using the idle sleep state, so if trying to put the cpu to sleep for
// some amount of time it must be at least this amount or more.
//
// The real value is something like 712 but we overestimate just in case.
// This isn't in VortexConfig because it's not configurable, it's a hardware
// constant that can't be avoided
#define ATTINY_IDLE_SLEEP_MINIMUM 800
#endif

// static members
uint64_t Time::m_curTick = 0;
uint64_t Time::m_prevTime = 0;
uint64_t Time::m_firstTime = 0;
#if VARIABLE_TICKRATE == 1
uint32_t Time::m_tickrate = DEFAULT_TICKRATE;
#endif
#ifdef VORTEX_LIB
uint32_t Time::m_simulationTick = 0;
bool Time::m_isSimulation = false;
bool Time::m_instantTimestep = false;
#endif

// Within this file TICKRATE may refer to the variable member
// or the default tickrate constant based on the configuration
#if VARIABLE_TICKRATE == 1
#define TICKRATE m_tickrate
#else
#define TICKRATE DEFAULT_TICKRATE
#endif

#ifdef VORTEX_ARDUINO

#define TIMERD0_PRESCALER 

#define millisClockCyclesPerMicrosecond() ((uint16_t) (20))  // this always runs off the 20MHz oscillator
#define millisClockCyclesToMicroseconds(a) ((uint32_t)((a) / millisClockCyclesPerMicrosecond()))
#define microsecondsToMillisClockCycles(a) ((uint32_t)((a) * millisClockCyclesPerMicrosecond()))

#define TIME_TRACKING_TIMER_PERIOD    (0x1FD)
#define TIME_TRACKING_TIMER_DIVIDER   (64)    /* Clock divider for TCD0 */

#define FRACT_MAX (1000)
#define TIME_TRACKING_TICKS_PER_OVF (TIME_TRACKING_TIMER_PERIOD   + 1UL)
#define TIME_TRACKING_CYCLES_PER_OVF (TIME_TRACKING_TICKS_PER_OVF  * TIME_TRACKING_TIMER_DIVIDER)
#define FRACT_INC (millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)%1000)
#define MILLIS_INC (millisClockCyclesToMicroseconds(TIME_TRACKING_CYCLES_PER_OVF)/1000)

struct sTimeMillis {
    volatile uint16_t timer_fract;
    volatile uint32_t timer_millis;
    volatile uint32_t timer_overflow_count;
} timingStruct;

unsigned long micros()
{
  uint32_t overflows, microseconds;
  uint16_t ticks;
  uint8_t flags;
  /* Save current state and disable interrupts */
  uint8_t oldSREG = SREG;
  cli(); /* INTERRUPTS OFF */
  TCD0.CTRLE = TCD_SCAPTUREA_bm;
  while (!(TCD0.STATUS & TCD_CMDRDY_bm)); // wait for sync - should be only one iteration of this loop
  flags = TCD0.INTFLAGS;
  ticks = TCD0.CAPTUREA;
  /* If the timer overflow flag is raised, and the ticks we read are low, then the timer has rolled over but
   * ISR has not fired. If we already read a high value of ticks, either we read it just before the overflow,
   * so we shouldn't increment overflows, or interrupts are disabled and micros isn't expected to work so it
   * doesn't matter.
   * Get current number of overflows and timer count */
  overflows = timingStruct.timer_overflow_count;
  /* Turn interrupts back on, assuming they were on when micros was called. */
  SREG = oldSREG; /* INTERRUPTS ON */
  if ((flags & TCD_OVF_bm) && (ticks < 0x07)) {
    overflows++;
  } // end getting ticks

#if (F_CPU == 20000000UL || F_CPU == 10000000UL || F_CPU == 5000000UL)
  uint8_t ticks_l = ticks >> 1;
  ticks = ticks + ticks_l + ((ticks_l >> 2) - (ticks_l >> 4) + (ticks_l >> 7));
  // + ticks +(ticks>>1)+(ticks>>3)-(ticks>>5)+(ticks>>8))
  // speed optimization via doing math with smaller datatypes, since we know high byte is 1 or 0.
  microseconds =   overflows * (TIME_TRACKING_CYCLES_PER_OVF / 20) + ticks; // ticks value corrected above.
#else
  microseconds = ((overflows * (TIME_TRACKING_CYCLES_PER_OVF / 16)) + (ticks * (TIME_TRACKING_CYCLES_PER_OVF / 16 / TIME_TRACKING_TIMER_PERIOD)));
#endif
  return microseconds;
}

struct sTimer {
  uint8_t            intClear;
  volatile uint8_t  *intStatusReg;
};

const struct sTimer _timerS = {
  TCD_OVF_bm,
  &TCD0.INTFLAGS
};

// Now for the ISRs. This gets a little bit more interesting now...
ISR(TCD0_OVF_vect, ISR_NAKED)
{
  __asm__ __volatile__(
  "push       r30"          "\n\t" // First we make room for the pointer to timingStruct by pushing the Z registers
  "push       r31"          "\n\t" //
  ::);
  __asm__ __volatile__(
  // ISR prologue (overall 9 words / 9 clocks):
  "push       r24"            "\n\t" // we use three more registers other than the pointer
  "in         r24,     0x3F"  "\n\t" // Need to save SREG too
  "push       r24"            "\n\t" // and push the SREG value
  "push       r25"            "\n\t" // second byte
  "push       r23"            "\n\t" // third byte
  // timer_fract handling (13 words / 15/16 clocks):
  "ldi        r23, %[MIINC]"  "\n\t" // load MILLIS_INC. (part of timer_millis handling)
  "ld         r24,        Z"  "\n\t" // lo8(timingStruct.timer_fract).
  "ldd        r25,      Z+1"  "\n\t" // hi8(timingStruct.timer_fract)
  "subi       r24,%[LFRINC]"  "\n\t" // use (0xFFFF - FRACT_INC) and use the lower and higher byte to add by subtraction
  "sbci       r25,%[HFRINC]"  "\n\t" // can't use adiw since FRACT_INC might be >63
  "st         Z,        r24"  "\n\t" // lo8(timingStruct.timer_fract)
  "std        Z+1,      r25"  "\n\t" // hi8(timingStruct.timer_fract)
  "subi       r24,%[LFRMAX]"  "\n\t" // subtract FRACT_MAX and see if it is lower
  "sbci       r25,%[HFRMAX]"  "\n\t" //
  "brlo               lower"  "\n\t" // skip next three instructions if it was lower
  "st         Z,        r24"  "\n\t" // Overwrite the just stored value with the decremented value
  "std        Z+1,      r25"  "\n\t" // seems counter-intuitive, but it requires less registers
  "subi       r23,     0xFF"  "\n\t" // increment the MILLIS_INC by one, if FRACT_MAX was reached
  "lower:"                    "\n\t" // land here if fract was lower then FRACT_MAX
  // timer_millis handling (13 words / 17 clocks):
  "ldd        r25,      Z+2"  "\n\t" // lo16.lo8(timingStruct.timer_millis)
  "add        r25,      r23"  "\n\t" // add r23 to r25. r23 depends on MILLIS_INC and if FRACT_MAX was reached
  "std        Z+2,      r25"  "\n\t" //
  "ldi        r24,     0x00"  "\n\t" // get a 0x00 to adc with. Problem: can't subi 0x00 without losing the carry
  "ldd        r25,      Z+3"  "\n\t" // lo16.hi8(timingStruct.timer_millis)
  "adc        r25,      r24"  "\n\t" //
  "std        Z+3,      r25"  "\n\t" //
  "ldd        r25,      Z+4"  "\n\t" // hi16.lo8(timingStruct.timer_millis)
  "adc        r25,      r24"  "\n\t" //
  "std        Z+4,      r25"  "\n\t" //
  "ldd        r25,      Z+5"  "\n\t" // hi16.hi8(timingStruct.timer_millis)
  "adc        r25,      r24"  "\n\t" //
  "std        Z+5,      r25"  "\n\t" //
  // timer_overflow_count handling (12 words / 16 clocks):
  "ldd        r25,      Z+6"  "\n\t" // lo16.lo8(timingStruct.timer_overflow_count)
  "subi       r25,     0xFF"  "\n\t" //
  "std        Z+6,      r25"  "\n\t" //
  "ldd        r25,      Z+7"  "\n\t" // lo16.hi8(timingStruct.timer_overflow_count)
  "sbci       r25,     0xFF"  "\n\t" //
  "std        Z+7,      r25"  "\n\t" //
  "ldd        r25,      Z+8"  "\n\t" // hi16.lo8(timingStruct.timer_overflow_count)
  "sbci       r25,     0xFF"  "\n\t" //
  "std        Z+8,      r25"  "\n\t" //
  "ldd        r25,      Z+9"  "\n\t" // hi16.hi8(timingStruct.timer_overflow_count)
  "sbci       r25,     0xFF"  "\n\t" //
  "std        Z+9,      r25"  "\n\t" //
  // timer interrupt flag reset handling (3 words / 3 clocks):
  "ldi        r24, %[CLRFL]"  "\n\t" // This is the TCx interrupt clear bitmap
  "sts   %[PTCLR],      r24"  "\n\t" // write to Timer interrupt status register to clear flag. 2 clocks for sts
  // ISR epilogue (8 words / 17/18 clocks):
  "pop        r23"            "\n\t"
  "pop        r25"            "\n\t"
  "pop        r24"            "\n\t" // pop r24 to get the old SREG value - 2 clock
  "out       0x3F,      r24"  "\n\t" // restore SREG - 1 clock
  "pop        r24"            "\n\t"
  "pop        r31"            "\n\t"
  "pop        r30"            "\n\t"
  "reti"                      "\n\t" // total 77 - 79 clocks total, and 58 words, vs 104-112 clocks and 84 words
  :: "z" (&timingStruct),            // we are changing the value of this, so to be strictly correct, this must be declared input output - though in this case it doesn't matter
                                     // Spence: Woah, uhh... No you aren't changing that. You're changing values in the struct it points to. That's very different. For that to be safely changed
                                     // in ASM it needs to be volatile (which it already is, since this is also in interrupt context and it has to be)
    [LFRINC] "M" (((0x0000 - FRACT_INC)    & 0xFF)),
    [HFRINC] "M" (((0x0000 - FRACT_INC)>>8 & 0xFF)),
    [LFRMAX] "M" ((FRACT_MAX    & 0xFF)),
    [HFRMAX] "M" ((FRACT_MAX>>8 & 0xFF)),
    [MIINC]  "M" (MILLIS_INC),
    [CLRFL]  "M" (_timerS.intClear),
    [PTCLR]  "m" (*_timerS.intStatusReg)
  );
}

__attribute__ ((noinline)) void delayMicroseconds(uint32_t us)
{
 /* Must be noinline because we rely on function-call overhead */
#if F_CPU == 48000000L
  // make the initial delay 24 cycles
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 8 cycles with 3 words
  // the loop takes 1/4 of a microsecond (12 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we only burned ~22 cycles above, subtraction takes another 2 - so we've lost
  // half a us and only need to drop 2 rounds through the loop!
  us -= 2; // = 2 cycles,

#elif F_CPU >= 44000000L
  // Again, we can do all this in half of 1 us, so we
  // just pass through the loop 2 times for 1 us delay.
  __asm__ __volatile__(
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 cycles
                          // Wait 5 cycles in 3 words.
  // the loop takes 1/4 of a microsecond (11 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 19 (21) cycles above, remove 2
  // us is at least 8 so we can subtract 2
  us -= 2;

#elif F_CPU >= 40000000L
  // Again, we can do all this in half of 1 us, so we
  // just pass through the loop 2 times for 1 us delay.
  __asm__ __volatile__(
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 cycles
                          // Wait 3 cycles in 2 words.
  // the loop takes 1/4 of a microsecond (10 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 17 (19) cycles above, remove 2.
  // us is at least 8 so we can subtract 2
  us -= 2;

#elif F_CPU >= 36000000L
  // Here we get the initial delay is about 24 cycles, so we pass through
  // the loop once for 1us delay.
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 10 cycles in 4 words
  // the loop takes 1/3 of a microsecond (12 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 23 (25) cycles above, remove 2
  us -= 2; // 2 cycles

#elif F_CPU >= 32000000L
  // here, we only take half a us at the start
  __asm__ __volatile__ ("rjmp .+0");
                          // wait 2 cycles
  // in by the end of this section.
  // the loop takes 1/4 of a microsecond (8 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we only burned ~14 cycles above, subtraction takes another 2 - so we've lost half a us,
  // and only need to drop 2 rounds through the loop!
  us -= 2; // = 2 cycles

#elif F_CPU >= 30000000L
  // for a one-microsecond delay, burn 14 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over the return.
    "ret" "\n\t"          // 4 cycles - rjmped over initially...
    "rcall .-4" "\n\t"    // 2 cycles - ... but then called here...
    "rcall .-6");         // 2+4 cycles - ... and here again!
                          // Waiting 14 cycles in only 4 words
  if (us <= 1) return; // = 3 cycles, (4 when true)
  // the loop takes 1/3 of a microsecond (10 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 28 (30) cycles above, remove 3
  us -= 3; // 2 cycles

#elif F_CPU >= 28000000L
  // for a one-microsecond delay, burn 12 cycles and return
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+0" "\n\t"     // 2 cycles
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 12 cycles in 5 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/4 of a microsecond (7 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles=
  // we just burned 27 (29) cycles above, remove 4, (7*4=28)
  // us is at least 8 so we can subtract 5
  us -= 4; // = 2 cycles,

#elif F_CPU >= 27000000L
  // for a one-microsecond delay, burn 11 cycles and return
  __asm__ __volatile__ (  // wait 8 cycles with 3 words
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4" "\n\t"    // 2 cycles - ... but then called here);
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop");               // 1 more  == 11 total
  if (us <= 1) return;    //  = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (8 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 27 (24) cycles above, remove 3
  us -= 3; // 2 cycles


#elif F_CPU >= 24000000L
  // for a one-microsecond delay, burn 8 cycles and return
  __asm__ __volatile__ (
    "rjmp .+2" "\n\t"     // 2 cycles - jump over next instruction.
    "ret" "\n\t"          // 4 cycles - rjmped over initially....
    "rcall .-4");         // 2 cycles - ... but then called here);
                          // wait 8 cycles with 3 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)

  // the loop takes 1/3 of a microsecond (8 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 24 (22) cycles above, remove 3
  us -= 3; // 2 cycles

#elif F_CPU >= 20000000L
  // for a one-microsecond delay, burn 4 clocks and then return
  __asm__ __volatile__ (
    "rjmp .+0" "\n\t"     // 2 cycles
    "nop" );              // 1 cycle
                          // wait 3 cycles with 2 words
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes a 1/2 of a microsecond (10 cycles) per iteration
  // so execute it twice for each microsecond of delay requested.
  us = us << 1; // x2 us, = 2 cycles
  // we just burned 21 (23) cycles above, remove 2
  // us is at least 4 so we can subtract 2.
  us -= 2; // 2 cycles

#elif F_CPU >= 16000000L
  // for a one-microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1us
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/4 of a microsecond (4 cycles) per iteration
  // so execute it four times for each microsecond of delay requested.
  us <<= 2; // x4 us, = 4 cycles
  // we just burned 19 (21) cycles above, remove 5, (5*4=20)
  // us is at least 8 so we can subtract 5
  us -= 5; // = 2 cycles

#elif F_CPU >= 12000000L
  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 1) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/3 of a microsecond (4 cycles) per iteration
  // so execute it three times for each microsecond of delay requested.
  us = (us << 1) + us; // x3 us, = 5 cycles
  // we just burned 20 (22) cycles above, remove 5, (5*4=20)
  // us is at least 6 so we can subtract 5
  us -= 5; // 2 cycles

#elif F_CPU >= 10000000L
  // for a 1 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 1.5us
  if (us <= 2) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/2 of a microsecond (5 cycles) per iteration
  // so execute it 2 times for each microsecond of delay requested.
  us = us << 1; // x2 us, = 2 cycles
  // we just burned 20 (22) cycles above, remove 4, (5*4=20)
  // us is at least 6 so we can subtract 4
  us -= 4; // 2 cycles

#elif F_CPU >= 8000000L
  // for a 1 and 2 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 2us
  if (us <= 2) return; //  = 3 cycles, (4 when true)
  // the loop takes 1/2 of a microsecond (4 cycles) per iteration
  // so execute it twice for each microsecond of delay requested.
  us <<= 1; // x2 us, = 2 cycles
  // we just burned 17 (19) cycles above, remove 5, (4*5=20)
  // us is at least 6 so we can subtract 4
  us -= 5; // = 2 cycles

#elif F_CPU >= 5000000L
  // for a 1 ~ 3 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 3us
  if (us <= 3) return; // 3 cycles, (4 when true)
  // the loop takes 1 microsecond (5 cycles) per iteration
  // so just remove 3 loops for overhead
  us -= 3; // = 2 cycles

#elif F_CPU >= 4000000L
  // for a 1 ~ 4 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 4us
  if (us <= 4) return; // 3 cycles, (4 when true)
  // the loop takes 1 microsecond (4 cycles) per iteration,
  // just remove 4 loops for overhead
  us -= 4; // = 2 cycles for the time taken up with call overhead and test above

#elif F_CPU >= 2000000L
  // for a 1 ~ 4 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 8us
  if (us <= 8) return; // 3 cycles, (4 when true)
  // the loop takes 2 microsecond (4 cycles) per iteration,
  // just remove 4 loops for overhead
  us >>= 1; // divide by 2.
  us -= 4; // = 2 cycles for the time taken up with call overhead and test above

#else // F_CPU >= 1000000
  // for the 1 MHz internal clock (default settings for common AVR microcontrollers)
  // the overhead of the function calls is 14 (16) cycles
  if (us <= 16) return; // 3 cycles, (4 when true)
  if (us <= 25) return; // 3 cycles, (4 when true), (must be at least 26 if we want to subtract 22 and rightshift twice.)
  // compensate for the time taken by the preceding and following commands (about 22 cycles)
  us -= 22; // = 2 cycles
  // the loop takes 4 microseconds (4 cycles)
  // per iteration, so execute it us/4 times
  // us is at least 4, divided by 4 gives us 1 (no zero delay bug)
  us >>= 2; // us div 4, = 4 cycles
#endif
/* Implementation of the delay loop of 4, 5, 7, 8, 10, 11, or 12 clocks. */
#if defined(DELAYMICROS_TWELVE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+2"      "\n\t"            // 2 cycles - jump over next instruction.
    "ret"           "\n\t"            // 4 cycles - rjmped over initially....
    "rcall .-4"     "\n\t"            // 2 cycles - ... but then called here
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_ELEVEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_TEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_NINE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "nop"           "\n\t"
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_EIGHT)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_SEVEN)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "rjmp .+0"      "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#elif defined(DELAYMICROS_FIVE)
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "nop"           "\n\t"            // 1 cycle
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#else // the classic 4 cycle delay loop...
  __asm__ __volatile__ (
    "1: sbiw %0, 1" "\n\t"            // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us)  // 2 cycles
  );
#endif
  // return = 4 cycles
}
#endif // VORTEX_ARDUINO

bool Time::init()
{
#ifdef VORTEX_ARDUINO
  // enable interrupts
  sei();

  // initialize main clock
#if (F_CPU == 20000000)
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00); // No division on clock
#elif (F_CPU == 16000000)
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00); // No division on clock
#elif (F_CPU == 10000000) // 20MHz prescaled by 2
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc)); // Clock DIV2
#elif (F_CPU == 8000000) // 16MHz prescaled by 2
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc)); // Clock DIV2
#elif (F_CPU == 5000000) // 20MHz prescaled by 4
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc)); // Clock DIV4
#elif (F_CPU == 4000000) // 16MHz prescaled by 4
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc)); // Clock DIV4
#elif (F_CPU == 2000000) // 16MHz prescaled by 8
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_8X_gc)); // Clock DIV8
#elif (F_CPU == 1000000) // 16MHz prescaled by 16
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_16X_gc)); // Clock DIV16
#else
  #error "F_CPU not supported"
#endif

  // initialize TCD0
  TCD0.CMPBCLR = TIME_TRACKING_TIMER_PERIOD;
  TCD0.CTRLB = 0x00;
  TCD0.CTRLC = 0x80;
  TCD0.INTCTRL = 0x01;
  TCD0.CTRLA = TCD_CLKSEL_20MHZ_gc | TCD_CNTPRES_DIV32_gc | TCD_SYNCPRES_DIV2_gc | 0x01;

  _PROTECTED_WRITE(CPUINT_CTRLA, CPUINT_IVSEL_bm);
#endif
  m_firstTime = m_prevTime = micros();
  m_curTick = 0;
#if VARIABLE_TICKRATE == 1
  m_tickrate = DEFAULT_TICKRATE;
#endif
#ifdef VORTEX_LIB
  m_simulationTick = 0;
  m_isSimulation = false;
  m_instantTimestep = false;
#endif
  return true;
}

void Time::cleanup()
{
}

void Time::tickClock()
{
  // tick clock forward
  m_curTick++;

#ifdef VORTEX_LIB
  if (m_instantTimestep) {
    return;
  }
#endif

#if DEBUG_ALLOCATIONS == 1
  if ((m_curTick % msToTicks(1000)) == 0) {
    DEBUG_LOGF("Cur Memory: %u (%u)", cur_memory_usage(), cur_memory_usage_background());
  }
#endif

  // perform timestep
  uint32_t elapsed_us;
  uint32_t us;
  do {
    us = micros();
    // detect rollover of microsecond counter
    if (us < m_prevTime) {
      // calculate wrapped around difference
      elapsed_us = (uint32_t)((UINT32_MAX - m_prevTime) + us);
    } else {
      // otherwise calculate regular difference
      elapsed_us = (uint32_t)(us - m_prevTime);
    }
    // if building anywhere except visual studio then we can run alternate sleep code
    // because in visual studio + windows it's better to just spin and check the high
    // resolution clock instead of trying to sleep for microseconds.
#if !defined(_MSC_VER)
    uint32_t required = (1000000 / TICKRATE);
    uint32_t sleepTime = 0;
    if (required > elapsed_us) {
      // in vortex lib on linux we can just sleep instead of spinning
      // but on arduino we must spin and on windows it actually ends
      // up being more accurate to poll QPF + QPC via micros()
      sleepTime = required - elapsed_us;
    }
#if defined(VORTEX_LIB)
    delayMicroseconds(sleepTime);
    break;
#elif defined(VORTEX_ARDUINO)
    // on the attiny we can sleep for any amount more than the minimum
    // amount of cycles it takes to actually run the sleep code
    for (uint8_t i = 0; i < sleepTime / ATTINY_IDLE_SLEEP_MINIMUM; ++i) {
      sleep_cpu();
    }
#endif
#endif
    // 1000us per ms, divided by tickrate gives
    // the number of microseconds per tick
  } while (elapsed_us < (1000000 / TICKRATE));

  // store current time
  m_prevTime = micros();
}

// get the current time with optional led position time offset
uint64_t Time::getCurtime(LedPos pos)
{
  // the current tick, plus the time offset per LED, plus any
  // simulation offset
#ifdef VORTEX_LIB
  return m_curTick + getSimulationTick();
#else
  return m_curTick;
#endif
}

// the real current time, bypass simulations, used by timers
uint64_t Time::getRealCurtime()
{
  return m_curTick;
}

uint32_t Time::getTickrate()
{
  return TICKRATE;
}

// Set tickrate in Ticks Per Second (TPS)
// The valid range for this is 1 <= x <= 1000000
void Time::setTickrate(uint32_t tickrate)
{
#if VARIABLE_TICKRATE == 1
  if (!tickrate) {
    // can't set 0 tickrate, so 0 sets default
    tickrate = DEFAULT_TICKRATE;
  } else if (tickrate > 1000000) {
    // more than 1 million ticks per second won't work anyway
    tickrate = 1000000;
  }
  // update the tickrate
  m_tickrate = tickrate;
#endif
}

uint32_t Time::msToTicks(uint32_t ms)
{
  // 0ms = 0 ticks
  if (!ms) {
    return 0;
  }
  // but anything > 0 ms must be no less than 1 tick
  // otherwise short durations will disappear at low
  // tickrates
  uint32_t ticks = (ms * TICKRATE) / 1000;
  if (!ticks) {
    return 1;
  }
  return ticks;
}

#ifdef VORTEX_LIB

// Start a time simulation, while the simulation is active you can
// increment the 'current time' with tickSimulation() then when you
// call endSimulation() the currentTime will be restored
uint32_t Time::startSimulation()
{
  m_simulationTick = 0;
  m_isSimulation = true;
  return (uint32_t)getCurtime();
}

// Tick a time simulation forward, returning the next tick
uint32_t Time::tickSimulation()
{
  return ++m_simulationTick;
}

// whether running time simulation
bool Time::isSimulation()
{
  return m_isSimulation;
}

// get the current tick in the simulation
uint32_t Time::getSimulationTick()
{
  return m_simulationTick;
}

// Finish a time simulation
uint32_t Time::endSimulation()
{
  uint32_t endTick = (uint32_t)getCurtime();
  m_simulationTick = 0;
  m_isSimulation = false;
  return endTick;
}

#endif

#if TIMER_TEST == 1
#include <assert.h>

void Time::test()
{
  DEBUG_LOG("Starting Time class tests...");

  // just return immediately when testing the time system,
  // this prevents the actual delay from occurring and
  // allows us to tick forward as fast as we want
  setInstantTimestep(true);

  // Test init function
  assert(init());
  DEBUG_LOG("Init test passed");
  cleanup();

  // Test tickClock function
  uint64_t initialTick = m_curTick;
  tickClock();
  assert(m_curTick == initialTick + 1);
  DEBUG_LOG("tickClock test passed");

  // Test msToTicks function
  assert(msToTicks(1000) == TICKRATE);
  assert(msToTicks(500) == TICKRATE / 2);
  assert(msToTicks(0) == 0);
  DEBUG_LOG("msToTicks test passed");

  // Test getRealCurtime function
  assert(getRealCurtime() == m_curTick);
  DEBUG_LOG("getRealCurtime test passed");

  // Test getTickrate function
  assert(getTickrate() == TICKRATE);
  DEBUG_LOG("getTickrate test passed");

  // Test setTickrate function (only when VARIABLE_TICKRATE is enabled)
#if VARIABLE_TICKRATE == 1
  uint32_t newTickrate = TICKRATE * 2;
  setTickrate(newTickrate);
  assert(getTickrate() == newTickrate);
  DEBUG_LOG("setTickrate test passed");
#endif

#ifdef VORTEX_LIB
  // Test simulation functions
  uint32_t simulationStartTick = startSimulation();
  assert(isSimulation());
  assert(getSimulationTick() == 0);
  DEBUG_LOG("startSimulation test passed");

  uint32_t simulationTick = tickSimulation();
  assert(getSimulationTick() == 1);
  DEBUG_LOGF("tickSimulation test passed, simulationTick: %u", simulationTick);

  uint32_t simulationEndTick = endSimulation();
  assert(!isSimulation());
  assert(simulationEndTick == simulationStartTick + 1);
  DEBUG_LOGF("endSimulation test passed, simulationEndTick: %u", simulationEndTick);
#endif

  DEBUG_LOG("Time class tests completed successfully.");
}
#endif
