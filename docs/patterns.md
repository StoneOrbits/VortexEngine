---
layout: default
title: Patterns
nav_order: 2
parent: Vortex Engine
---

# Patterns

Patterns in Vortex Engine are the ways in which LEDs are controlled to produce different effects. There are two kinds of patterns: single LED patterns and multi LED patterns.

## Single Led Patterns

A single LED pattern is a pattern which only targets a single LED for rendering. This means any kind of blink or color, as long as it's on one LED, is considered a single LED pattern. Single LED patterns are available on all devices and any Vortex device with multiple LEDs can freely program any single LED pattern into each LED independently of one another.

However, single LED patterns cannot work together or produce 'travelling' effects across the LEDs. This is because each single LED pattern operates independently and does not have information about the state of other LEDs.

## Multi Led Patterns

Multi LED patterns are special patterns which implicitly target multiple LEDs at once. Unlike single LED patterns, a multi LED pattern can control all of the LEDs at once and create effects that appear to 'travel' across the LEDs.

You can think of the difference between a multi-LED pattern and several single LED patterns as:

- A multi-LED pattern has one 'brain' controlling many LEDs at once
- Each single-LED pattern has its own 'brain' controlling only one LED

Only the larger Vortex Devices from StoneOrbits are capable of supporting multi LED patterns. The Duo cannot run multi LED patterns but it will happily convert them into single LED patterns with equivalent colorsets (unfortunately the multi LED effect is lost).

## The Pattern List



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
