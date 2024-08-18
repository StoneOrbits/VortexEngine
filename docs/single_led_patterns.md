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


## Basic Patterns

Basic patterns cover all possible patterns which blink on and off. They have the following parameters:
 - on duration: how long the led blinks on for
 - off duration: how long the led is off each blink
 - gap duration: how large of a gap between each volley of colors
 - dash duration: how large of a dash/line embedded in the gap
 - grouping size: how many colors appear in a group before the gap

## Blend Patterns
Blend patterns are similar to basic patterns but blend between colors in the colorset instead only using the selected colors. They have the following paramters:
 - on duration: how long the led blinks on for
 - off duration: how long the led is off each blink
 - gap duration: how large of a gap between each volley of colors
 - dash duration: how large of a dash/line embedded in the gap
 - grouping size: how many colors appear in a group before the gap
 - blend speed: the speed at which the blend will progress
 - flip count: the number of times the blend will flip to complementary colors


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