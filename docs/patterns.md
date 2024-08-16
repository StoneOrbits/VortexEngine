---
layout: default
title: Patterns
nav_order: 20
parent: Vortex Engine
---

# Patterns

Patterns control the ways in which LEDs are blinked on and off for specific durations to produce different visual effects. They also determine how a [colorset](colorsets.html) is utilized, usually by showing each color in the order it appears in the set.

# Pattern Parameters

All patterns have variables called parameters that can be modified to create countless original patterns.
The best way to understand how these paramters work is by playing with them in real-time on [Lightshow.lol](lightshow.lol).

# Types of Patterns

## Multi Led Patterns

Multi LED patterns are displayed across all of the LEDs on a device. They create coordinated effects that appear to 'travel' across multiple LEDs and cannot be displayed on a single LED by itself.

Mutli LED pattern parameters vary widely for each pattern. Try experimenting with them to find new possibilities.

## Single Led Patterns

A Single LED pattern is a pattern which is displayed entirely on a single LED. This means it is possible to display a different single LED pattern on each LED on the device at the same time, but it is also common practice for a [mode](modes.html) to feature the same pattern and colorset on all LEDs. 

There are three varieties of single led patterns:

### Basic Patterns

Basic patterns cover all possible patterns which blink on and off. They have the following parameters:
 - on duration: how long the led blinks on for
 - off duration: how long the led is off each blink
 - gap duration: how large of a gap between each volley of colors
 - dash duration: how large of a dash/line embedded in the gap
 - grouping size: how many colors appear in a group before the gap

### Blend Pattern
Blend patterns are similar to basic patterns but blend between colors in the colorset instead only using the selected colors. They have the following paramters:
 - on duration: how long the led blinks on for
 - off duration: how long the led is off each blink
 - gap duration: how large of a gap between each volley of colors
 - dash duration: how large of a dash/line embedded in the gap
 - grouping size: how many colors appear in a group before the gap
 - blend speed: the speed at which the blend will progress
 - flip count: the number of times the blend will flip to complementary colors

### Solid Pattern
This is a pattern that will display only one color in the colorset without blinking. This is primarily used for special cases in the Vortex Engine.
 - used to produce effects like keychain mode
 - includes all the same parameters as Basic Pattern
 - color index: the index of the color to display

## The Pattern List

Below is the list of patterns

```
  strobe
  hyperstrobe
  dops
  strobie
  dopy
  ultradops
  strobegap
  hypergap
  dopgap
  strobiegap
  dopygap
  ultragap
  blinkie
  ghostcrush
  doubledops
  chopper
  dashgap
  dashdops
  dashcrush
  ultradash
  gapcycle
  dashcycle
  tracer
  ribbon
  miniribbon
  blend
  blendstrobe
  blendstrobegap
  complementary blend
  complementary blendstrobe
  complementary blendstrobegap
  solid
  hueshift
  theater chase
  chaser
  zigzag
  zipfade
  drip
  dripmorph
  crossdops
  doublestrobe
  meteor
  sparkletrace
  vortexwipe
  warp
  warpworm
  snowball
  lighthouse
  pulsish
  fill
  bounce
  splitstrobie
  backstrobe
  materia
```
