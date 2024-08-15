---
layout: default
title: Global
nav_order: 6
parent: Menus
grand_parent: Vortex Engine
---

<style>
  .brightness-container {
    display: flex;
    justify-content: center;
    align-items: center;
    gap: 20px; /* Adds space between each brightness option */
    margin-bottom: 16px;
  }

  .brightness-box {
    width: 20px;
    height: 20px;
    border-radius: 4px;
    display: inline-block;
    box-shadow: 0 0 20px rgba(0, 0, 0, 0.8);
  }

  .yellow-1 {
    background-color: #665500;
    box-shadow: 0 0 10px rgba(102, 85, 0, 0.8);
  }

  .yellow-2 {
    background-color: #998800;
    box-shadow: 0 0 15px rgba(153, 136, 0, 0.8);
  }

  .yellow-3 {
    background-color: #ccbb00;
    box-shadow: 0 0 20px rgba(204, 187, 0, 0.8);
  }

  .yellow-4 {
    background-color: #ffee00;
    box-shadow: 0 0 25px rgba(255, 238, 0, 0.8);
  }

  .red-blink {
    background-color: #ff0000;
    box-shadow: 0 0 20px rgba(255, 0, 0, 0.8);
    animation: blink 1s infinite;
  }

  @keyframes blink {
    50% { opacity: 0; }
  }

  .brightness-label {
    text-align: center;
    margin-top: 8px;
  }
</style>

# Global Brightness

The Global Brightness menu adjusts the overall brightness of the Vortex Device, helping to save battery life or change the intensity of lightshows.

## Using the Global Brightness Menu

When the Global Brightness menu is entered, the device shows the current brightness level. Short click to cycle through brightness levels and select on to save and exit.


<div class="brightness-container">
  <div>
    <div class="brightness-box yellow-1"></div>
    <div class="brightness-label"><strong>Low</strong></div>
  </div>
  <div>
    <div class="brightness-box yellow-2"></div>
    <div class="brightness-label"><strong>Med</strong></div>
  </div>
  <div>
    <div class="brightness-box yellow-3"></div>
    <div class="brightness-label"><strong>High</strong></div>
  </div>
  <div>
    <div class="brightness-box yellow-4"></div>
    <div class="brightness-label"><strong>Max</strong></div>
  </div>
  <div>
    <div class="brightness-box red-blink"></div>
    <div class="brightness-label"><strong>Exit</strong></div>
  </div>
</div>
