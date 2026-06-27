---
layout: default
title: Colorset Controls
nav_order: 30
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
# Colorset Controls

<img style="float:right;max-width:40%;margin:10px;" src="{{ '/assets/images/lightshow-lol-colorset.png' | relative_url }}">

The **Colorset Panel** shows the list of up to 8 colors for the selected LEDs of the current mode.

### Key Features

- **Color Swatches:** Each color in the colorset is shown as a swatch. Click a swatch to open the **Color Picker** and modify it. The first swatch is highlighted with a green border to indicate the **main selected LED**.
- **Add Color:** The **+** button adds a new color to the colorset (up to 8), opening the Color Picker immediately.
- **Empty Slots:** Dashed slots after the **+** button indicate remaining unused positions (up to 8 total).
- **Delete Color:** Right-click a color swatch and select **Delete Color** to remove it from the set.
- **Drag and Drop:** Drag a color swatch to reorder colors within the colorset.
- **Copy / Paste:** Right-click a swatch to copy a single color, or right-click the canvas for broader copy/paste options.

### Colorset Generator

The generator (toggled via the chevron button below the swatches) lets you quickly create a new colorset:

- **Color Count:** Use the slider or number input to set how many colors to generate (1–8).
- **Style:** Choose from preset styles — **Rainbow**, **Random**, **Pastel**, **Dark**, **Grayscale**, **Vibrant**, **Warm**, or **Cool**.
- **Brightness:** Adjust overall brightness of the generated colors (10–100%).
- **Generate:** Click the orange **Generate** button to replace the current colorset.

### Tips

- There is a maximum of 8 colors in a colorset.
- Black counts as 'off' — it can extend gaps in a pattern. Changing a color to black is not the same as deleting it.
- Use the generator to quickly build a colorset, then fine-tune individual colors with the Color Picker.
- Colorsets work together with [Patterns]({{ site.baseurl }}{% link vortex-engine/patterns/index.md %}) — try different color and pattern combinations for unique effects.
- For a deeper overview of how colorsets fit into the Vortex Engine, see the [Colorsets]({{ site.baseurl }}{% link vortex-engine/colorsets/index.md %}) page.

## Other Panels

<div class="panel-grid">
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/pattern.md %}" class="panel-link">
    <span class="panel-title">← Pattern Panel 🔗</span>
    <img src="{{ '/assets/images/lightshow-lol-pattern.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/color-picker.md %}" class="panel-link">
    <span class="panel-title">🔗 Color Picker →</span>
    <img src="{{ '/assets/images/lightshow-lol-color-picker.png' | relative_url }}" class="panel-img">
  </a>
</div>


