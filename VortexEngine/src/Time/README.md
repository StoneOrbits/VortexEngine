# Time

This folder contains all source files relating to time tracking or time management.

# [Time Control Class](TimeControl.h)

This is the global manager class for all Time operations.

This includes timestep and measurement of time, as well as converstion between ticks and seconds.

There is also the 'time simulation' feature which allows virtually stepping forward in time without actually stepping the tick counter forward. 
This is used for things like generating the pattern strip in the test framework, a single pattern is "simulated" many times in a single tick and
the led colors are sampled each simulated tick to generate a pattern strip.

# [Timer Class](Timer.h)

This is a timer class used for creating timed events in patterns.

You can think of this like a continuously running stopwatch, or like a cylinder rolling down a hill with notches on the outside of the cylinder.

The notches indicate 'alarms' on the timer, as the cylinder rolls and each notch touches the ground it triggers that 'alarm'.

# [Timings](Timings.h)

This contains global definitions for commonly reused timings and time measurements.
