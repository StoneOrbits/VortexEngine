#include "VortexEngine.h"

#include <Arduino.h>

void __attribute__((weak)) init_clock() {
  #ifndef CLOCK_SOURCE
    #error "CLOCK_SOURCE not defined. CLOCK_SOURCE must be either 0 (internal) or 2 (external clock)"
  #endif
  #if (CLOCK_SOURCE == 0)
    #if (defined(CLOCK_TUNE_INTERNAL))
      tune_internal(); // Will be inlined as only called once. Just too long and ugly to put two implementations in middle of this.
    #else
      #if (F_CPU == 20000000)
        /* No division on clock */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
      #elif (F_CPU == 16000000)
        /* No division on clock */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
      #elif (F_CPU == 10000000) // 20MHz prescaled by 2
        /* Clock DIV2 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));
      #elif (F_CPU == 8000000) // 16MHz prescaled by 2
        /* Clock DIV2 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_2X_gc));
      #elif (F_CPU == 5000000) // 20MHz prescaled by 4
        /* Clock DIV4 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc));
      #elif (F_CPU == 4000000) // 16MHz prescaled by 4
        /* Clock DIV4 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_4X_gc));
      #elif (F_CPU == 2000000) // 16MHz prescaled by 8
        /* Clock DIV8 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_8X_gc));
      #elif (F_CPU == 1000000) // 16MHz prescaled by 16
        /* Clock DIV16 */
        _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PEN_bm | CLKCTRL_PDIV_16X_gc));
      #else
        #ifndef F_CPU
          #error "F_CPU not defined"
        #else
          #error "F_CPU defined as an unsupported value for untuned internal oscillator"
        #endif
      #endif
    #endif
  #elif (CLOCK_SOURCE == 2)
    _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_EXTCLK_gc);
    // while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);  // This either works, or hangs the chip - EXTS is pretty much useless here.
    // w/out CFD, easier to determine what happened if we don't just hang here.
    uint8_t count = 10;
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm && count--);
    if (CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm) {
      _PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, 0x00);
    }
  #else
    #error "CLOCK_SOURCE is defined, but it isn't 0 (internal) or 2 (external clock), and those are the only clock sources supported by this part."
  #endif
}

void __attribute__((weak)) init_millis()
{
  #if defined(MILLIS_USE_TIMERNONE)
    badCall("init_millis() is only valid with millis time keeping enabled.");
  #else
    #if defined(MILLIS_USE_TIMERA0)
      #if !defined(TCA_BUFFERED_3PIN)
        TCA0.SPLIT.INTCTRL = TCA_SPLIT_HUNF_bm;
      #else
        TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
      #endif
    #elif defined(MILLIS_USE_TIMERA1)
      TCA1.SPLIT.INTCTRL |= TCA_SPLIT_HUNF_bm;
    #elif defined(MILLIS_USE_TIMERD0)
      TCD_t* pTCD;
      _fastPtr_d(pTCD, &TCD0);
      pTCD->CMPBCLR        = TIME_TRACKING_TIMER_PERIOD; // essentially, this is TOP
      pTCD->CTRLB          = 0x00; // oneramp mode
      pTCD->CTRLC          = 0x80;
      pTCD->INTCTRL        = 0x01; // enable interrupt
      pTCD->CTRLA          = TIMERD0_PRESCALER | 0x01; // set clock source and enable!
    #elif defined(MILLIS_USE_TIMERRTC)
      while(RTC.STATUS); // if RTC is currently busy, spin until it's not.
      // to do: add support for RTC timer initialization
      RTC.PER             = 0xFFFF;
      #ifdef MILLIS_USE_TIMERRTC_XTAL
        _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,0x03);
        RTC.CLKSEL        = 2; // external crystal
      #else
        _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA,0x02);
        // RTC.CLKSEL=0; this is the power on value
      #endif
      RTC.INTCTRL         = 0x01; // enable overflow interrupt
      RTC.CTRLA           = (RTC_RUNSTDBY_bm|RTC_RTCEN_bm|RTC_PRESCALER_DIV32_gc);//fire it up, prescale by 32.
    #else // It's a type b timer - we have already errored out if that wasn't defined
      _timer->CCMP = TIME_TRACKING_TIMER_PERIOD;
      // Enable timer interrupt, but clear the rest of register
      _timer->INTCTRL = TCB_CAPT_bm;
      // Clear timer mode (since it will have been set as PWM by init())
      _timer->CTRLB = 0;
      // CLK_PER/1 is 0b00, . CLK_PER/2 is 0b01, so bitwise OR of valid divider with enable works
      _timer->CTRLA = TIME_TRACKING_TIMER_DIVIDER|TCB_ENABLE_bm;  // Keep this last before enabling interrupts to ensure tracking as accurate as possible
    #endif
  #endif
}

int main()
{
  //onBeforeInit(); // Emnpty callback called before init but after the .init stuff. First normal code executed
  //init(); // Interrupts are turned on just prior to init() returning.
  init_millis();
  init_clock();
  //initVariant();
  _PROTECTED_WRITE(CPUINT_CTRLA,CPUINT_IVSEL_bm);
  //if (!onAfterInit()) sei();  // enable interrupts.
  VortexEngine::init();
  for (;;) {
    VortexEngine::tick();
  }
  return 0;
}
