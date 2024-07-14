---
layout: default
title: Vortex Devices
nav_order: 4
has_children: true
---

## What is Vortex Engine?

The purpose of Vortex Engine, and the reason for it's creation, is devices with arrays of leds used to produce visually appealing lightshows when a user spins, manipulates or dances with the device. Vortex Engine is the core piece of software which allows the users to configure the patterns, colors, and settings to produce consistent colorful patterns based on their preferences. 

The engine provides the framework of everything necessary for a 'Vortex Device' to function (input, storage, timing, pattern format) while ensuring that all of the devices are compatible with one another. For example, regardless of how many leds a device has, Vortex Engine will ensure the pattern and colorset configurations can be shared with any other Vortex Device.

## What is a Vortex Device?

Currently, the only Vortex Devices in existence are the lightshow products from [StoneOrbits](https://www.stoneorbits.com). However, any device with at least one LED could become a 'Vortex Device'. This means in theory anybody could create their own Vortex Devices by assembling some hardware and configuring vortex engine.

The four devices currently available include:

 - [StoneOrbits Vortex Gloves](https://stoneorbits.com/products/the-vortex-gloves-1): The single best wired gloveset
 - [StoneOrbits Vortex Orbit](https://stoneorbits.com/products/copy-of-the-vortex-with-customs): A compact 28 led orbit
 - [StoneOrbits Vortex Omega Handle](https://stoneorbits.com/products/omega-handles): Weighted 3 led Orbit handles
 - [StoneOrbits Duo Chip](https://stoneorbits.com/products/duo-orbit-light): The new best modular orbit chip.
 - [StoneOrbits Chromadeck (Coming Son)](): The super hub to answer all your needs
 - [StoneOrbits Spark Orbit (Coming Son)](): The next generation of integrated orbit

You can read more about the Vortex Devices [here](vortex_devices.html).

# Vortex Devices

This page aims to cover the main differences and similaries of each Vortex Device.

A Vortex Device must expose some amount of LEDs to display the patterns and colors that can be configured, a button is recommended
so that the user can input commands to the vortex device.

The available Vortex Devices incldue:

  - [Vortex Gloves](gloves_device.html)  
  - [Vortex Orbit](orbit_device.html)  
  - [Omega Handle](handle_device.html)  
  - [Vortex Duo](duo_device.html)  
  - [Chromadeck](chromadeck_device.html)  

## Mode Sharing Capabilities

Each device has a modesharing menu but some devices lack modesharing hardware

The capabilities of each device looks like this:

               send IR | recv IR | send VL | recv VL | connect PC  
      gloves      x         x         x                    x  
       orbit      x         x         x                    x  
      handle      x         x         x                    x  
         duo                          x         x             
        deck      x         x         x         x          x  

This means that all devices except the duo can share with each other via Infrared. But if one of those devices
wishes to share with a duo, they must send via Visible Light to the duo.

The duo can share with other duos via Visible Light.

The one major restriction is the duo cannot send to other vortex devices because they do not support receiving Visible Light.

In the future we hope to create new Vortex Devices to bridge this gap, particularly some form of device to receive VL that
can also connect to the PC editor and send/recv IR.

