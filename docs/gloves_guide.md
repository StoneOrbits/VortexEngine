---
layout: default
title: Gloves Guide
nav_order: 1
parent: Guides & Tutorials
---

<style>
.device-icon {
   margin: 0px;
   width: 30%;
   height: 30%
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
   width: 30%;
   height: 30%;
}
</style>

<img align="right" width="" height="220" class="device-icon" src="assets/images/device-gloves.png">

# Gloves Guide

Welcome to the Vortex Wired Gloves Guide, this page aims to get you up and running as fast as possible.

## Buttons

The gloves have one <span style="color: #00c6ff">navigation button</span> and a small <span style="color: #00ff00">power switch</span> on each glove.

There is also a very small <span style="color: #ff0000">reset button</span> for updating the device firmware.

## Navigating Modes

Short click the <span style="color: #00c6ff">navigation button</span> to forward through the 14 available modes.

To edit a mode, navigate to it then hold the navigation button to open the menus.

## Menus

Use short clicks to cycle through the following menu options, long click to enter a menu.

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
layout: default
title: Gloves Guide
nav_order: 3
parent: Guides & Tutorials
---

<style>
.white { background-color: rgba(255, 255, 255, 0.6); }
.cyan { background-color: rgba(0, 255, 255, 0.6); }
.purple { background-color: rgba(128, 0, 128, 0.6); }
.green { background-color: rgba(0, 128, 0, 0.6); }
.blue { background-color: rgba(0, 0, 255, 0.6); }
.yellow { background-color: rgba(255, 255, 0, 0.6); }
.red { background-color: rgba(255, 0, 0, 0.6); }

.rounded-box { 
   display:inline-block;
   width:28px;
   height:12px;
   margin-right:8px;
   margin-left:8px;
   margin-top: 4px;
   border-radius: 2px;
   border: 2px solid #dfdfdf;
   align-items: center;
}

.color-list-entry {
   align-items: center;
   font-size: 18px;
}

.device-icon {
   margin: 20px;
   width: 30%;
   height: 30%
}
</style>

<img align="right" width="" height="220" class="device-icon" src="assets/images/gloves-logo-square-512.png">

# Gloves Quick Use Guide

Welcome to the Vortex Gloves Quick-Use-Guide, this guide aims to get you up and running as fast as possible.

## Accessing Menus

To enter the menu:

1. **Hold the Navigation Button:** Hold the button until the LEDs alternate white blinks.
2. **Cycle Menus:** Use short clicks to cycle through menu options. The menu colors indicate their function:

    <div class="color-list-entry"><span class="rounded-box white"></span>Randomizer</div>
    <div class="color-list-entry"><span class="rounded-box cyan"></span>Mode Sharing</div>  
    <div class="color-list-entry"><span class="rounded-box purple"></span>Editor Connection</div>
    <div class="color-list-entry"><span class="rounded-box green"></span>Color Select</div>  
    <div class="color-list-entry"><span class="rounded-box blue"></span>Pattern Select</div>
    <div class="color-list-entry"><span class="rounded-box yellow"></span>Global Brightness</div>
    <div class="color-list-entry"><span class="rounded-box red"></span>Factory Reset</div>

## Selecting LEDs for Colorset/Pattern/Randomization

1. **Enter the Desired Menu:** Use a long click to select.
2. **LED Selection:**
   - Short click to cycle between [LED groups](LEDgroups.html).
   - Long click to select the LED group you want to customize.
   - You can also make a [custom LED group](CustomGroups.html).

## Randomizing a Mode

1. **Select Randomizer Menu (White):**
2. **Select an LED group.**
3. **Randomize:**
   - Short click to cycle through randomizations.
   - Long click to confirm and save the randomized LEDs.

Learn more about the [Randomizer](randomizer.html).

## Changing the Colorset on a Mode
<img align="right" width="" height="220" src="assets/images/ColorSelect.png">

1. **Select Color Select Menu (Green):**
2. **Select an LED group.**
3. **Edit Colorset:**
   - Click to navigate through the colors in your set.
   - Long click a color to edit it.
   - Hold on a color until it flashes red to delete it.
   - Long click the slot after your colorset to add a color.
   - Select the red edge lights to save and exit.

Learn more about [Color Selection](colorSelect.html).

## Changing the Pattern on a Mode

1. **Select Pattern Select Menu (Blue):**
2. **Select an LED group.**
3. **Select Pattern:**
   - Choose one of the 4 shortcuts to enter the pattern list.
   - Cycle through patterns and long click to apply.

## Wirelessly Transferring to Another Device

1. **Select Mode Sharing Menu (Cyan):**
2. **Initiate Transfer:**
   - Short click to cycle between receive (default) and send.
   - Point the button of the sending device at the button of the receiving device.
   - The receiving device will start to glow green and play the mode once it is received.

## Connecting to the PC

To connect to PC and web applications see [Connecting to Computer](computer.html).

## Changing Global Brightness

1. **Select Global Brightness Menu (Yellow):**
2. **Adjust Brightness:**
   - Use short clicks to cycle through brightness options.
   - Long click to set.

## Factory Resetting the Device

1. **Select Factory Reset Menu (Red):**
2. **Confirm Reset:**
   - Short click to cycle between factory reset or cancel.
   - Hold the factory reset option until the device fades from red to white to reset.

