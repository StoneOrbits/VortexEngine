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

## The Vortex Editors

Vortex Engine doesn't just power embedded lightshow devices. The software can also be built for desktop computers to implement 'virtual Vortex Devices'.  A 'virtual Vortex Device', although sounding complicated, is actually the best way to provide a PC-based savefile editor for Vortex devices.

The best example of a 'virtual Vortex Device' is the website [lightshow.lol](https://lightshow.lol), here you see the output of a single-led 'virtual device' with a series of controls to edit the pattern and colorset in realtime.

Another example of a , a Windows-only desktop software that can connect to Vortex Devices and program them via USB.

The editor allows for configurations that are otherwise unobtainable through the device menus. You can read more about the editor [here](editor.html).

## The Vortex Community and Ecosystem

All Vortex Devices can transmit and receive their configurations in some way. This allows for an ecosystem where users device configurations can transfer to other devices to create matching patterns and colorsets.

Explore the official Vortex Community Hub where you can upload and share configurations and modes with friends and other lightshow artists.

[Visit Vortex Community Hub](https://vortex.community)

Join the Vortex Community Discord to share your experiences, enhance your use of the Vortex Engine, and discuss Vortex with other enthusiasts.

[Join Vortex Community Discord](https://discord.gg/FnbKjPgy)



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

