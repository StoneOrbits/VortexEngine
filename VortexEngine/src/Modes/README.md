# Modes

This folder contains all content related to 'modes' and managing them.

## [Mode Class](Mode.h)

This defines a class that describes a 'Mode'.

A 'mode' is a state of operation on the Vortex Engine which maps one Pattern and one Colorset to each Led of the device.

## [Mode Builder Class](ModeBuilder.h)

This is a global manager class which implements the factory pattern to expose routines for instantiating Mode objects

## [Modes Class](Modes.h)

This is the global manager class for all Mode instances that are loaded.

This class manages running a single mode at a time, and upon receiving a click will iterate to the next Mode.
