---
layout: default
title: Vortex Emulator
nav_order: 2
parent: Vortex Software
---

# Vortex Emulator

This is sometimes called the Vortex 'Test Framework' because it was originally designed for rapidly developing and testing the Vortex Engine firmwares, but later was renamed to the Vortex Emulator.

The Vortex Emulator is a tool developed to simulate the behavior of Vortex Devices on your computer. It doesn't serve much use to a user other than for fun, but it is a valuable tool for development and debugging of device firmwares.

## Features

### Device Simulation

The Vortex Emulator can simulate all types of Vortex Devices, including the Duo, Gloves, Orbit, Orbit Handle, and ChromaDeck. 

### Mode Testing

The Vortex Emulator allows running modes and testing them in a simulated environment. This is particularly useful for testing complex modes or debugging issues, as it allows you to see exactly how the mode will behave on a device.

### Integration with Vortex Editor

The Vortex Emulator is integrated with the Desktop Vortex Editor, it is able to connect via a simulated usb connection from program to program.

## Source Code

You can find the Vortex Emulator / Test Framework [here](https://github.com/StoneOrbits/VortexEmulator)

In order to build the Vortex Emulator it requires some version of Vortex Engine exist within it's main folder. This can be any branch of Vortex Engine like 'orbit', 'handles' or 'duo'.