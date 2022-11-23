#ifndef VORTEX_CONFIG_H
#define VORTEX_CONFIG_H

// Here are various configuration options for the vortex engine as a whole

// The version number of the vortex engine
#define VORTEX_VERSION        "1.0 beta"

// ===================================================================
//  Numeric Configurations

// Max Color Slots
//
// The max number of colors in a colorset, this was never tested with
// anything other than 8, you have been warned
#define MAX_COLOR_SLOTS       8

// Default Global Brightness
//
// The starting default global brightness if there is no savefile present
// The maximum value is 255
#define DEFAULT_BRIGHTNESS    255

// Max Modes
//
// The maximum number of modes that can be active at once
// this should reflect the available RAM of the device.
// However it is also heavily influenced by the active patterns
// so it's hard to pick a number, 16 seems reasonable
#define MAX_MODES             16

// Tickrate in Ticks Per Second (TPS)
//
// The valid range for this is 1 <= x <= 1000000 (default 1000)
//
// Any value near or above 10000 will most likely be too fast for
// the processor to handle.
//
// Any value less than 100 and you risk a single tick taking longer
// than some pattern timings which results in unexpected behaviour
#define DEFAULT_TICKRATE      1000

// Finger time offset in ticks
//
// This changes how many ticks out of sync each finger will run.
// So 33 means each finger runs 33 ticks out of sync with the
// previous finger.
//
// This was an early feature that fell into disrepair, I don't
// think it works anymore and really hasn't proven to be useful
#define DEFAULT_TICK_OFFSET   0

// Max Memory Usage
//
// The maximum memory usage allowed by the memory tracker.
// The memory tracker isn't present in final builds, only debug
// so this number doesn't actually do anything in production.
// Mostly for catching leaks or high memory usage in development.
#define MAX_MEMORY            50000

// ===================================================================
//  Boolean Configurations (0 or 1)

// Fill From Thumb
//
// The ring menu will fill from the thumb if this is present, otherwise it
// will fill from the pinkie.
//
// The logic is cleaner for fill from pinkie but fill from thumb is preferred
#define FILL_FROM_THUMB       1

// Use Palm Lights
//
// Adjust the engine to account for palm lights
#define USE_PALM_LIGHTS       0

// Demo All Patterns
//
// The default modes that are set on the gloveset will be dynamically
// generated with one mode per pattern in the patterns list
//
// This can be used to quickly demo all possible patterns, mostly useful
// for testing and development
#define DEMO_ALL_PATTERNS     0

// Save Template
//
// Dump the initialized modes in json format to either serial or the console.
// This will generate a json structure that mirrors that of the binary save
// file. The result of this can be found in example_modes.json
#define SAVE_TEMPLATE         1

// Debug Allocations
//
// Tracks all memory allocations and logs them, useful for finding leaks
//
// Note tracking allocations while using the test framework will be inaccurate
// because the test framework allocations will be caught as well.
//
// When the test framework does things like display a color for the first time
// it will allocate space permanently for the new brush and never free it
#define DEBUG_ALLOCATIONS     0

// Variable Tickrate
//
// This controls whether the setTickrate function is available and
// whether changing the tickrate is allowed.
//
// The tickrate should always be fixed in final builds because this
// functionality induces extra performance costs and the intended
// tickrate for the final build should already be known.
//
// However there may be some clever uses for variable tickrate in
// the final build? I'm not sure.
#define VARIABLE_TICKRATE     0

// Auto Mode-Sharing
//
// Automatically share the mode every 3 seconds in 'sender' mode
// as opposed to requiring a long click to send the mode
//
// This often has issues and should be used carefully
#define AUTO_SEND_MODE        0

// ===================================================================
//  Test Framework configurations

#ifdef TEST_FRAMEWORK

// When running in the test framework with demo all patterns enabled
// we should change the max patterns to the total pattern count because
// the test framework can handle the memory usage and we can't demo
// all the patterns without the increased limit
#if DEMO_ALL_PATTERNS == 1
  #undef MAX_MODES
  #include "Patterns/Patterns.h"
  #define MAX_MODES           PATTERN_COUNT
#endif

// In the test framework variable tickrate must be enabled to allow
// the tickrate slider to function, also the test framework never runs
// at full tickrate, maximum is 500 tps
#undef VARIABLE_TICKRATE
#define VARIABLE_TICKRATE 1

#endif // TEST_FRAMEWORK


#endif // VORTEX_CONFIG_H
