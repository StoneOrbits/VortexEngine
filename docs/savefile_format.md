---
layout: default
title: Savefiles
nav_order: 30
parent: Vortex Engine
---

# Vortex Savefiles

Vortex Engine uses a custom format to save and transfer modes, the savefile for a mode can be acquired various different ways and it can be represented in different formats. 

The important thing to understand is the savefiles can be transferred across devices and adjusted to fit any device with any number of leds.

## Savefile Contents

The general idea of a savefile is to store the data that represents the blinking patterns and colorsets that make up a mode.  Since a device can have many leds a mode must contain a pattern and colorset for each led on the device.

## Acquring and Sharing Savefiles

Depending on the device being used, the savefile for modes can usually be acquired through https://lightshow.lol

Simply connecting the device and then loading the modes from the device will give access to the mode savefiles directly on lightshow.lol. These modes can be then shared on [Vortex Community](https://vortex.community)

The one exception is the Duo, acquring mode savefiles for the Duo requires access to a [Chromalink](duo_chromalink_guide.html)

## Mode Stretching and Chopping

When a mode is transferred from one device to another a step occurs on the receiving device. 

The receiving device will look at the number of leds in the mode, and compare that to the number of leds on the receiving device.

If the led count is different between an incoming savefile and the actual device led count, then *stretching* or *chopping* rules are applied.

The *stretching* rule will repeat a smaller mode across a larger number of leds.

The *chopping* rule will trim a larger mode to fit on a smaller number of leds.

