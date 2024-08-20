---
layout: default
title: Randomizer
nav_order: 1
parent: Menus
grand_parent: Vortex Engine
---

# Randomizer Menu

The Randomizer menu lets you generate exciting new modes on the fly, perfect for experimenting with different patterns and colors.

## How It Works

The Randomizer uses intelligent algorithms based on color theory to create visually appealing and unique modes. This allows creation of coherent colorsets with an added element of randomization.

## Random Seed
The Randomizer chooses a starting point based on the mode you start from. This means that starting with the same mode will produce the same sequence of random results.

For those familiar, the randomizer seed is a checksum of the current mode.

## Selecting LEDs

Upon entering the randomizer the LEDs will show magenta for [LED selection](led_selection.html). 

- When seleting ALL LEDs, All LEDs will be randomized. Only the LEDs that matched before randomizatioin will get the same random results. If they did not match before randomization they will randomize differently from each other.

- When selecting MultiLED, the randomizer will give either a MultiLED result, or two groups of Single LED results (i.e. even LEDs get result 1, and odd LEDs get result 2).

- When selecting any other option, only that selection will be randomized.

## Using the Randomizer

1. **Pick a Mode to Randomize**: Navigate to the mode you want to randomize and hold the button until the leds flash white
2. **Enter the Randomizer**: Long click to enter the white menu
3. **Pick the Leds to Randomize**: Short click to cycle through led groups, long click to select a group
4. **Generate a New Mode**: Short click to randomize as many times as you like
5. **Save and Exit**: Long click to save the mode and exit.

## Auto Randomization

While cycling through random modes in the randomizer menu, perform three consecutive short clicks to enable _Auto Cycle_ and automatically generate a new mode every few seconds. Short click to stop cycling.

## Advanced Randomization

See [Advanced Menus](advanced_menus.html) to enable this feature.

With the _advanced randomizer_, after selecting the led group to randomize, you may choose:

 - **Rainbow Ribbon**: Randomize Colorset only  
 - **White Strobe**: Randomize Pattern only  
 - **Rainbow Strobe**: Randomize both pattern & colorset  

Other than this extra option the _advanced randomizer_ will behave the exact same as the basic randomizer.

