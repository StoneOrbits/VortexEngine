---
layout: default
title: Vortex Devices
nav_order: 2
has_children: true
---

# Vortex Devices

This page aims to cover the main differences and similaries of each Vortex Device.

A Vortex Device must expose some amount of LEDs to display the patterns and colors that can be configured, a button is recommended
so that the user can input commands to the vortex device.

Below you will find a brief description of each device along with some sections describing the capabilities.

  - [Vortex 20 led Wired Gloveset](gloves_device.html)  
  - [Vortex Orbit Copact 28 Led Orbit](orbit_device.html)  
  - [Omega Handle 3 Led Orbit Handle](handle_device.html)  
  - [Vortex Duo 2 Led Modular Chip](duo_device.html)  
  - [Chromadeck Mode Storage](chromadeck_device.html)  

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

