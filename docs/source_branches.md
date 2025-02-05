---
layout: default
title: Branches
nav_order: 0
parent: Source Code
---

# Source Branches

Vortex Engine itself is the "core" software that powers all devices. However, each device runs a derivation of the core that has been specifically modified for that device.

The primary differences between the core and each device are the hardcoded number of LEDs being controlled (as each device may have a different count) and the device menus, such as Color Selection or Randomizer.

Each device and its specific changes are maintained in branches named after the devices. These branches are permanent and continuously receive reintegration updates whenever the core is modified.

At the time of writing, the existing branches include:

### The Core Branch
The core is stored in the **master** branch, the default main branch of the repository. The core is configured as a 1-LED device with agnostic menus, while each device branch adjusts LED enumeration and potentially modifies menus to match its specific LED count or layout.

The goal is for the core to run on any device, but it may only power the first LED or treat all LEDs the same.

The core also implements features that may only exist on certain devices. For example, wireless senders and receivers are not universally present:

- Older devices have infrared senders and receivers but no light receiver.
- The Duo has a light receiver but no infrared hardware.
- The Chromadeck and Spark have both infrared and light receivers.

All devices have light-sending hardware (LEDs), but only those with light-receiving hardware (Duo, Chromadeck, and Spark) can receive wireless LED blink transfers.

Even though the core includes sender and receiver logic, individual device branches may need to override sender or receiver configurations to match their specific hardware.

### Desktop Library Branch
The Vortex Engine Desktop Library, known as **VortexLib**, is stored in the **desktop** branch.

VortexLib is designed for desktop PCs, enabling software that interfaces with Vortex Engine devices. For example, both the Vortex Editor and lightshow.lol are powered by VortexLib.

### Device Branches
The remaining branches are dedicated to specific devices with unique hardware configurations. Each branch is named after its corresponding device:

- gloves
- handle
- orbit
- duo
- spark
- chromadeck

More details on each device can be found in the [Vortex Devices Section](vortex_devices.html).

## Reintegrating Changes

Whenever changes are made to the core (master), they must be reintegrated across all device branches.

The first step in reintegration is updating **master** with the latest core changes.

After master has been updated, pull requests for each device can be created by merging master into the respective device branch.

Follow these steps:

1. Create a new branch for the device reintegration, e.g.:  
   `git branch -c name/orbit/reintegrate-new-fix`
2. Create a pull request to merge the new branch into the device branch, e.g.:  
   `name/orbit/reintegrate-new-fix -> orbit`
3. When merging the pull request, **DO NOT SQUASH OR REBASE**.  
   A regular merge commit must be used to retain history.
