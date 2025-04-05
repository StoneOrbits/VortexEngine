---
layout: default
title: Chromalink Duo
nav_order: 45
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

# Chromalink Duo

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-chromalink-duo.png">

The **Chromalink Panel** handles the connection with a **Duo** made via the **Chromadeck** and **Chromalink** cable.

### Key Features

- **Update Check:** Checks with the database on vortex.community for latest firmware version and informs the user if the firmware is completely up to date.
- **Flash Firmware:** Updates the firmware to the latest version.
- **Custom Firmware Flash:** Upload custom Duo Firmwares from your computer.

Once the duo is connected with **Connect Duo** the device status appears along with the Duo version and number of modes.

With a Duo connected the **Modes Panel** can be used to **pull** the modes from the Duo into the **Modes List** where they can be edited and then **pushed** back to the duo.

It is possible to adjust the total number of modes on a Duo with the editor, simply delete the extra modes and hit push.

## Other Panels

<div class="panel-grid">
  <a href="lightshow_lol_update_panel.html" class="panel-link">
    <span class="panel-title">← Update Panel 🔗</span>
    <img src="assets/images/lightshow-lol-device-update.png" class="panel-img">
  </a>
  <a href="lightshow_lol_modes.html" class="panel-link">
    <span class="panel-title">🔗 Modes Panel →</span>
    <img src="assets/images/lightshow-lol-modes.png" class="panel-img">
  </a>
</div>

