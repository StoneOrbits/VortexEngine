---
layout: default
title: Preview Animation
nav_order: 0
parent: Lightshow.lol
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

The **Preview** is the spinning circle of colors immediately visible when entering the page, and the **Animation Panel** specifically controls the appearance of this **Preview**.

While the **Preview** will react to changes made in various UI panels (like Pattern or Colorset), the **Animation Panel** only affects the on-screen preview, not how the actual device looks.

The five animation shape options are self-explanatory, but the six slider options in this panel are:

- **Speed**: How fast the animation progresses.
- **Trail**: How long the colors remain visible (the length of the trail).
- **Size**: The size of each blink of color.
- **Blur**: Softness/diffusion of the blink on screen.
- **Radius**: The size of the animation pattern on screen.
- **Spread**: With many LEDs, the distance between LEDs in the preview.

<div class="panel-grid-title">
  <p> </p>
</div>
<div class="panel-grid">
  <a href="lightshow_lol_chromalink_duo.html" class="panel-link">
    <span class="panel-title">← Chromalink Panel 🔗</span>
    <img src="assets/images/lightshow-lol-chromalink-duo.png" class="panel-img">
  </a>
  <a href="lightshow_lol_device_controls.html" class="panel-link">
    <span class="panel-title">🔗 Device Control →</span>
    <img src="assets/images/lightshow-lol-device.png" class="panel-img" alt="Device Control">
  </a>
</div>
