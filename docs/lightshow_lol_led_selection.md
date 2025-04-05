---
layout: default
title: Led Selection
nav_order: 20
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
# LED Selection Panel

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-led-select.png">

The **LED Selection Panel** allows you to select and modify individual LEDs settings for each mode on your device.

### Key Features

- **Visual Interface:** Displays a layout of all LEDs on the selected device.
- **Individual Customization:** Select individual LEDs to assign patterns and colors.
- **Batch Selection:** Select multiple LEDs for simultaneous modifications.

The LED Selection panel only appears after a device has been selected or connected from the [Device Control Panel](lightshow_lol_device_controls.html).

Led selections can be added or removed by holding shift or ctrl respectively, and you can group select by clicking and dragging your cursor to draw selection box over the image.

## Other Panels

<div class="panel-grid">
  <a href="lightshow_lol_modes.html" class="panel-link">
    <span class="panel-title">← Modes Panel 🔗</span>
    <img src="assets/images/lightshow-lol-modes.png" class="panel-img">
  </a>
  <a href="lightshow_lol_pattern.html" class="panel-link">
    <span class="panel-title">🔗 Pattern Panel →</span>
    <img src="assets/images/lightshow-lol-pattern.png" class="panel-img">
  </a>
</div>

