#include "VortexEngine.h"

#include <Arduino.h>

static void init_clock()
{
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
}

static void init_millis()
{
  TCD_t* pTCD;
  _fastPtr_d(pTCD, &TCD0);
  pTCD->CMPBCLR        = TIME_TRACKING_TIMER_PERIOD; // essentially, this is TOP
  pTCD->CTRLB          = 0x00; // oneramp mode
  pTCD->CTRLC          = 0x80;
  pTCD->INTCTRL        = 0x01; // enable interrupt
  pTCD->CTRLA          = TIMERD0_PRESCALER | 0x01; // set clock source and enable!
}

extern "C" void __cxa_pure_virtual(void) {}
extern "C" void __cxa_deleted_virtual(void) {}

// For C++11, only need the following:
void *operator new  (size_t size) { return malloc(size); }
void *operator new[](size_t size) { return malloc(size); }
void  operator delete  (void * ptr) { free(ptr); }
void  operator delete[](void * ptr) { free(ptr); }
void * operator new  (size_t size, void * ptr) noexcept { return ptr; }
void * operator new[](size_t size, void * ptr) noexcept { return ptr; }
void  operator delete  (void* ptr, size_t size) noexcept { free(ptr); }
void  operator delete[](void* ptr, size_t size) noexcept { free(ptr); }
void* operator new  (size_t size, std::align_val_t al) { return malloc(size); }
void* operator new[](size_t size, std::align_val_t al) { return malloc(size); }
void  operator delete  (void* ptr, std::align_val_t al) noexcept { free(ptr); }
void  operator delete[](void* ptr, std::align_val_t al) noexcept { free(ptr); }
void  operator delete  (void* ptr, size_t size, std::align_val_t al) noexcept{ free(ptr); }
void  operator delete[](void* ptr, size_t size, std::align_val_t al) noexcept { free(ptr); }

int main()
{
  init_millis();
  init_clock();
  _PROTECTED_WRITE(CPUINT_CTRLA,CPUINT_IVSEL_bm);
  VortexEngine::init();
  for (;;) {
    VortexEngine::tick();
  }
  return 0;
}
