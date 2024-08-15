---
layout: default
title: Factory Reset
nav_order: 7
parent: Menus
grand_parent: Vortex Engine
---

<style>
  .reset-container {
    display: flex;
    flex-direction: column;
    margin-left: 20px;
    margin-bottom: 20px;
    gap: 16px;
  }

  .reset-step {
    display: flex;
    align-items: center;
    gap: 12px;
  }

  .reset-box {
    width: 20px;
    height: 20px;
    border-radius: 4px;
    position: relative;
    display: inline-block;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
  }

  .white-blink {
    background-color: #ffffff;
    animation: blink 1s infinite;
  }

  .red-blink {
    background-color: #ff0000;
    animation: blinkFast 0.3s infinite;
  }

  .hold-transition {
    background-color: #ff0000;
    animation: blinkFast 0.3s infinite, fadeToWhite 3s linear infinite;
  }

  .white-solid {
    background-color: #ffffff;
    animation: none; /* Ensures no animation is applied */
  }

  @keyframes blink {
    50% { opacity: 0; }
  }

  @keyframes blinkFast {
    50% { opacity: 0; }
  }

  @keyframes fadeToWhite {
    0% {
      background-color: #ff0000;
    }
    100% {
      background-color: #ffffff;
    }
  }

  .reset-text {
    font-weight: bold;
  }
</style>

# Factory Reset

The Factory Reset menu restores the Vortex Device to its original settings.

## Using Factory Reset

The steps to use Factory Reset are:

<div class="reset-container">
  <div class="reset-step">
    <div class="reset-box white-blink"></div>
    <div class="reset-text">Blinking white</div>
    Select to exit without reset, or click to go to red blink
  </div>
  
  <div class="reset-step">
    <div class="reset-box red-blink"></div>
    <div class="reset-text">Blinking red</div>
    Hold to begin reset, or click to go back to white blink
  </div>
  
  <!--<div class="reset-step"> -->
  <!--  <div class="reset-box hold-transition"></div> -->
  <!--  <div class="reset-text">Blinks to white</div> -->
  <!--  Hold till led blinks to solid white and release -->
  <!--</div> -->
  
  <div class="reset-step">
    <div class="reset-box white-solid"></div>
    <div class="reset-text">Solid white</div>
    Release on solid white to perform reset
  </div>
</div>

After the reset, the device returns to its factory settings, with all modes and options restored to default.
