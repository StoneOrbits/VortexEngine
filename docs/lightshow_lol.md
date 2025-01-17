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


<!-- <div style="text-align: center; margin: 20px">
  <h1 style="color:orange;">⚠ <b style="color:yellow;">This page is under construction</b> ⚠<br></h1>
</div> -->

# Lightshow.lol

This page details how to use the web-based Vortex Editor [lightshow.lol](https://lightshow.lol). 

## Basics

Lightshow.lol is an online editor designed to both dispaly previews of the **Patterns**, **Colorsets**, and **Modes** of a **Vortex Device**, and edit them in real-time. It can also export [modes](modes.html) to any Vortex devices or upload them directly to [Vortex.Community](vortex_community.html) for sharing. 

{: .note }
Although the preview is an exact simulation of a real device, due to the difference in the way color is produced by LEDs and displays **the preview will never look exactly the same as real LEDs**

## How to Guides

Ligthshow.lol is intuitive, feature-packed, and incredibly powerful once you understand it's panels. To get started, checkout these [helpful examples](lightshow_lol_examples.html) that show how to use different features. 

## Lightshow.lol 

In the middle of the screen there is an animated trail of colorful dots. These dots serve as your preview of the mode you're currently building. 

Each of the surrounding panels can be used to modify the mode being displayed.

Select one of the Panels below to learn more about it:

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

