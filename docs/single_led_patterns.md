---
layout: default
title: Single-Led Patterns
nav_order: 0
parent: Patterns
grand_parent: Vortex Engine
---

# Single-Led Patterns

A single-led pattern is displayed entirely on **one LED**. This means each LED may use a different pattern at the same time.

Unlike multi-led patterns where each pattern is a distinct type, there are **two types** of single led patterns:

 - **Strobe Patterns** will blink or strobe from color to color in distinct flashes
 - **Blend Patterns** will smoothly blend from color to color (while strobing)


## Strobe Patterns

Strobe patterns are the most basic kind of pattern, they simply blink from color to color in the colorset.  A strobe pattern has the following parameters:

 - **on duration**: how long the led stays on each 'blink'
 - **off duration**: the amount of time the led stays off between each 'blink'
 - **gap duration**: the length of the pause after a complete cycle of colors
 - **dash duration**: how large the extra burst of light is after the gap
 - **grouping size**: how many blinks appear in a 'group' before the gap

## Blend Patterns
Blend patterns are nearly identical to strobe patterns but instead of blinking from color to color, they blend between colors in the colorset. They have the same parameters as strobe, plus two extra:

 - **blend speed**: the speed at which the blend will progress
 - **flip count**: the number of times the blend 'flips' to complementary colors

## The Pattern List

Below is the list of single-led patterns

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
```