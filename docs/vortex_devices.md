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

  - [Vortex Wired Gloveset](https://stoneorbits.com/products/the-vortex-gloves-1): you can read more [here](gloves_device.html)  
  - [Copact 28 Led Orbit](https://stoneorbits.com/products/copy-of-the-vortex-with-customs): you can read more [here](orbit_device.html)  
  - [3 Led Orbit Handle](https://stoneorbits.com/products/omega-handles): you can read more [here](handle_device.html)  
  - [2 Led Modular Chip (Coming soon)](): you can read more [here](duo_device.html)  
  - [Chromadeck Mode Storage (Coming soon)](): you can read more [here](chromadeck_device.html)  

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

