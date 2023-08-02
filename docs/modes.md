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

## Led Count

Technically, a Mode is more than just one Pattern and Colorset. Since each LED on the device can be configured independently, a Mode actually consists of a Pattern and Colorset for each LED on the device. This means that the number of LEDs (the Led Count) directly affects the complexity and variety of the Modes.

## The Mode List

The Vortex Device is centered around the 'Mode List', a list of each Mode the device has configured. Each device will have a different size Mode List. For example, the Duo has 5 modes, while other devices like the Gloves have more.

You can iterate through the Mode List by turning the device on and performing single clicks. When you reach the end of the Mode List, the device will wrap around and cycle back to the first Mode.

## Mode Slots

You can think of the Mode List as a series of 'Mode Slots'. These are spaces where Modes can be stored. You can freely change your Modes, move them around, and share them with other devices. You can also empty out your Mode Slots to reduce the number of Modes on the device, or reset Mode Slots to their default state with the factory reset.

## Opening Menus

When you open the menus on any Vortex Device, the Mode that was selected and playing when you opened the menus will be targeted for configuration. This means that any changes you make in the menus will affect the currently selected Mode.

To continue reading about the Menus, go [here](menus.html).

