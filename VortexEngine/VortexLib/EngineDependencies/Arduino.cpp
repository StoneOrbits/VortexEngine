#include <Arduino.h>

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

void init_arduino()
{
}

void cleanup_arduino()
{
}

void delay(uint16_t ms)
{
}

void delayMicroseconds(uint32_t us)
{
}

// used for seeding randomSeed()
unsigned long analogRead(uint32_t pin)
{
  // random number generator will use this as seed, the answer to life
  return 42;
}

// used to read button input
unsigned long digitalRead(uint32_t pin)
{
  return Vortex::vcallbacks()->checkPinHook(pin);
}

void digitalWrite(uint32_t pin,  uint32_t val)
{
}

unsigned long millis()
{
  return 0;
}

unsigned long micros()
{
  return 0;
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

uint8_t *portOutputRegister(int port)
{
  return 0;
}

int digitalPinToPort(int pin)
{
  return 0;
}

int digitalPinToBitMask(int pin)
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
