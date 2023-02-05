#include "Arduino.h"

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "VortexConfig.h"

#include <chrono>
#include <random>
#include <string>
#include <time.h>

#include "EngineWrapper.h"

using namespace std;

SerialClass Serial;

#ifdef _MSC_VER
static LARGE_INTEGER start;
static LARGE_INTEGER tps; //tps = ticks per second

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  // Perform actions based on the reason for calling.
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    VEngine::init();
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    VEngine::cleanup();
    break;
  }
  return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif

void init_arduino()
{
#ifdef LINUX_FRAMEWORK
  start = micros();
#elif TEST_FRAMEWORK
  QueryPerformanceFrequency(&tps);
  QueryPerformanceCounter(&start);
#endif
}

void cleanup_arduino()
{
}

void delay(size_t amt)
{
#ifndef _MSC_VER
  sleep(amt);
#else
  Sleep((DWORD)amt);
#endif
}

void delayMicroseconds(size_t us)
{
}

// used for seeding randomSeed()
unsigned long analogRead(uint32_t pin)
{
  return 0;
}

// used to read button input
unsigned long digitalRead(uint32_t pin)
{
#ifdef TEST_FRAMEWORK
  // get button state
  if (pin == 19) {
    if (g_pTestFramework->isButtonPressed(0)) {
      return LOW;
    }
  }
  if (pin == 20) {
    if (g_pTestFramework->isButtonPressed(1)) {
      return LOW;
    }
  }
#endif
  return HIGH;
}

void digitalWrite(uint32_t pin,  uint32_t val)
{
}

/// Convert seconds to milliseconds
#define SEC_TO_MS(sec) ((sec)*1000)
/// Convert seconds to microseconds
#define SEC_TO_US(sec) ((sec)*1000000)
/// Convert seconds to nanoseconds
#define SEC_TO_NS(sec) ((sec)*1000000000)

/// Convert nanoseconds to seconds
#define NS_TO_SEC(ns)   ((ns)/1000000000)
/// Convert nanoseconds to milliseconds
#define NS_TO_MS(ns)    ((ns)/1000000)
/// Convert nanoseconds to microseconds
#define NS_TO_US(ns)    ((ns)/1000)

unsigned long millis()
{
#ifndef _MSC_VER
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  uint64_t ms = SEC_TO_MS((uint64_t)ts.tv_sec) + NS_TO_MS((uint64_t)ts.tv_nsec);
  return (unsigned long)ms;
#else
  return (unsigned long)GetTickCount();
#endif
}

unsigned long micros()
{
#ifndef _MSC_VER
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  uint64_t us = SEC_TO_US((uint64_t)ts.tv_sec) + NS_TO_US((uint64_t)ts.tv_nsec);
  return (unsigned long)us;
#else
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  // yes, this will overflow, that's how arduino micros() works *shrug*
  return (unsigned long)((now.QuadPart - start.QuadPart) * 1000000 / tps.QuadPart);
#endif
}

unsigned long random(uint32_t low, uint32_t high)
{
  return low + (rand() % (high - low));
}


void randomSeed(uint32_t seed)
{
#ifndef _MSC_VER
  srand(time(NULL));
#else
  srand((uint32_t)GetTickCount() ^ (uint32_t)GetCurrentProcessId());
#endif
}

void pinMode(uint32_t pin, uint32_t mode)
{
  // ???
}

void attachInterrupt(int interrupt, void (*func)(), int type)
{
}

void detachInterrupt(int interrupt)
{
}

void test_ir_mark(uint32_t duration)
{
#ifndef _MSC_VER
#endif
}

void test_ir_space(uint32_t duration)
{
#ifndef _MSC_VER
#endif
}

// this is only used on the windows framework to test IR
void (*IR_change_callback)(uint32_t data);
void installIRCallback(void (*func)(uint32_t))
{
  IR_change_callback = func;
}

int digitalPinToInterrupt(int pin)
{
  return 0;
}

void SerialClass::begin(uint32_t i)
{
}

void SerialClass::print(uint32_t i)
{
  std::string str = std::to_string(i);
  write((const uint8_t *)str.c_str(), str.length());
}

void SerialClass::print(const char *s)
{
  std::string str = s;
  write((const uint8_t *)str.c_str(), str.length());
}

void SerialClass::println(const char *s)
{
  std::string str = s;
  str += "\n";
  write((const uint8_t *)str.c_str(), str.length());
}

uint32_t SerialClass::write(const uint8_t *buf, size_t len)
{
  return 0;
}

SerialClass::operator bool()
{
  if (connected) {
    return true;
  }
  connected = true;
  return true;
}

int32_t SerialClass::available()
{
  int32_t amount = 0;
  return amount;
}

size_t SerialClass::readBytes(char *buf, size_t amt)
{
  size_t total = 0;
  return total;
}

uint8_t SerialClass::read()
{
  uint8_t byte = 0;
  if (!readBytes((char *)&byte, 1)) {
    return 0;
  }
  return byte;
}
