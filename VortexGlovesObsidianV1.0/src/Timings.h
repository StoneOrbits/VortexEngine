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

// Tickrate in Ticks Per Second (TPS)
//
//    Default: 1000
//
// The valid range for this is 1 <= x <= 1000000
//
// However any value near or above 10000 will most likely be 
// too fast for the processor to handle
#define DEFAULT_TICKRATE 1000

// Finger time offset in ticks
//
// This changes how many ticks out of sync each finger
// will run. So 33 means each finger runs 33 ticks out
// of sync with the previous finger
#define DEFAULT_TICK_OFFSET 33

// Fixed Tickrate
//
// Uncomment this to enable a fixed tickrate. That will
// disable the setTickrate function and use a constant
// tickrate instead. Use this for final builds when you
// know the intended tickrate.
#ifndef TEST_FRAMEWORK
#define FIXED_TICKRATE
#endif

#endif
