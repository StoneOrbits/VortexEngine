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
  <img src="{{ '/assets/images/lightshow-lol-animation.png' | relative_url }}" alt="Lightshow.lol Animation">
</div>

The **Animation Panel** controls the appearance of the **preview** — the spinning trail of colors shown in the editor.

Settings on the **Animation Panel** do not affect the device output. They only change how the preview appears on your screen.

### Animation Shapes

Five buttons at the top let you choose the path the animation follows:

| Button | Shape |
|--------|-------|
| Circle | Circular orbit |
| Infinity | Figure-8 path |
| Heart | Heart-shaped path |
| Box | Rectangular path |
| Orbit | LEDs orbit around a center point |

### Slider Controls

Six sliders tune different qualities of the preview trail:

- **Speed**: How fast the animation progresses
- **Trail**: How long the colors remain visible (the length of the trail)
- **Size**: The size of each blink of color in the trail
- **Blur**: Softness/diffusion of each blink of color
- **Radius**: The radius of the animation path on screen
- **Spread**: With many LEDs, the distance between each LED in the preview

### Device Presets

When a device is connected, the animation panel automatically applies a preset tuned to that device (orbit, handle, gloves, etc.), including a **Spread** slider that is hidden when no device is selected.

## Other Panels

<div class="panel-grid">
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/chromalink-duo.md %}" class="panel-link">
    <span class="panel-title">← Chromalink Panel 🔗</span>
    <img src="{{ '/assets/images/lightshow-lol-chromalink-duo-basic.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/device-controls.md %}" class="panel-link">
    <span class="panel-title">🔗 Device Control →</span>
    <img src="{{ '/assets/images/lightshow-lol-device.png' | relative_url }}" class="panel-img" alt="Device Control">
  </a>
</div>
