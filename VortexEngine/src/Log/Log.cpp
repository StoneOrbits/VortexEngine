#include "Log.h"

#include "../Time/TimeControl.h"
#include "../Serial/Serial.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <Arduino.h>

#ifndef ARDUINO_BUILD
#include "VortexLib.h"
#endif

#if LOGGING_LEVEL > 0
void InfoMsg(const char *msg, ...)
{
#ifdef ARDUINO_BUILD
  if (!SerialComs::isConnected()) {
    return;
  }
#endif
  va_list list;
  va_start(list, msg);
#ifdef ARDUINO_BUILD
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), msg, list);
  Serial.println(buf);
#else
  Vortex::printlog(NULL, NULL, 0, msg, list);
#endif
  va_end(list);
}
#endif

#if LOGGING_LEVEL > 1
void ErrorMsg(const char *func, const char *msg, ...)
{
#ifdef ARDUINO_BUILD
  if (!SerialComs::isConnected()) {
    return;
  }
#endif
  va_list list;
  va_start(list, msg);
#ifdef ARDUINO_BUILD
  char fmt[2048] = {0};
  snprintf(fmt, sizeof(fmt), "%s(): %s", func, msg);
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), fmt, list);
  Serial.println(buf);
#else
  Vortex::printlog(NULL, func, 0, msg, list);
#endif
  va_end(list);
}
#endif

#if LOGGING_LEVEL > 2
void DebugMsg(const char *file, const char *func, int line, const char *msg, ...)
{
#ifdef ARDUINO_BUILD
  if (!SerialComs::isConnected()) {
    return;
  }
#endif
  va_list list;
  va_start(list, msg);
  const char *ptr = file + strlen(file);
  // go backwards till either start or backslash
  while (ptr > file) {
    if (*ptr == '\\') {
      // file starts after the backslash
      file = ptr + 1;
      break;
    }
    ptr--;
  }
#ifdef ARDUINO_BUILD
  char fmt[2048] = {0};
  snprintf(fmt, sizeof(fmt), "%s:%d %s(): %s", file, line, func, msg);
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), fmt, list);
  Serial.println(buf);
#else
  Vortex::printlog(file, func, line, msg, list);
#endif
  va_end(list);
}
#endif
