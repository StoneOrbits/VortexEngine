---
layout: default
title: Colorset Controls
nav_order: 5
parent: Lightshow.lol
---
<style>
  .panel-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 16px;
    margin: 0 auto;
    margin-top: 50px;
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
# Colorset Controls

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-colorset.png">

The **Colorset Panel** represents the list of colors for the selected leds of the current mode.

### Key Features

- **Drag and Drop** Drag to rearrange colors in your colorset.
- **Dynamic Color Picker:** Click to modify the color with the Color Picker

There is a maximum of 8 colors in a colorset, black is considered 'off' which can be used to extend gaps in the pattern.

<div class="panel-grid">
  <a href="lightshow_lol_pattern.html" class="panel-link">
    <span class="panel-title">← Pattern Panel 🔗</span>
    <img src="assets/images/lightshow-lol-pattern.png" class="panel-img">
  </a>
  <a href="lightshow_lol_color_picker.html" class="panel-link">
    <span class="panel-title">🔗 Color Picker →</span>
    <img src="assets/images/lightshow-lol-color-picker.png" class="panel-img">
  </a>
</div>


