#ifndef TIMINGS_H
#define TIMINGS_H

// so that anything which uses a timing can have access to msToTicks
#include "TimeControl.h"
#include "../VortexConfig.h"

// local definition which converts the global configuration that
// is defined in milliseconds into a time in ticks that can be
// used for comparisons in the menu
#define MENU_TRIGGER_THRESHOLD_TICKS  MS_TO_TICKS(MENU_TRIGGER_TIME)
#define SHORT_CLICK_THRESHOLD_TICKS   MS_TO_TICKS(CLICK_THRESHOLD)

#define CONSECUTIVE_WINDOW_TICKS      MS_TO_TICKS(CONSECUTIVE_WINDOW)

#define AUTO_RANDOM_DELAY_TICKS       MS_TO_TICKS(AUTO_RANDOM_DELAY)

// Color delete threshold (in milliseconds)
#define DELETE_THRESHOLD_TICKS        MS_TO_TICKS(COL_DELETE_THRESHOLD)
// Color delete cycle time (in milliseconds)
#define DELETE_CYCLE_TICKS            MS_TO_TICKS(COL_DELETE_CYCLE)

// Factory reset time
#define FACTORY_RESET_THRESHOLD_TICKS MS_TO_TICKS(RESET_HOLD_TIME)

// How fast serial can check for connections
#define MAX_SERIAL_CHECK_INTERVAL     MS_TO_TICKS(SERIAL_CHECK_TIME)

// How long to wait before timing out IR receive
#define MAX_TIMEOUT_DURATION          MS_TO_TICKS(IR_RECEIVER_TIMEOUT_DURATION)

// How long to wait between each IR send
#define MAX_WAIT_DURATION             MS_TO_TICKS(IR_SENDER_WAIT_DURATION)

// how long the user must press on a menu entry to enter the advanced menu
#define ADV_MENU_DURATION_TICKS       MS_TO_TICKS(ADVANCED_MENU_ENTER_DURATION)

// Strobe Timings
//
// Below are timings for all different kinds of standard
// strobes as defined by the gloving community.  All times
// are in milliseconds.

// Basic Strobe
#define STROBE_ON_DURATION          6
#define STROBE_OFF_DURATION         6

// Hyperstrobe
#define HYPERSTROBE_ON_DURATION     36
#define HYPERSTROBE_OFF_DURATION    26

// Dops
#define DOPS_ON_DURATION            6
#define DOPS_OFF_DURATION           40

// Dopish
#define DOPY_ON_DURATION            5
#define DOPY_OFF_DURATION           15

// Ultradops
#define ULTRADOPS_ON_DURATION       3
#define ULTRADOPS_OFF_DURATION      5

// Strobie
#define STROBIE_ON_DURATION         2
#define STROBIE_OFF_DURATION        28

// Signal
#define SIGNAL_ON_DURATION          16
#define SIGNAL_OFF_DURATION         120

// Ribbon
#define RIBBON_DURATION             6

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
