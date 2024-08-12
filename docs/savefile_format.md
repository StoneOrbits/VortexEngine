---
layout: default
title: Savefiles
nav_order: 30
parent: Vortex Engine
---

# Savefile Format

The Vortex Engine uses a specific format to save and transfer modes on your Vortex Devices. Understanding this format can help you better understand how devices share modes and translate each others modes.

## Overview

The savefile format is a binary format that stores the state of your Vortex Device. This includes the current mode list and any other settings that your device may have.

The savefile can be produced by connecting the device to the editor and pulling all modes from the device into the editor then navigating to File > Export to save the entire mode list as a .vortex file.

There are technically two kinds of savefiles, a .vortex file is a full vortex savefile which contains the engine version, all global device settings, and all modes on the device.

There is another kind of savefile, a .vtxmode savefile contains just the contents of a single mode along with the engine version. When transferring modes accross devices with the modesharing menu only a vtxmode is transferred.

It is up to you to decide which save to use for whatever circumstances you may face. If you want to just share a single mode with a friend then you might send them a link to the vtxmode file, where as if you wanted to share your entire set you might save a .vortex savefile.

## Mode Stretching and Chopping

When a mode is transferred from one device to another there is a step which must occur on the receiving device. After the mode has been received and while it is being loaded, the receiving device must make a decision based on the number of leds present in the mode savefile, and the number of leds the device currently exposes.

If the led count is different between an incoming savefile and the actual device led count, then "stretching" or "chopping" rules are applied.

The "stretching" rule will apply to a receiver which has more leds than then mode it received, it will repeat the smaller mode across the larger set of leds however many times necessary to fill the leds. For example an orbit receiving a duo mode would repeat that 2 led mode 14 times across it's leds.

The "chopping" rule will apply when a receiver has less leds than the mode it has received, this will simply chop off the remaining leds and only use the ones which fit on the device.  For example an orbit mode (28 leds) sent to a wired gloveset (10 leds) would chop the remaining 18 leds off and only use the first 10.

## Compatibility

The savefile format is designed to be forward and backward compatible. This means that savefiles created by older versions of the Vortex Engine can be read by newer versions, and vice versa. However, some features may not be available if you load a savefile created by a newer version of the Vortex Engine on an older version.

## Conclusion

Understanding the savefile format can help you better understand how your Vortex Device works. However, you don't need to worry about the details of the format unless you're interested in the inner workings of the Vortex Engine. The engine handles all the details for you, so you can focus on creating amazing light shows.

