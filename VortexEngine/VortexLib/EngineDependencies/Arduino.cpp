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

#include "VortexLib.h"

using namespace std;

SerialClass Serial;

#if !defined(_MSC_VER) || defined(WASM)
uint64_t start = 0;
#else
static LARGE_INTEGER start;
static LARGE_INTEGER tps; //tps = ticks per second
#endif

void init_arduino()
{
#if !defined(_MSC_VER) || defined(WASM)
  start = micros();
#else
  QueryPerformanceFrequency(&tps);
  QueryPerformanceCounter(&start);
#endif
}

void cleanup_arduino()
{
}

void delay(size_t amt)
{
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
  return Vortex::vcallbacks()->readHook(pin);
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
  if (!tps.QuadPart) {
    return 0;
  }
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

void send_ir(bool mark, uint32_t duration)
{
  Vortex::vcallbacks()->infraredWrite(mark, duration);
}

int digitalPinToInterrupt(int pin)
{
  return 0;
}

void SerialClass::begin(uint32_t baud)
{
  Vortex::vcallbacks()->serialBegin(baud);
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
  return Vortex::vcallbacks()->serialWrite(buf, len);
}

SerialClass::operator bool()
{
  if (connected) {
    return true;
  }
  if (!Vortex::vcallbacks()->serialCheck()) {
    return false;
  }
  connected = true;
  return true;
}

int32_t SerialClass::available()
{
  return Vortex::vcallbacks()->serialAvail();
}

size_t SerialClass::readBytes(char *buf, size_t amt)
{
  return Vortex::vcallbacks()->serialRead(buf, amt);
}

uint8_t SerialClass::read()
{
  uint8_t byte = 0;
  if (!readBytes((char *)&byte, 1)) {
    return 0;
  }
  return byte;
}
