---
layout: default
title: Patterns
nav_order: 3
parent: Vortex Engine
---

# Patterns

Patterns in Vortex Engine are the ways in which LEDs are blinked to produce different effects. You can think of patterns and colorsets as the two sides of the coin that make up a mode.

A pattern designates the blinking speed and style of the mode, specifically it designates the on and off duration of the led each blink.  

An important distinction is there are two kinds of patterns: single LED patterns and multi LED patterns.

## Single Led Patterns

A single LED pattern is a pattern which only targets a single LED for rendering. This means any kind of blink, as long as it's on one LED, is considered a single LED pattern. Single LED patterns are available on all devices and any Vortex device with multiple LEDs can freely program any single LED pattern into each LED independently of one another.

However, single LED patterns cannot work together or produce 'travelling' effects across the LEDs. This is because each single LED pattern operates independently and does not have information about the state of other LEDs.

There are three kinds of single led patterns that can be utilized in Vortex Engine:

### Basic Pattern
The most basic single led pattern that the others derive from.

This pattern is the basis of every other basic pattern and it provides the core logic for on/off/gap/dash/group.

Those 5 inputs can be used to produce an exponentially large amount of different patterns and blinks.

includes parameters: 
 - on duration: how long the led blinks on for
 - off duration: how long the led is off each blink
 - gap duration: how large of a gap between each volley of colors
 - dash duration: how large of a dash/line embedded in the gap
 - grouping size: how many colors appear in a group before the gap

### Blend Pattern
A pattern that will blend between your colors instead of blinking them

 - includes all the same parameters as Basic Pattern
 - exposes 2 additional parameters:
    - blend speed: the speed at which the blend will progress
    - flip count: the number of times the blend will flip to complementary colors

### Solid Pattern
A pattern that will display only one color in the colorset solid

 - generally not used directly, used to produce effects like keychain mode
 - includes all the same parameters as Basic Pattern
 - exposes one extra parameter:
    - color index: the index of the color to display

## Multi Led Patterns

Multi LED patterns are special patterns which implicitly target multiple LEDs at once. Unlike single LED patterns, a multi LED pattern can control all of the LEDs at once and create effects that appear to 'travel' across the LEDs.

You can think of the difference between a multi-LED pattern and several single LED patterns as:

- A multi-LED pattern has one 'brain' controlling many LEDs at once
- Each single-LED pattern has its own 'brain' controlling only one LED

Only the larger Vortex Devices from StoneOrbits are capable of supporting multi LED patterns. The Duo cannot run multi LED patterns but it will happily convert them into single LED patterns with equivalent colorsets (unfortunately the multi LED effect is lost).

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
