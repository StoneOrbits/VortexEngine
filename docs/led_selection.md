---
layout: default
title: Led Selection
nav_order: 0
parent: Menus
grand_parent: Vortex Engine
published: true
---

# Led Selection

When you enter menus like <span style="color: #00ff00;">Color Select</span>, <span style="color: #3366ff;">Pattern Select</span>, or <span style="color: #ffffff;">Randomizer</span>, all of the LEDs will change to <span style="color: #ff00ff;">magenta</span>. 

This is the **ALL LEDs** option. Cycle to see different options and select which LEDs you want to change. The changes you make in these menus will only affect the LEDs that were selected when you entered the menu.

## Demonstration of LED Selection

Below is a simple demonstration of how LED selection works. The squares represent the LEDs on your device. You can click the "Next LED" button to move the selection to the next LED.

<div id="led-container">
  <div class="led" id="led-1"></div>
  <div class="led" id="led-2"></div>
  <div class="led" id="led-3"></div>
  <div class="led" id="led-4"></div>
  <div class="led" id="led-5"></div>
</div>

<div id="status-text" style="margin-top: 20px; font-size: 16px; color: #ffffff;"></div>

<button id="next-led-button">Next LED</button>

<script src="{{ '/assets/js/LedSelect.js' | relative_url }}"></script>


<style>
  #led-container {
    display: flex;
    gap: 10px;
    margin-bottom: 20px;
  }
  .led {
    width: 50px;
    height: 50px;
    background-color: #555555;
    opacity: 0.3;
    transition: background-color 0.3s, opacity 0.3s;
  }
</style>

## Led Selection Options

Because each device has a different number of LEDs with a different LED layout, Each device has an unique set of options in LED selection. Here are a few of the available options:

**-ALL LEDs:** Selects all LEDs at once for use with all patterns.

**-MutliLED:** Selects all LEDs at once for use with MultiLED patterns.

**-LED Group:** These groups vary the most from device to device. They allow targeting premade groups of LEDs such as Odd LEDs or Even LEDs.

**-Single LED:** This allows the targeting of a single LED and can be used to customize each LED independantly.