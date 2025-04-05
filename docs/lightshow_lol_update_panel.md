---
layout: default
title: Device Updates
nav_order: 40
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
# Device Updates

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-device-update.png">

The **Device Update Panel** is responsible for checking and delivering updates for the Spark Orbit, Spark Handles, and the Chromadeck.

### Key Features

- **Realtime Update Check:** Checks with the database on vortex.community for latest firmware version
- **Instant Firmware Flash:** Immediately update the firmware to the latest with the click of a button
- **Up-to-date Notification:** This window will inform if the firmware is completely up to date

The other devices like Vortex Orbit, Handles and Wired Gloves are not supported by this update panel.

## Other Panels

<div class="panel-grid">
  <a href="lightshow_lol_color_picker.html" class="panel-link">
    <span class="panel-title">← Color Picker 🔗</span>
    <img src="assets/images/lightshow-lol-color-picker.png" class="panel-img" alt="Color Picker">
  </a>
  <a href="lightshow_lol_chromalink_duo.html" class="panel-link">
    <span class="panel-title">🔗 Chromalink Panel →</span>
    <img src="assets/images/lightshow-lol-chromalink-duo-basic.png" class="panel-img">
  </a>
</div>

