---
layout: default
title: Lightshow.lol Examples
nav_order: 2
parent: Lightshow.lol
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

Lightshow.lol is an online tool that allows you to design, preview, and export custom [modes](modes.html) for any Vortex devices. 

The lightshow preview is the main focus of the page, with various control panels serving to modify the pattern, colorset, and preview animation.

{: .warning }
Although the preview is an exact simulation of a real device, due to the difference in the way color is produced by LEDs and displays **the preview will never look exactly the same as real LEDs**

### What is a Vortex Editor

In the **Vortex Engine** ecosystem a [mode](modes.html) consists of a [pattern](patterns.html) and a [colorset](colorsets.html) for each LED on a device.

For example, the **Spark Orbit** has **6 LEDs** which means **6 Patterns** and **6 Colorsets** -- one for each LED.

A **Vortex Editor** is any kind of software designed to edit the **Patterns** or **Colorsets** in the **Modes** of a **Vortex Device**.

### Panel Breakdown
In order to get familiar with how lightshow.lol works it is important to first familiarize yourself with each UI Panel.

Select one of the Panels below to learn more about it:

<div class="panel-grid">
  <a href="lightshow_lol_animation.html" class="panel-link">
    <span class="panel-title">🔗 Animation Panel →</span>
    <img src="assets/images/lightshow-lol-animation.png" class="panel-img">
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
  <a href="lightshow_lol_update_panel.html" class="panel-link">
    <span class="panel-title">🔗 Update Panel →</span>
    <img src="assets/images/lightshow-lol-device-update.png" class="panel-img">
  </a>
  <a href="lightshow_lol_chromalink_duo.html" class="panel-link">
    <span class="panel-title">🔗 Chromalink Panel →</span>
    <img src="assets/images/lightshow-lol-chromalink-duo-basic.png" class="panel-img">
  </a>
</div>

### Example Workflows

Once you're satisfied with your understanding of the UI Panels then read the example workflows at the bottom of this page.

To use Lightshow.lol with

