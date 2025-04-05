---
layout: default
title: Pattern Controls
nav_order: 25
parent: Control Panels
---
<style>
  .panel-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 16px;
    margin: 0 auto;
    max-width: 80%;
    margin-top: 10px;
  }

  .panel-link {
    background-color: #2e2e2e;
    border-radius: 8px;
    text-decoration: none;
    color: #ffffff;
    padding: 16px;
    display: flex;
    flex-direction: column;
    align-items: center;
    transition: transform 0.2s;
    border: 1px solid #080808;
  }

  .panel-link:hover {
    transform: scale(1.02);
  }

  .panel-title {
    margin-bottom: 8px;
    font-weight: bold;
    color: #ffcc00;
  }

  .panel-img {
    max-width: 100%;
  }
</style>
# Pattern Controls

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-pattern.png">

The **Pattern Panel** is where you choose and modify patterns for your LEDs. Patterns determine how LEDs blink or animate over time.

### Key Features

- **Pattern Library:** Choose from a library of predefined patterns (e.g., strobe, dops, blend).
- **Custom Parameters:** Adjust parameters like blink size, gap size, blend speed, etc

The various parameters control the appearance of the blinking of the LED, like the timing and length of blinks and other sometimes abstract qualities. Try making changes to come up with unique effects!

## Other Panels

<div class="panel-grid">
  <a href="lightshow_lol_led_selection.html" class="panel-link">
    <span class="panel-title">← Led Selection 🔗</span>
    <img src="assets/images/lightshow-lol-led-select.png" class="panel-img">
  </a>
  <a href="lightshow_lol_colorset.html" class="panel-link">
    <span class="panel-title">🔗 Colorset Panel →</span>
    <img src="assets/images/lightshow-lol-colorset.png" class="panel-img">
  </a>
</div>


