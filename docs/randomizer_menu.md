---
layout: default
title: Randomizer
nav_order: 1
parent: Menus
grand_parent: Vortex Engine
---

<style>
/* Styling for section headers and dividers */
.section-header {
  font-size: 1.5em;
  margin-bottom: 10px;
}

hr {
  border: none;
  height: 1px;
  background-color: #eee;
  margin: 20px 0;
}
</style>

# Randomizer Menu

The Randomizer menu lets you generate exciting new modes on the fly, perfect for experimenting with different patterns and colors.

## How It Works

The Randomizer uses intelligent algorithms based on color theory to create visually appealing and unique modes. This allows creation of coherent colorsets with an added element of randomization.

## Random Seed
The Randomizer chooses a starting point based on the mode you start from. This means that starting with the same mode will produce the same sequence of random results.

For those familiar, the randomizer seed is a checksum of the current mode.

## Using the Randomizer

1. **Pick a Mode to Randomize**: Navigate to the mode you want to randomize and hold the button until the leds flash white
2. **Enter the Randomizer**: Long click to enter the white menu
3. **Pick the Leds to Randomize**: Short click to cycle through led groups, long click to select a group
4. **Generate a New Mode**: Short click to randomize as many times as you like
5. **Save and Exit**: Long click to save the mode and exit.

## Advanced Randomization

The randomizer offers an _advanced_ mode, see [Advanced Menus](advanced_menus.html) to enable this feature.

With the advanced randomizer, after picking which leds to randomize, you may choose to randomize specificly:

 - **Rainbow Ribbon**: Colorset only  
 - **White Strobe**: Pattern only  
 - **Rainbow Strobe**: Both pattern and colorset  

After selecting this, the randomizer will behave the exact same except for which part it randomizes.

## Auto Randomization

Activate Auto Cycle mode with three consecutive short clicks to automatically generate new modes every few seconds. Click to turn off.
