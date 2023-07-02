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
#define FORCE_SLEEP_THRESHOLD_TICKS   Time::msToTicks(FORCE_SLEEP_TIME)
#define INSTANT_ON_OFF_TICKS          Time::msToTicks(INSTANT_ON_OFF_TIME)

// the window of how many ticks within the user must
// click the button 5 times to enable the lock
#define LOCK_CLICK_WINDOW_TICKS       Time::msToTicks(LOCK_CLICK_WINDOW)

#define CONSECUTIVE_WINDOW_TICKS      Time::msToTicks(CONSECUTIVE_WINDOW)
#define UNLOCK_WAKE_WINDOW_TICKS      Time::msToTicks(UNLOCK_WAKE_WINDOW)

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

// how long the user must press on a menu entry to enter the advanced menu
#define ADV_MENU_DURATION_TICKS       Time::msToTicks(ADVANCED_MENU_ENTER_DURATION)

// Strobe Timings
//
// Below are timings for all different kinds of standard
// strobes as defined by the gloving community.  All times
// are in milliseconds.

// Basic Strobe
// TODO: Should this be 5/13?
#define STROBE_ON_DURATION          4
#define STROBE_OFF_DURATION         4

// Hyperstrobe
#define HYPERSTROBE_ON_DURATION     24
#define HYPERSTROBE_OFF_DURATION    20

// Dops
#define DOPS_ON_DURATION            3
#define DOPS_OFF_DURATION           24

// Dopish
#define DOPISH_ON_DURATION          3
#define DOPISH_OFF_DURATION         8

// Ultradops
#define ULTRADOPS_ON_DURATION       2
#define ULTRADOPS_OFF_DURATION      3

// Strobie
#define STROBIE_ON_DURATION         1
#define STROBIE_OFF_DURATION        16

// Signal
#define SIGNAL_ON_DURATION          16
#define SIGNAL_OFF_DURATION         120

// Ribbon
#define RIBBON_DURATION             4

// Blink Timings
//
// Below are blink timings used for various menus and indication blinks

// Bulb selection blink speed
#define BULB_SELECT_ON_MS           150
#define BULB_SELECT_OFF_MS          50

// Exit Menu blink speed
#define EXIT_MENU_ON_MS             300
#define EXIT_MENU_OFF_MS            150

#endif
