---
layout: default
title: Pattern Select
nav_order: 5
parent: Menus
grand_parent: Vortex Engine
---

<style>
  @keyframes glow {
    0%, 100% { filter: hue-rotate(-10deg); }
    50% { filter: hue-rotate(10deg); }
  }

  @keyframes blink {
    50% { opacity: 0; }
  }

  .option-container {
    display: flex;
    align-items: center;
    margin-bottom: 5px;
    margin-left: 20px;
  }

  .glow-box {
    width: 20px;
    height: 20px;
    border-radius: 4px;
    margin-right: 8px;
    box-shadow: 0 0 20px rgba(0, 0, 0, 0.8);
    border: 1px solid #aaa;
    animation: glow 2s ease-in-out infinite;
  }

  .glow-red {
    background-color: rgba(255, 0, 0, 0.6);
    box-shadow: 0 0 20px rgba(255, 0, 0, 0.4);
  }

  .glow-green {
    background-color: rgba(0, 255, 0, 0.6);
    box-shadow: 0 0 20px rgba(0, 255, 0, 0.4);
  }

  .glow-blue {
    background-color: rgba(0, 0, 255, 0.6);
    box-shadow: 0 0 20px rgba(0, 0, 255, 0.4);
  }

  .glow-purple {
    background-color: rgba(128, 0, 128, 0.6);
    box-shadow: 0 0 20px rgba(128, 0, 128, 0.4);
  }

  .red-blink {
    background-color: rgba(255, 0, 0, 0.6);
    box-shadow: 0 0 10px rgba(255, 0, 0, 0.6);
    animation: blink 1s infinite;
  }
</style>

# Pattern Select

The Pattern Select menu lets you quickly change the blinking speed or style of your current mode, also known as the [Pattern](patterns.html).

## Selecting Patterns

Vortex Devices offer a wide range of flashing patterns. To make selecting patterns easier, the Pattern Select menu provides shortcuts to different points in the pattern list.

After choosing LEDs and entering the Pattern Select menu, you'll see five options, each represented by a glowing color:

<div class="option-container">
  <div class="glow-box glow-red"></div>
  <strong>Red</strong>: Start at the beginning of the pattern list
</div>
<div class="option-container">
  <div class="glow-box glow-green"></div>
  <strong>Green</strong>: Start 1/4 into the pattern list
</div>
<div class="option-container">
  <div class="glow-box glow-blue"></div>
  <strong>Blue</strong>: Start halfway through the pattern list
</div>
<div class="option-container">
  <div class="glow-box glow-purple"></div>
  <strong>Purple</strong>: Start 3/4 into the pattern list
</div>
<div class="option-container" style="margin-bottom:10px;">
  <div class="glow-box red-blink"></div>
  <strong>Red Blink</strong>: Exit the Pattern Select menu
</div>

To choose a new pattern: 

 1. Select a shortcut to enter the pattern list
 2. Short click to cycle through patterns
 3. Long click to save the pattern and exit

## Types of Patterns

There are two distinct **types** of patterns:

**Multi LED Patterns**: These span across all LEDs, and only one multi-LED pattern can be active at a time.

**Single LED Patterns**: These play on individual LEDs, allowing each LED to have its own unique pattern.

Note: The Vortex Duo only supports single LED patterns.