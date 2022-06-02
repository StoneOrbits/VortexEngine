#include "Log.h"

#include <stdarg.h>
#include <stdio.h>

#include <Arduino.h>

#ifdef TEST_FRAMEWORK
#ifdef LINUX_FRAMEWORK
#include "TestFrameworkLinux.h"
#else
#include "TestFramework.h"
#endif
#endif

void DebugMsg(const char *file, const char *func, int line, const char *msg, ...)
{
  va_list list;
  va_start(list, msg);
#ifdef TEST_FRAMEWORK
  TestFramework::printlog(file, func, line, msg, list);
#else
  char fmt[2048] = {0};
  snprintf(fmt, sizeof(fmt), "%s:%d %s(): %s", file, line, func, msg);
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), fmt, list);
  Serial.println(buf);
#endif
  va_end(list);
}


void ErrorMsg(const char *func, const char *msg, ...)
{
  char fmt[2048] = {0};
  snprintf(fmt, sizeof(fmt), "%s(): %s", func, msg);
  char buf[2048] = {0};
  va_list list;
  va_start(list, msg);
  vsnprintf(buf, sizeof(buf), fmt, list);
  va_end(list);
  Serial.println(buf);
}
