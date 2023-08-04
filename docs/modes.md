---
layout: default
title: Modes
nav_order: 1
parent: Vortex Engine
---

# Modes

Vortex revolves around a list of modes, each device has a different size list of modes and the entire purpose of a Vortex Device is to render a 'mode'.

## What is a Mode?

To put it discretely, a Mode is a combination of a Pattern with a Colorset. Think of a Pattern as the rhythm or tempo of a song, and the Colorset as the notes. The Pattern determines the blinking speed or style of the LEDs, and the Colorset determines which colors are displayed on each blink. You can read more about patterns [here](patterns.html) and colorsets [here](colorsets.html).

So for example, if you have a basic strobe pattern paired with the colorset (red, green, blue) then you could say that mode is an "rgb strobe".

## Led Count

Technically, a Mode is more than just one Pattern and Colorset. Since each LED on the device can be configured independently, a Mode actually consists of a Pattern and Colorset for each LED on the device. This means that the number of LEDs (the Led Count) directly affects the complexity of the Mode.

A device with many leds like the Vortex Orbit (28 leds) can configure a very complex mode with different patterns and colorsets on each of the 28 leds, where as a small device like the duo can only create modes with two leds.

Despite the different in led count these modes are still compatible with one another, the 2 led mode could be loaded onto the orbit and the 28 led mode could be loaded onto the duo.

## The Mode List

The Vortex Device is centered around the 'Mode List', a list of each Mode the device has configured. Each device will have a different size Mode List. For example, the Duo has 5 modes, while other devices like the Gloves have more.

You can iterate through the Mode List by turning the device on and performing single clicks. When you reach the end of the Mode List, the device will wrap around and cycle back to the first Mode.

## Mode Slots

You can think of the Mode List as a series of 'Mode Slots'. These are spaces where Modes can be stored. You can freely change your Modes, move them around, and share them with other devices. You can also empty out your Mode Slots to reduce the number of Modes on the device, or reset Mode Slots to their default state with the factory reset.

## Opening Menus

When you open the menus on any Vortex Device, the Mode that was selected and playing when you opened the menus will be targeted for configuration. This means that any changes you make in the menus will affect the currently selected Mode.

To continue reading about the Menus, go [here](menus.html).

## Empty Modes

A lesser known feature of the Vortex Engine is it will automatically skip over empty modes, this means if your device has for example 5 mode slots and you clear out one of the modes so it is "empty" then your device will only have 4 modes and the 5th slot will be effectively gone.

The only way to do this is to use the editor and set "pattern none" on all leds, this will make a mode "empty" and any attempt to iterate to the mode will skip over it to the next mode.

As a precaution the first mode on the device will not be skipped if it is empty, this means if it is emptied out it will simply be a blank mode giving the effect the device is off when it really is not.

It is advised to avoid emptying your modes out unless you are absolutely sure that is what you want to do because the only way to restore your mode afterward is with a factory reset.

