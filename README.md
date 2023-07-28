# Vortex Engine

Welcome to the Vortex Engine firmware repository! This open-source project is designed for use with our Arduino-based LED devices: Orbit, Gloves, Handle, and Duo chip. The firmware enables users to create mesmerizing light shows by completely unlocking their ability to program any flashing patterns, colorsets, or effects onto their devices.

**Note**: While this firmware can be modified and re-flashed by experienced users, it is not intended for basic users to do so.

## Repository Structure

This repository is organized into branches, with each branch corresponding to one of the four devices:

&nbsp;&nbsp;[![Core](https://github.com/StoneOrbits/VortexEngine/actions/workflows/core_build.yml/badge.svg?branch=master)](https://github.com/StoneOrbits/VortexEngine/actions/workflows/core_build.yml) Firmware core that all branches derive from  
&nbsp;&nbsp;[![Orbit](https://github.com/StoneOrbits/VortexEngine/actions/workflows/orbit_build.yml/badge.svg?branch=orbit)](https://github.com/StoneOrbits/VortexEngine/actions/workflows/orbit_build.yml) Firmware for the Vortex Classic Orbit  
&nbsp;&nbsp;[![Handle](https://github.com/StoneOrbits/VortexEngine/actions/workflows/handle_build.yml/badge.svg?branch=handle)](https://github.com/StoneOrbits/VortexEngine/actions/workflows/handle_build.yml) Firmware for the Vortex Omega Handles  
&nbsp;&nbsp;[![Gloves](https://github.com/StoneOrbits/VortexEngine/actions/workflows/gloves_build.yml/badge.svg?branch=gloves)](https://github.com/StoneOrbits/VortexEngine/actions/workflows/gloves_build.yml) Firmware for the Vortex Wired Gloveset  
&nbsp;&nbsp;[![Duo](https://github.com/StoneOrbits/VortexEngine/actions/workflows/duo_build.yml/badge.svg?branch=duo)](https://github.com/StoneOrbits/VortexEngine/actions/workflows/duo_build.yml) Firmware for the Vortex Duo Modular Chip  

To access the firmware for a specific device, switch to the corresponding branch.

## Contributing

We welcome and encourage contributions from the community! If you would like to contribute to this project, please follow our [contribution guidelines](CONTRIBUTING.md).

Note: While this firmware can be modified and re-flashed by experienced users, it is not intended for basic users to do so.

### Installation
In order to get started first you'll need to set up Arduino IDE
Download the latest version of the program for your operating system here
https://www.arduino.cc/en/software 

### Setup 
Next we'll need to configure Arduino to work with our hardware!

#### Trinket Based Devices

For the Orbit, Gloves and Handle you need to setup arduino for a trinket m0

https://learn.adafruit.com/adafruit-trinket-m0-circuitpython-arduino/arduino-ide-setup

You will need to select the following board to install, make sure to select the correct version

#### ATTiny Based Devices

For the attiny based devices (duo chip) you will need to get megaTinyCore installed and you will need an Arduino Uno to be used as a jtag2updi programmer.

### Library Setup
Lastly we need to copy the libraries folder to the directory C:\Users\YourPCName\Documents\Arduino
This should give arduino everything it needs to work with the Vortex Gloves!

Check out the Vortex Testing Framework to run the framework on your desktop
