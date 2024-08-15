---
layout: default
title: Web Emulator
nav_order: 50
parent: Vortex Engine
published: false
---

<style>
#controls {
  margin-top: 10px;
  text-align: center;
}

#vortex-canvas {
  width: 256px;
  height: 256px;
  border: 2px black solid;
  border-radius: 50%;
}

.lightshow-button {
  background-color: #007bff;
  border: none;
  color: white;
  padding: 10px 20px;
  margin: 20px 5px;
  font-size: 16px;
  border-radius: 5px;
  cursor: pointer;
  transition: background-color 0.3s;
}

.lightshow-button:hover {
  background-color: #0056b3;
}
</style>

# Web Emulator

<script src="{{ '/assets/js/VortexLib.js' | relative_url }}" type="module"></script>
<script src="{{ '/assets/js/Lightshow.js' | relative_url }}" type="module"></script>
<script src="{{ '/assets/js/initLightshow.js' | relative_url }}" type="module"></script>

<div id="controls">
  <div id="lightshow-container">
    <canvas id="vortex-canvas"></canvas>
  </div>
  <button id="cycle-button" class="lightshow-button">Cycle</button>
  <button id="select-button" class="lightshow-button">Select</button>
  <button id="menus-button" class="lightshow-button">Menus</button>
</div>