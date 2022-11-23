#pragma once

// Here are various configuration options for the vortex engine as a whole

// The version number of the vortex engine
#define VORTEX_VERSION "1.0 beta"

// uncomment this to use palm lights
//#define USE_PALM_LIGHTS

// uncomment me to debug allocations
// NOTE: tracking allocations while using the test framework will be inaccurate
//       because the test framework allocations will be caught as well. Doing 
//       things like displaying a color for the first time will allocate space
//       and never really return it
//#define DEBUG_ALLOCATIONS

// uncomment this to turn on auto mode sender in mode sharing
//#define AUTO_SEND_MODE

// comment this out if you want the menu to fill from pinkie
// the logic is cleaner for fill from pinkie
#define FILL_FROM_THUMB

// Uncomment to demo all the patterns with rgb, note this will create one mode per pattern
//#define DEMO_ALL_PATTERNS

// The max number of colors in a colorset
#define MAX_COLOR_SLOTS 8

// The starting default global brightness
#define DEFAULT_BRIGHTNESS 255

// The maximum memory usage allowed by the memory tracker
// The memory tracker isn't present in final builds, only debug
// so this number should match the amount of memory on the device
#define MAX_MEMORY 50000

// The maximum number of modes that can be stored
#ifdef TEST_FRAMEWORK
// crank this up so we can demo all modes and generate the template
#define MAX_MODES     PATTERN_COUNT
#else
#define MAX_MODES     16
#endif

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
// of sync with the previous finger.
// NOTE: This was an early feature that fell into disrepair,
//       I don't think it works anymore and really isn't useful
#define DEFAULT_TICK_OFFSET 0

// Fixed Tickrate
//
// Uncomment this to enable a fixed tickrate. That will
// disable the setTickrate function and use a constant
// tickrate instead. Use this for final builds when you
// know the intended tickrate and don't need the variable
// tickrate functionality provided by the test framework
#ifndef TEST_FRAMEWORK
#define FIXED_TICKRATE
#endif

