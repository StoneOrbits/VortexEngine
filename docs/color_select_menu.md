---
layout: default
title: Color Select
nav_order: 4
parent: Menus
grand_parent: Vortex Engine
---

<style>
.slot {
    width: 50px;
    height: 50px;
    margin: 5px;
    border-radius: 50%;
    border: 2px solid #777;
    line-height: 50px;
    cursor: pointer;
    position: relative;
    display: inline-block;
}

.empty {
    background-color: #444;
}

.add-slot {
    background-color: #fff;
    color: #000;
    font-weight: bold;
    text-align: center;
}

.save-slot {
    background-color: #888;
    background-size: cover;
}
</style>

# Color Select

The Color Select menu allows customization of the colors in the current [Mode](mode.html). This collection of colors is commonly referred to as a [Colorset](colorsets.html).

## Overview

<div id="slots-container" style="display: flex; justify-content: center; margin-bottom: 20px;">
    <div class="slot" id="slot1" style="background-color: red;" onclick="editColor(1)"></div>
    <div class="slot" id="slot2" style="background-color: green;" onclick="editColor(2)"></div>
    <div class="slot" id="slot3" style="background-color: blue;" onclick="editColor(3)"></div>
    <div class="slot empty" id="slot4" onclick="editColor(4)"></div>
    <div class="slot add-slot" id="slot5" onclick="editColor(5)">+</div>
    <div class="slot empty" id="slot6"></div>
    <div class="slot empty" id="slot7"></div>
    <div class="slot empty" id="slot8"></div>
    <div class="slot save-slot" onclick="saveColors()"></div>
</div>

<script src="{{ '/assets/js/ColorSelect.js' | relative_url }}"></script>

<p>In the above diagram, the <em>colorset</em> contains four colors: 
<span style="color: #f00">Red</span>, 
<span style="color: #0f0">Green</span>, 
<span style="color: #07f">Blue</span>, and 
<span style="color: #090909">Blank</span></p>

<img src="assets/images/color-select.png">

In the above diagram, the _colorset_ contains four colors: <span style="color: #f00">Red</span>, <span style="color: #0f0">Green</span>, <span style="color: #07f">Blue</span>, and <span style="color: #090909">Blank</span>

The options in the menu include: [adding](color_select_menu.html#Editing-Color), [editing](color_select_menu.html#Editing-Color), [deleting](color_select_menu.html#Editing-Color), and [saving](color_select_menu.html#Editing-Color).

The options are the same on all devices, but are displayed differently:

 - [Orbit Color Select](orbit_color_select.html)
 - [Handle Color Select](handle_color_select.html)
 - [Gloves Color Select](gloves_color_select.html)

## Adding a New Color
The **add** option will always appear on the first empty slot if there are less than 8 colors.

It will appears as a blinking white light, select it to add a new color to the colorset.

Follow the [Selecting Colors](color_select_menu.html#Editing-Colo) section to pick the color.

## Editing Colors
To **edit** a color: select it then follow the [Selecting Colors](color_select_menu.html#Editing-Colo) section to pick the color.

## Deleting Colors

To **delete** a color:
 1. cycle to the color slot
 2. **hold** the button until the LED glows red
 3. release the button **while the LED is red**

**Warning**: If all colors are deleted, the mode will not display anything and appear as if the device is off.

## Saving

The **save** option always appears at the end, selecting this option will save the changes and exit the menu.

## Selecting Colors

 1. **Select a Slot**: Start by selecting the slot where you want to adjust the color.

 2. **Select Hue Quadrant**: Pick which quadrant contains the desired hue.

 3. **Select Hue**: Pick the specific hue to decide the base color.

 4. **Select Saturation**: Pick the saturation to adjust how white the color appears.

 5. **Select Brightness**: Pick the brightness to adjust how dark the color appears.

After selecting brightness the menu will go back to the slot selection and reflect the new choice.