---
layout: default
title: Color Picker
nav_order: 6
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
# Color Picker

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-color-picker.png">

The **Color Picker** is a detailed interface for picking new colors for a colorset.

### Key Features

- **Precision Control:** Fine-tune colors with RGB/HEX inputs.
- **Quick Adjustments:** Easily modify brightness, saturation, and hue.

The color picker is opened by selecting a color in the colorset, and it can be closed unlike most other panels. The color picker will update the color of the last color that was clicked in the colorset.

When a device is connected it will display a live preview of the color being chosen as the user navigates the color picker.

<div class="panel-grid">
  <a href="lightshow_lol_colorset.html" class="panel-link">
    <span class="panel-title">← Colorset Panel 🔗</span>
    <img src="assets/images/lightshow-lol-colorset.png" class="panel-img">
  </a>
  <a href="lightshow_lol_animation.html" class="panel-link">
    <span class="panel-title">🔗 Animation Panel →</span>
    <img src="assets/images/lightshow-lol-animation.png" class="panel-img">
  </a>
</div>