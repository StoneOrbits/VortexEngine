# Vortex CLI

This folder contains the command line interface vortex tool

This tool allows the vortex engine to be run on the command line and print it's output in various ways.

The usage includes options to control the engine in various ways, this allows for robust tests to be
designed around this tool to test the vortex engine to ensure it is behaving correctly.

The usage of the tool is as follows:

    Usage: ../vortex [options] < input commands
    Output Selection (at least one required):
      -x, --hex                Use hex values to represent led colors
      -c, --color              Use console color codes to represent led colors
    
    Engine Control Flags (optional):
      -t, --no-timestep        Bypass the timestep and run as fast as possible
      -l, --lockstep           Only step once each time an input is received
      -i, --in-place           Print the output in-place (interactive mode)
      -r, --record             Record the inputs and dump to a file after (recorded_input.txt)
      -a, --autowake           Automatically and instantly wake on sleep (disable sleep)
      -n, --nolock             Automatically unlock upon locking the chip (disable lock)
      -s, --storage [file]     Persistent storage to file (default file: FlashStorage.flash)
    
    Initial Pattern Options (optional):
      -P, --pattern <id>       Preset the pattern ID on the first mode
      -C, --colorset c1,c2...  Preset the colorset on the first mode (csv list of hex codes or color names)
      -A, --arguments a1,a2... Preset the arguments on the first mode (csv list of arguments)
    
    Other Options:
      -h, --help               Display this help message
    
    Input Commands (pass to stdin):
       c         standard short click
       l         standard long click
       m         open menus length click
       a         enter adv menu length click (enter adv menu from menus)
       d         delete length click (delete color in col select)
       s         enter sleep length click (enter sleep at main modes)
       f         force sleep length click (force sleep anywhere)
       t         toggle button pressed (only way to wake after sleep)
       r         rapid button click (ex: r15)
       w         wait 1 tick
       <digits>  repeat command n times (only single digits in -i mode)
       q         quit
    Example Usage:
       ./vortex -ci
       ./vortex -ci -P42 -Ccyan,purple
       ./vortex -ct -P0 -Cred,green -A1,2 <<< w10q
    
