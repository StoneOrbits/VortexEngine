---
layout: default
title: Animation
nav_order: 5
parent: Control Panels
---

<style>
  .panel-grid-title {
    margin: 0 auto;
    text-align: center;
  }

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
    border-radius: 4px;
  }

  .preview-img-container {
    float: right;
    margin: 20px;
    max-width: 40%;
    border: 2px solid #444;
    border-radius: 8px;
  }

  .preview-img-container img {
    display: block;
    width: 100%;
    height: auto;
    border-radius: 6px;
  }

  ul {
    margin-left: 20px;
    margin-top: 10px;
  }
</style>

# Preview Animation

<div class="preview-img-container">
  <img src="assets/images/lightshow-lol-animation.png" alt="Lightshow.lol Animation">
</div>

The **Animation Panel** specifically controls the appearance of this **preview** (the spinning circle of colors).

The settings on the **Animation Panel** do not affect how the mode appears on a device, these controls  only affect how the **preview** appears on your screen.

The five animation shapes change the shape of the trail's path in the preview.

The six slider options affect other qualities of the trails:

- **Speed**: How fast the animation progresses
- **Trail**: How long the colors remain visible (the length of the trail)
- **Size**: The size of each blink of color in the trail
- **Blur**: Softness/diffusion of each blink of color
- **Radius**: The size of the animation pattern on screen
- **Spread**: With many LEDs, the distance between each LED in the preview

## Other Panels

<div class="panel-grid">
  <a href="lightshow_lol_chromalink_duo.html" class="panel-link">
    <span class="panel-title">← Chromalink Panel 🔗</span>
    <img src="assets/images/lightshow-lol-chromalink-duo-basic.png" class="panel-img">
  </a>
  <a href="lightshow_lol_device_controls.html" class="panel-link">
    <span class="panel-title">🔗 Device Control →</span>
    <img src="assets/images/lightshow-lol-device.png" class="panel-img" alt="Device Control">
  </a>
</div>
