---
layout: default
title: Building
nav_order: 9
parent: Source Code
---

# Building Vortex Engine

The steps to build Vortex Engine differ based on the device/branch you are targeting.

If you have access to Linux, this is the easiest way to build for the target device.

Alternatively windows is available but significantly more complicated and less supported.

## Common Build Steps

Whether building for Linux or Windows the first few steps are universal.

First checkout the repository:

```
git clone https://github.com/StoneOrbits/VortexEngine.git
```

Then switch to the desired device branch:
```
cd VortexEngine
git switch duo
```
Read moure about the branches [here](source_branches.html)

## Linux Build Steps
The following steps should work on Linux (Ubuntu/debian)

Run the install dependencies make target
```
make install
```

This should install all the required build dependencies to build the device.

Now run the build:
```
make
```

### Building Duo Example
After running make the output should end with something like this:

```
$ make
"C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/avr8/avr8-gnu-toolchain/bin/"/avr-objdump --disassemble --source --line-numbers --demangle --section=.text vortex.elf > vortex.lst
"C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/avr8/avr8-gnu-toolchain/bin/"/avr-nm --numeric-sort --line-numbers --demangle --print-size --format=s vortex.elf > vortex.map
chmod +x avrsize.sh
./avrsize.sh vortex.elf 0x7e00
Success! Program uses 32150/32768 or 0x7d96/0x8000 bytes of flash space (98.11%)
Global variables use 167/2048 or 0xa7/0x800 bytes of SRAM (8.15%)
== Success building Duo v1.5.14 ==
```

This should produce several files, the important one is **vortex.bin**

From here, with a **Chromadeck** and **Chromalink**, use the **Flash Custom Firmware** option on [lightshow.lol](https://lightshow.lol) to flash a Duo with **vortex.bin**

Congratulations the Duo is now running a custom firmware

## Building Other Devices

Other devices are slightly more complicated, reachout on [discord](support.html)

## Windows Build Steps
This is quite a pain and not really worth documenting, it would be easier to automate this with similar commands to Linux than to document the current process.

If you really need to build on Windows just join the [discord](support.html) and ask, I'll be happy to help.