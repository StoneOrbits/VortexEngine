---
layout: default
title: Led Selection
nav_order: 0
parent: Menus
grand_parent: Vortex Engine
published: true
---

# Led Selection

When you enter menus like <span style="color: #00ff00;">Color Select</span>, <span style="color: #3366ff;">Pattern Select</span>, or <span style="color: #ffffff;">Randomizer</span>, the first step is selecting which LEDs you want to change. The changes you make in these menus will only affect the LEDs that were selected when you entered the menu.

## Demonstration of LED Selection

Below is a simple demonstration of how LED selection works. The flashing squares represent the LEDs on your device. You can click the "Next LED" button to move the selection to the next LED.

<div id="led-container">
  <div class="led" id="led-1"></div>
  <div class="led" id="led-2"></div>
  <div class="led" id="led-3"></div>
  <div class="led" id="led-4"></div>
  <div class="led" id="led-5"></div>
</div>

<button id="next-led-button">Next LED</button>

<script>
  let currentLed = 0;
  const leds = document.querySelectorAll('.led');

  function updateLedSelection() {
    leds.forEach((led, index) => {
      if (index === currentLed) {
        led.style.backgroundColor = '#00ff00';
        led.style.opacity = '1';
      } else {
        led.style.backgroundColor = '#555555';
        led.style.opacity = '0.3';
      }
    });
  }

  document.getElementById('next-led-button').addEventListener('click', () => {
    currentLed = (currentLed + 1) % leds.length;
    updateLedSelection();
  });

  updateLedSelection();
</script>

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
