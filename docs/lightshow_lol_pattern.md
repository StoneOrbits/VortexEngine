---
layout: default
title: Pattern Controls
nav_order: 4
parent: Lightshow.lol
---
<style>
  .panel-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 16px;
    margin: 0 auto;
    margin-top: 50px;
    max-width: 80%;
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
  }

  .panel-img {
    max-width: 100%;
  }
</style>
# Pattern Controls

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-pattern.png">

The **Pattern Panel** is where you design and assign animations for your LEDs. Patterns determine how LEDs blink or animate over time.

### Key Features

- **Pattern Library:** Choose from a library of predefined patterns (e.g., strobe, dops, blend).
- **Custom Parameters:** Adjust parameters like blink size, gap size, blend speed, etc

Test test test

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


