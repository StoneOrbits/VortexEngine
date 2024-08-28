---
layout: default
title: Modes
nav_order: 15
parent: Vortex Engine
---

# Modes
Think of modes as containers that hold personalized color and pattern combinations. Each device has several independantly customizable modes, and each mode always contains at least one [Colorset](colorsets.html) and one [Pattern](patterns.html). The more LEDs the device has, the more complex a mode can become.

## Mode Slots
A device will always begin displaying the first mode slot when it is powered on. Short clicking the navigation button will iterate to the next slot. Upon reaching the last slot, the next click will cycle back to the first mode slot.

## Multiple LEDs
It is possible to give each LED in a mode its own pattern and colorset, this means a single mode could have as many patterns and colorsets as there are LEDs.

Regardless of how many LEDs a mode contains, it is still compatible with any Vortex Device. When a device loads a mode it will either trim or repeat the LEDs in the mode across it's own LEDs.

## Editing Modes

To edit a mode slot, navigate to it and open the menus by holding the navigation button until the lights start flashing white. Aside from adjusting [Global Brightness](global_brightness_menu.html) or [Factory Resetting](factory_reset_menu.html), any changes you make in the menus will only affect the currently selected Mode.

To continue reading about the Menus, go [here](menus.html).
