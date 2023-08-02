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

## Core

The 'core' is not technically a Vortex Device but rather it is the core firmware from which all devices derive.

Technically the vortex core firmware cannot run on a device as it lacks the integrations for that device hardware.

As soon as the core is modified to run on a specific device it is technically no longer the 'core'.

## Gloves

 [Store Page](https://stoneorbits.com/products/the-vortex-gloves-1)

 The gloves are a wired gloveset, you can read more [here](gloves_device.html)

## Orbit

 [Store Page](https://stoneorbits.com/products/copy-of-the-vortex-with-customs)

 The orbit is a compact 28 led orbit, you can read more [here](orbit_device.html)

## Handle

 [Store Page](https://stoneorbits.com/products/omega-handles)

 The handle is a weighted 3 led orbit handle, you can read more [here](handle_device.html)

## Duo

 [Store Page (Coming soon)]()

 The duo is an unbeatable 2 led modular orbit chip, you can read more [here](duo_device.html)

## Chromadeck

## Mode Sharing Capabilities

Each device has a modesharing menu but some devices lack modesharing hardware

The capabilities of each device looks like this:

               send IR | recv IR | send VL | recv VL | connect PC  
      gloves      x         x         x                    x  
       orbit      x         x         x                    x  
      handle      x         x         x                    x  
         duo                          x         x             

This means that all devices except the duo can share with each other via Infrared. But if one of those devices
wishes to share with a duo, they must send via Visible Light to the duo.

The duo can share with other duos via Visible Light.

The one major restriction is the duo cannot send to other vortex devices because they do not support receiving Visible Light.

In the future we hope to create new Vortex Devices to bridge this gap, particularly some form of device to receive VL that
can also connect to the PC editor and send/recv IR.

