---
layout: default
title: Modes List
nav_order: 15
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
# Modes List

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-modes.png">

The **Modes Panel** organizes and manages your device's list of modes. A [mode](modes.html) is a collection of patterns and colorsets (one for each LED). You can retrieve a copy of your device's modes, add new ones, delete, or modify modes, and save the modes to your device.

### Key Features

- **Mode Creation:** Add, edit, or delete modes with ease.
- **Mode Preview:** Switch between modes to see their effects in real-time.
- **Pull/Push:** Pull existing modes from your device or push save new modes to your device.
- **Sharing:** Quickly share your mode with the Vortex Community.

## Other Panels

<div class="panel-grid">
  <a href="lightshow_lol_device_controls.html" class="panel-link">
    <span class="panel-title">← Device Control 🔗</span>
    <img src="assets/images/lightshow-lol-device.png" class="panel-img">
  </a>
  <a href="lightshow_lol_led_selection.html" class="panel-link">
    <span class="panel-title">🔗 Led Selection →</span>
    <img src="assets/images/lightshow-lol-led-select.png" class="panel-img">
  </a>
</div>

