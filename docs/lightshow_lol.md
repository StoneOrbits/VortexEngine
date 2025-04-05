---
layout: default
title: Lightshow.lol
nav_order: 3
has_children: true
---

<style>
  .panel-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
    gap: 16px;
    margin: 16px 0;
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

  .important-inline {
    display: inline-block;
    background-color: #fe00001f;
    color: #ffffff;
    border: 1px solid #ff5050;
    border-radius: 8px;
    box-shadow: 0 2px 4px rgba(0,0,0,0.6);
    padding: 12px;
    margin: 5px;
  }
  .important-inline-header {
    display: block;
    font-size: 1em;
  }
</style>

# Lightshow.lol

[Lightshow.lol](https://lightshow.lol) is an online Vortex Editor and Simulator capable of connecting with any [Vortex Device](vortex_devices.html) to preview and edit modes in real-time.

<div style="border-radius: 5px; border: 3px solid gray; text-align: center; margin: 20px">
  <img src="assets/images/lightshow-lol-preview.gif" style="box-shadow: 2px 2px 10px rgba(0, 0, 0, 0.2); border-radius: 5px;">
</div>

{: .note }
The **preview** will never look exactly like **real LEDs**.  Although this is a real-time simulation, RGB programmable LEDs produce light and colors differently from computer monitors and screens.

## Getting Started

To get started with lightshow.lol, check out these [common use-cases and examples](lightshow_lol_examples.html).

If you want to learn more about the individual controls then continue reading.

## Control Panels

Depending on whether lightshow.lol is used from **mobile** or **desktop** the layout will appear different, but functions nearly the same. 

Pick one of the options below to learn more about it:

<div class="panel-grid">
  <a href="lightshow_lol_animation.html" class="panel-link">
    <span class="panel-title">🔗 Animation Panel →</span>
    <img src="assets/images/lightshow-lol-animation.png" class="panel-img">
  </a>
    <a href="lightshow_lol_pattern.html" class="panel-link">
    <span class="panel-title">🔗 Pattern Panel →</span>
    <img src="assets/images/lightshow-lol-pattern.png" class="panel-img">
  </a>
    <a href="lightshow_lol_colorset.html" class="panel-link">
    <span class="panel-title">🔗 Colorset Panel →</span>
    <img src="assets/images/lightshow-lol-colorset.png" class="panel-img">
  </a>
    <a href="lightshow_lol_color_picker.html" class="panel-link">
    <span class="panel-title">🔗 Color Picker →</span>
    <img src="assets/images/lightshow-lol-color-picker.png" class="panel-img">
  </a>
  <a href="lightshow_lol_device_controls.html" class="panel-link">
    <span class="panel-title">🔗 Device Control →</span>
    <img src="assets/images/lightshow-lol-device.png" class="panel-img">
  </a>
  <a href="lightshow_lol_modes.html" class="panel-link">
    <span class="panel-title">🔗 Modes Panel →</span>
    <img src="assets/images/lightshow-lol-modes.png" class="panel-img">
  </a>
  <a href="lightshow_lol_led_selection.html" class="panel-link">
    <span class="panel-title">🔗 Led Selection →</span>
    <img src="assets/images/lightshow-lol-led-select.png" class="panel-img">
  </a>
  <a href="lightshow_lol_update_panel.html" class="panel-link">
    <span class="panel-title">🔗 Update Panel →</span>
    <img src="assets/images/lightshow-lol-device-update.png" class="panel-img">
  </a>
  <a href="lightshow_lol_chromalink_duo.html" class="panel-link">
    <span class="panel-title">🔗 Chromalink Panel →</span>
    <img src="assets/images/lightshow-lol-chromalink-duo-basic.png" class="panel-img">
  </a>
</div>

