#include "VortexEngine.h"

#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/io.h>

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
  VortexEngine::init();
  _PROTECTED_WRITE(CPUINT_CTRLA,CPUINT_IVSEL_bm);
  for (;;) {
    VortexEngine::tick();
  }
  return 0;
}
