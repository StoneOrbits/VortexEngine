---
layout: default
title: Color Picker
nav_order: 35
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
# Color Picker

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-color-picker.png">

The **Color Picker** is a detailed interface for picking new colors for a colorset.

### Key Features

- **Precision Control:** Fine-tune colors with RGB/HEX inputs.
- **Quick Adjustments:** Easily modify brightness, saturation, and hue.
- **Real-Time Preview:** See a live preview of the color being chosen on any connected device.


The color picker is opened by selecting a color in the colorset, and it can be closed unlike most other panels. The color picker will update the color of the last color that was clicked in the colorset.

## Other Panels

<div class="panel-grid">
  <a href="lightshow_lol_colorset.html" class="panel-link">
    <span class="panel-title">← Colorset Panel 🔗</span>
    <img src="assets/images/lightshow-lol-colorset.png" class="panel-img">
  </a>
  <a href="lightshow_lol_update_panel.html" class="panel-link">
    <span class="panel-title">🔗 Update Panel →</span>
    <img src="assets/images/lightshow-lol-device-update.png" class="panel-img">
  </a>
</div>