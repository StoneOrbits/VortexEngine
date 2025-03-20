---
layout: default
title: Duo
nav_order: 5
parent: Vortex Devices
published: true
---

<style>
.device-icon {
   margin: 20px auto;
   width: 20%;
   height: auto;
}

.white { background-color: rgba(255, 255, 255); }
.cyan { background-color: rgba(0, 255, 255); }
.purple { background-color: rgba(128, 0, 128); }
.green { background-color: rgba(0, 128, 0); }
.blue { background-color: rgba(0, 0, 255); }
.yellow { background-color: rgba(255, 255, 0); }
.red { background-color: rgba(255, 0, 0); }

.rounded-box { 
   display: inline-block;
   width: 16px;
   height: 16px;
   margin-right: 8px;
   margin-left: 5px;
   border-radius: 2px;
   border: 2px solid #555;
   vertical-align: middle;
}

.color-list-entry {
   display: flex;
   align-items: center;
   font-size: 16px;
   font-weight: bold;
   margin-bottom: 10px; /* Remove bottom margin for seamless transition */
   padding: 8px;
   border: 1px solid #333;
   border-bottom: none; /* Remove bottom border */
   border-radius: 4px; /* Round top corners only */
   background-color: #222; /* Darker background */
   color: #eee; /* Light text for contrast */
   transition: background-color 0.3s; /* Smooth background color transition */
}

.color-list-entry:hover {
   background-color: #2a2a2a; /* Slightly lighter on hover */
}

.color-list-entry + div {
   margin-top: 0; /* Remove top margin for seamless transition */
   margin-bottom: 16px;
   padding-left: 30px;
   font-size: 18px;
   line-height: 1.5;
   padding: 10px;
   border: 1px solid #333;
   border-top: none; /* Remove top border */
   border-radius: 0 0 4px 4px; /* Round bottom corners only */
   background-color: #333; /* Dark background for text */
   color: #ccc; /* Light text for readability */
   transition: background-color 0.3s; /* Smooth background color transition */
}

.color-list-entry + div:hover {
   background-color: #3a3a3a; /* Slightly lighter on hover */
}

.menu-wrapper {
   padding-left: 60px;
   padding-right: 60px;
}

/* Scoped link styles within the menu section */
.menu-wrapper a {
   text-decoration: none; /* Remove default link styling */
   color: inherit; /* Inherit color from parent */
   display: block; /* Ensures the link covers the whole section */
}

.device-icon {
   margin: 20px;
   width: 20%;
   height: 20%;
}
</style>

<img align="right" width="" height="220" class="device-icon" src="assets/images/duo-logo-button-square-512.png">

# The Duo

Welcome to the Duo LED Guide, this page aims to get you up and running as fast as possible.

## Basics

  The Duo has one <strong style="color: #00c6ff">navigation button</strong>, this button also acts as the <strong style="color: #00ff00;">power switch</strong>.

**Power**

- _While off:_ Click the button to power on the Duo.
- _When On_: Hold the navigation button and release when the lights go out to power off the Duo.

**Special Functions**

- The Duo has some unique <a href="duo_special_functions_guide.html">Special Functions</a>. Check out the basic menus before diving into these options.

**Using The Button**

- Short click the <span style="color: #00c6ff">navigation button</span> to cycle forward through the 9 available modes.
- To open the menus, navigate to a mode then hold the navigation button until the lights flash white.
- After opening the menus, use short clicks to cycle through the following menu options, long click to enter a menu.

---

# Duo Interactive Tutorial

Learn how to use the Duo with this interactive tutorial:

<div style="text-align: center; margin: 20px 0;">
  <iframe 
    src="https://opensourcelightshows.github.io/DuoTutorial/" 
    width="90%" 
    height="600" 
    style="border: 5px solid #444; border-radius: 8px;"
    title="Duo Tutorial"
    allowfullscreen>
  </iframe>
</div>

---

## Menus

{% raw %}
<div class="menu-wrapper">
<a href="randomizer_menu.html">
  <div class="color-list-entry"><span class="rounded-box white"></span>Randomizer</div>
</a>

<a href="mode_sharing_menu.html">
  <div class="color-list-entry"><span class="rounded-box cyan"></span>Mode Sharing</div>
</a>

<a href="color_select_menu.html">
  <div class="color-list-entry"><span class="rounded-box green"></span>Color Select</div>
</a>

<a href="pattern_select_menu.html">
  <div class="color-list-entry"><span class="rounded-box blue"></span>Pattern Select</div>
</a>

<a href="global_brightness_menu.html">
  <div class="color-list-entry"><span class="rounded-box yellow"></span>Global Brightness</div>
</a>

<a href="factory_reset_menu.html">
  <div class="color-list-entry"><span class="rounded-box red"></span>Factory Reset</div>
</a>
</div>
{% endraw %}

---

## Next Steps
<div class="next-steps">
  <ul>
    <li><a href="basic_usage.html">Explore Vortex Engine</a>: Learn key terms, concepts, and how to use on-device menus.</li>
    <li><a href="duo_chromalink_guide.html">Connect a Chromalink</a>: Connect your Duo to the PC with a Chromadeck to get the latest firmware.</li>
  </ul>
</div>

---

If you have any questions or need further assistance, check out the [Support Page](support.html).




