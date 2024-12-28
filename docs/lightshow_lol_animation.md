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
  }

  .panel-img {
    max-width: 100%;
  }
</style>

# Preview Animation

<div style="float: right; margin: 20px; max-width: 40%">
  <img src="assets/images/lightshow-lol-animation.png">
</div>

The **Preview** is the spinning circle of colors immediately visible when entering the page, the **Animation Panel** specifically controls the appearance of this **Preview**.

The **Preview** will react to changes made in various UI panels (like Pattern or Colorset) but the **Animation Panel** is slightly different. The **Animation Panel** only affects the **Preview** on screen, not how the device looks.

The five animation shape options are self-explanatory, but the six slider options in this panel are:

- **Speed**: This represents how fast the animation will progress.
- **Trail**: This corresponds to how long the colors will remain visible (i.e., the length of the trail).
- **Size**: This is the size of each blink of color on the screen.
- **Blur**: This controls the appearance of the blink on the screen, increasing softness and diffusion.
- **Radius**: This controls the size of the animation pattern on the screen.
- **Spread**: With many LEDs, this controls the distance between LEDs in the preview.



<div class="panel-grid-title">
  <p>  </p>
</div>
<div class="panel-grid">
  <a href="lightshow_lol_color_picker.html" class="panel-link">
    <span class="panel-title">← Color Picker 🔗</span>
    <img src="assets/images/lightshow-lol-color-picker.png" class="panel-img">
  </a>
  <a href="lightshow_lol_device_controls.html" class="panel-link">
    <span class="panel-title">🔗 Device Control →</span>
    <img src="assets/images/lightshow-lol-device.png" class="panel-img">
  </a>
</div>