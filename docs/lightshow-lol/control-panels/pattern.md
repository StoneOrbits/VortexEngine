---
layout: default
title: Pattern Controls
nav_order: 25
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
# Pattern Controls

<img style="float:right;max-width:40%;margin:10px;" src="{{ '/assets/images/lightshow-lol-pattern.png' | relative_url }}">

The **Pattern Panel** controls how each LED blinks or animates over time. Every mode has a pattern assigned to each of its selected LEDs.

### Pattern Selection

- **Dropdown:** Choose from **Strobe**, **Blend**, **Solid**, and **Multi-LED** patterns. Each category produces a different type of animation.
- **Randomize:** Click the dice button to instantly apply a random pattern to the selected LED.

### Pattern Parameters

Click the chevron to expand the parameter sliders. Each pattern exposes up to 7 adjustable controls (e.g. blink size, gap size, blend speed). Drag a slider or type a value to fine-tune the effect. Disabled sliders mean that pattern doesn't use that parameter.

Every pattern in the dropdown can be recreated by adjusting these sliders — they are the raw controls that define each pattern's behavior.

> For a complete reference of all available patterns and what each parameter does, see the [Patterns]({{ site.baseurl }}{% link vortex-engine/patterns/index.md %}) page.
>
> Patterns determine an LED's blink animation, but the actual colors used come from the **Colorset** assigned to that LED. See the [Colorsets]({{ site.baseurl }}{% link vortex-engine/colorsets/index.md %}) page for details on how colors and patterns work together.
{: .note }

## Other Panels

<div class="panel-grid">
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/led-selection.md %}" class="panel-link">
    <span class="panel-title">← Led Selection 🔗</span>
    <img src="{{ '/assets/images/lightshow-lol-led-select.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/colorset.md %}" class="panel-link">
    <span class="panel-title">🔗 Colorset Panel →</span>
    <img src="{{ '/assets/images/lightshow-lol-colorset.png' | relative_url }}" class="panel-img">
  </a>
</div>


