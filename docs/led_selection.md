---
layout: default
title: Led Selection
nav_order: 0
parent: Menus
grand_parent: Vortex Engine
published: true
---

<style>
  .led {
    width: 20em;
    max-width: 60px;
    aspect-ratio: 1;
    background-color: #9600ff;
    transition: background-color 0.3s, opacity 0.3s;
    box-shadow: 0 0 10px rgba(150, 0, 255, 0.8);
    border-radius: 5px;
    margin: 1em;
  }
  .led-blink {
    background-color: #9600ff;
    box-shadow: 0 0 20px rgba(150, 0, 255, 0.8);
    animation: blink 1s infinite;
  }
  @keyframes blink {
    50% { opacity: 0; }
  }
  #led-container {
    display: flex;
    gap: 1em;
    justify-content: center;
  }
  #led-selection-container {
    display: flex;
    flex-direction: column;
    gap: 1em;
    justify-content: center;
  }
  #status-text {
    color: white;
    font-size: 16px;
    margin: 0 auto;
  }
  #next-led-button {
    width: 6em;
    margin: 0 auto;
    font-size: 16px;
    padding: 5px;
  }
</style>

# Led Selection

When entering a menu like <span style="color: #00ff00;">Color Select</span>, <span style="color: #3366ff;">Pattern Select</span>, or <span style="color: #ffffff;">Randomizer</span>, the first step will always be _led selection_.

When you enter the _led selection step_ of a menu the LEDs will begin blinking <span style="color: #9600ff;">purple</span>. 

This purple color indicates which leds will be affected by the menu, the first option is the **ALL LEDs** option -- ergo all the leds will blink purple. 

## Demonstration of LED Selection

Below is a simple demonstration of how LED selection works. The squares represent the LEDs on your device. 

Press the _Next LED_ button to cycle through the different led selection options.

<div id="led-selection-container">
  <div id="led-container">
    <div class="led" id="led-1"></div>
    <div class="led" id="led-2"></div>
    <div class="led" id="led-3"></div>
    <div class="led" id="led-4"></div>
    <div class="led" id="led-5"></div>
  </div>
  <div id="status-text"><strong>Selection:</strong> All LEDs</div>
  <button id="next-led-button">Next LED</button>
</div>

<script src="{{ '/assets/js/LedSelect.js' | relative_url }}"></script>

## Led Selection Options

Because each device has a different number of LEDs often in a different layout, there may be unique led selection options offered by some devices but not others.

However, similar to the above demonstration, all devices provide at least the following options:

 **ALL LEDs:** Change all LEDs at once

 **Mutli-LED:** Change all LEDs to a multi-led pattern

 **Even LEDs:** For example every even led 0, 2, 4

 **Odd LED:** The inverse of even, 1, 3, 5, etc

 **Single LED:** Select any single led to change

The Multi-LED option is unique in that it only appears in the <span style="color: #3366ff;">Pattern Select</span> menu.
