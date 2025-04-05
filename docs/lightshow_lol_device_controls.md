---
layout: default
title: Device Controls
nav_order: 10
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
# Device Controls

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-device.png">

The **Device Controls Panel** is very simple, offering only two purposes: to **select** or **connect** a **device**.

- **Select** a device when you want to make a mode for that type of device without needing to connect it.
- **Connecting** a device will automatically select the correct type from the device list.

In both cases, whether **Selecting** or **Connecting** a device, the **LED Selection Panel** will appear.

## Other Panels

<div class="panel-grid">
  <a href="lightshow_lol_modes.html" class="panel-link">
    <span class="panel-title">🔗 Modes Panel →</span>
    <img src="assets/images/lightshow-lol-modes.png" class="panel-img">
  </a>
    <a href="lightshow_lol_led_selection.html" class="panel-link">
    <span class="panel-title">← LED Selection Panel 🔗</span>
    <img src="assets/images/lightshow-lol-led-select.png" class="panel-img">
  </a>
</div>
