#ifndef TIMINGS_H
#define TIMINGS_H

// so that anything which uses a timing can have access to msToTicks
#include "TimeControl.h"
#include "../VortexConfig.h"

// local definition which converts the global configuration that
// is defined in milliseconds into a time in ticks that can be
// used for comparisons in the menu
#define MENU_TRIGGER_THRESHOLD_TICKS  Time::msToTicks(MENU_TRIGGER_TIME)
#define SHORT_CLICK_THRESHOLD_TICKS   Time::msToTicks(CLICK_THRESHOLD)

#define CONSECUTIVE_WINDOW_TICKS      Time::msToTicks(CONSECUTIVE_WINDOW)

#define AUTO_RANDOM_DELAY_TICKS       Time::msToTicks(AUTO_RANDOM_DELAY)

// the number of ticks to enter sleep mode
#define SLEEP_ENTER_THRESHOLD_TICKS   Time::msToTicks(SLEEP_TRIGGER_TIME)
#define SLEEP_WINDOW_THRESHOLD_TICKS  Time::msToTicks(SLEEP_WINDOW_TIME)

// Color delete threshold (in milliseconds)
#define DELETE_THRESHOLD_TICKS        Time::msToTicks(COL_DELETE_THRESHOLD)
// Color delete cycle time (in milliseconds)
#define DELETE_CYCLE_TICKS            Time::msToTicks(COL_DELETE_CYCLE)

// Factory reset time
#define FACTORY_RESET_THRESHOLD_TICKS Time::msToTicks(RESET_HOLD_TIME)

// How fast serial can check for connections
#define MAX_SERIAL_CHECK_INTERVAL     Time::msToTicks(SERIAL_CHECK_TIME)

// How long to wait before timing out IR receive
#define MAX_TIMEOUT_DURATION          Time::msToTicks(IR_RECEIVER_TIMEOUT_DURATION)

// How long to wait between each IR send
#define MAX_WAIT_DURATION             Time::msToTicks(IR_SENDER_WAIT_DURATION)

// Strobe Timings
//
// Below are timings for all different kinds of standard
// strobes as defined by the gloving community.  All times
// are in milliseconds.

// Basic Strobe
#define STROBE_ON_DURATION          8
#define STROBE_OFF_DURATION         10

// Hyperstrobe
#define HYPERSTROBE_ON_DURATION     32
#define HYPERSTROBE_OFF_DURATION    28

// Dops
#define DOPS_ON_DURATION            4
#define DOPS_OFF_DURATION           16

// Dopish
#define DOPISH_ON_DURATION          4
#define DOPISH_OFF_DURATION         12

// Ultradops
#define ULTRADOPS_ON_DURATION       2
#define ULTRADOPS_OFF_DURATION      5

// Strobie
#define STROBIE_ON_DURATION         3
#define STROBIE_OFF_DURATION        28

// Ribbon
#define RIBBON_DURATION             20

#endif
