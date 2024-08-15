---
layout: default
title: Led Selection
nav_order: 0
parent: Menus
grand_parent: Vortex Engine
---

# Led Selection

The <span style="color: #00ff00;">Color Select</span>, <span style="color: #3366ff;">Pattern Select</span>, and <span style="color: #ffffff;">Randomizer</span> will change only the leds which were selected upon entering the menu.

The led selection is always the first step after entering one of these menus.

<script src="{{ '/assets/js/VortexLib.js' | relative_url }}" type="module"></script>
<script src="{{ '/assets/js/Lightshow.js' | relative_url }}" type="module"></script>
<script src="{{ '/assets/js/initLightshow.js' | relative_url }}" type="module"></script>

<div id="lightshow-container" style="width: 100%; height: 400px;">
  <canvas id="vortex-canvas"></canvas>
</div>
