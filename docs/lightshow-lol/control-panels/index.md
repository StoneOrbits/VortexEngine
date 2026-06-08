---
layout: default
title: Control Panels
nav_order: 10
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

# Control Panels

Here you can read about individual control panels to gain additional insight that may not come up in the other tutorials.

Depending on whether lightshow.lol is used on **mobile** or **desktop** the layout will appear different, but the function of these panels will be nearly the same.

Pick one of the panels below to learn more about it:

<div class="panel-grid">
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/animation.md %}" class="panel-link">
    <span class="panel-title">🔗 Animation Panel →</span>
    <img src="{{ '/assets/images/lightshow-lol-animation.png' | relative_url }}" class="panel-img">
  </a>
    <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/pattern.md %}" class="panel-link">
    <span class="panel-title">🔗 Pattern Panel →</span>
    <img src="{{ '/assets/images/lightshow-lol-pattern.png' | relative_url }}" class="panel-img">
  </a>
    <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/colorset.md %}" class="panel-link">
    <span class="panel-title">🔗 Colorset Panel →</span>
    <img src="{{ '/assets/images/lightshow-lol-colorset.png' | relative_url }}" class="panel-img">
  </a>
    <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/color-picker.md %}" class="panel-link">
    <span class="panel-title">🔗 Color Picker →</span>
    <img src="{{ '/assets/images/lightshow-lol-color-picker.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/device-controls.md %}" class="panel-link">
    <span class="panel-title">🔗 Device Control →</span>
    <img src="{{ '/assets/images/lightshow-lol-device.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/modes-list.md %}" class="panel-link">
    <span class="panel-title">🔗 Modes Panel →</span>
    <img src="{{ '/assets/images/lightshow-lol-modes.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/led-selection.md %}" class="panel-link">
    <span class="panel-title">🔗 Led Selection →</span>
    <img src="{{ '/assets/images/lightshow-lol-led-select.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/update-panel.md %}" class="panel-link">
    <span class="panel-title">🔗 Update Panel →</span>
    <img src="{{ '/assets/images/lightshow-lol-device-update.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/chromalink-duo.md %}" class="panel-link">
    <span class="panel-title">🔗 Chromalink Panel →</span>
    <img src="{{ '/assets/images/lightshow-lol-chromalink-duo-basic.png' | relative_url }}" class="panel-img">
  </a>
</div>

