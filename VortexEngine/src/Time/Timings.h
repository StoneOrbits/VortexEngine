#ifndef TIMINGS_H
#define TIMINGS_H

// so that anything which uses a timing can have access to msToTicks
#include "TimeControl.h"

// Menu Trigger Thresholds
//
// how long must hold to trigger ring menu and
// how long each ring menu takes to fill
#define MENU_TRIGGER_THRESHOLD Time::msToTicks(1000)
#define MENU_DURATION Time::msToTicks(1000)

// Short Click Threshold
//
// if click held for <= this value then the click will be registered as
// a 'short click' otherwise if held longer than this threshold it will
// be registered as a 'long click'
//
// The long hold is detected by just checking the holdDuration()
#define SHORT_CLICK_THRESHOLD Time::msToTicks(250)

// Strobe Timings
//
// Below are timings for all different kinds of standard
// strobes as defined by the gloving community.  All times 
// are in milliseconds.

// Basic Strobe
#define STROBE_ON_DURATION          5
#define STROBE_OFF_DURATION         8

// Hyperstrobe
#define HYPERSTROBE_ON_DURATION     25
#define HYPERSTROBE_OFF_DURATION    25

// Dops
#define DOPS_ON_DURATION            2
#define DOPS_OFF_DURATION           13

// Dopish
#define DOPISH_ON_DURATION          2
#define DOPISH_OFF_DURATION         7

// Ultradops
#define ULTRADOPS_ON_DURATION       1
#define ULTRADOPS_OFF_DURATION      3

// Strobie
#define STROBIE_ON_DURATION         3
#define STROBIE_OFF_DURATION        22

// Ribbon
#define RIBBON_DURATION             20

#endif
