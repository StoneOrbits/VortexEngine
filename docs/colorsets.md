---
layout: default
title: Colorsets
nav_order: 25
parent: Vortex Engine
---

# Colorsets

Colorsets in Vortex Engine are the lists of colors that are used in conjunction with Patterns to produce different effects on the LEDs. 


## What is a Colorset?

Colorsets are the other half of the coin that makes up a Mode (patterns being the first half).  A pattern designates how the modes blinks, and the colorset will determine the color of the blinks.

A colorset technically contains a list of colors and a "current position", then a pattern may choose to increment the "current position" whenever it pleases.

## How are Colorsets used?

Colorsets are used in combination with Patterns to create Modes. A Mode is a combination of a Pattern with a Colorset. The Pattern determines the blinking speed or style of the LEDs, and the Colorset determines which colors are displayed on each blink. 

This means that by changing the Colorset, you can dramatically change the appearance of a Mode, even if the Pattern remains the same.

Some multi-led patterns are creative with how the colorset is applied, for example some patterns will pull the first color out of the colorset for some special purpose like a single led playing a special pattern and then use the rest of the colors for the pattern it is displaying. 

The key point to understand is it's up to the pattern to decide how to use the colorset, but is up to the user to choose which colorset to go with which pattern.

## How many colors can a Colorset have?

Although the maximum number of colors that can be in a colorset is 8, the actual number of colors in a Colorset will vary depending on the user preference. Some modes and patterns may look attractive with few colors in the colorset, and some patterns may appear better with many colors.

It's up to the user to mix and match and decide which combination they prefer.

## How do I create a Colorset?

Creating a Colorset involves selecting the colors you want to include and arranging them in the order you want them to be displayed. The specific process for creating a Colorset can vary depending on the specific Vortex Device and software you are using.

In all Vortex Devices you can adjust the colorset by entering the [Color Selection Menu](color_select_menu.html)

