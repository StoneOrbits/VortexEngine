#include "Log.h"

#include "TimeControl.h"
#include "Serial.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <string>

#include <Arduino.h>

#ifdef TEST_FRAMEWORK
#ifdef LINUX_FRAMEWORK
#include "TestFrameworkLinux.h"
#else
#include "TestFramework.h"
#endif
#endif

using namespace std;

void DebugMsg(const char *file, const char *func, int line, const char *msg, ...)
{
  if (!SerialComs::initialized()) {
    return;
  }
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
  if (!SerialComs::initialized()) {
    return;
  }
  va_list list;
  va_start(list, msg);
#ifdef TEST_FRAMEWORK
  TestFramework::printlog(NULL, func, 0, msg, list);
#else
  char fmt[2048] = {0};
  snprintf(fmt, sizeof(fmt), "%s(): %s", func, msg);
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), fmt, list);
  Serial.println(buf);
#endif
  va_end(list);
}

void InfoMsg(const char *msg, ...)
{
  if (!SerialComs::initialized()) {
    return;
  }
  va_list list;
  va_start(list, msg);
#ifdef TEST_FRAMEWORK
  TestFramework::printlog(NULL, NULL, 0, msg, list);
#else
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), msg, list);
  Serial.println(buf);
#endif
  va_end(list);
}

#ifdef TEST_FRAMEWORK

// a line of spaces used for indent logging
#define SPACES "                                                                                                                                "
// a str with a given amount of spaces
#define SPACED_STR(num_spaces) ((SPACES + sizeof(SPACES)) - num_spaces)

void IndentMsg(int level, const char *msg, ...)
{
  if (!SerialComs::initialized()) {
    return;
  }
  if (level < 0 || level > (int)(sizeof(SPACES) - 1)) {
    return;
  }
  va_list list;
  va_start(list, msg);
  std::string spaced_msg = SPACED_STR(level * 2);
  spaced_msg += msg;
#ifdef TEST_FRAMEWORK
  TestFramework::printlog(NULL, NULL, 0, spaced_msg.c_str(), list);
#else
  char buf[2048] = {0};
  vsnprintf(buf, sizeof(buf), spaced_msg.c_str(), list);
  Serial.println(buf);
#endif
  va_end(list);
}
#endif
