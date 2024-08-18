---
layout: default
title: Color Select
nav_order: 4
parent: Menus
grand_parent: Vortex Engine
---

<style>
.slot {
  min-width: 50px;
  min-height: 50px;
  width: 50px;
  height: 50px;
  margin: 6px;
  border-radius: 50%;
  line-height: 50px;
  cursor: pointer;
  position: relative;
  display: inline-block;
  text-align: center;
  border: 2px solid transparent;
  transition: transform 0.2s ease-in-out, box-shadow 0.2s ease-in-out, border-color 0.2s ease-in-out;  
}

.slot.empty:hover {
  box-shadow: none;
  transform: none;
}

.slot.empty {
  box-shadow: none;
  transform: none;
}

.slot:not(.empty):hover {
  transform: scale(1.1);
}

.empty {
  background-color: #222;
  border: 2px dashed #555;
  cursor: default; /* No pointer cursor for empty slots */
}

.add-slot {
  background-color: #444;
  display: flex;
  justify-content: center;
  align-items: center;
  cursor: pointer;
  border: 2px solid #666;
}

.plus-icon {
  font-size: 64px;
  color: #fff;
  font-weight: bold;
  font-family: 'Times New Roman';
}

.save-slot {
  background-color: #888;
  background-size: cover;
}

#slots-container {
  display: flex;
  justify-content: center;
  margin-top: 50px;
  margin-bottom: 60px;
}

#color-select-diagram {
  width:240px;
  float:right;
  margin:0;
  margin-left:20px;
  margin-right:20px;
  padding:0;
}

.dropdown {
  box-shadow: 5px 5px 10px 4px rgba(0, 0, 0, 0.4);
  border-radius: 12px;
  transition: all 0.3s ease-in-out;
}

.dropdown-option {
  transition: transform 0.2s ease-in-out, box-shadow 0.2s ease-in-out, border-color 0.2s ease-in-out;
}

.dropdown-option:hover {
  transform: scale(1.1);
}

@keyframes flashRed {
  0% { 
  }
  50% {
    background-color: rgba(255, 0, 0, 0.6);
    box-shadow: 0 0 5px 2px rgba(255, 0, 0, 0.6);
  }
  100% {
  }
}

@media (max-width: 500px) {
  .slot {
    width: 40px;
    height: 40px;
    min-width: 40px;
    min-height: 40px;
    line-height: 40px;
    margin: 4px;
  }

  .plus-icon {
    font-size: 40px;
  }

  #slots-container {
    margin-top: 30px;
    margin-bottom: 40px;
  }

  #color-select-diagram {
    width:180px;
  }
}

@media (max-width: 400px) {
  .slot {
    width: 32px;
    height: 32px;
    min-width: 32px;
    min-height: 32px;
    line-height: 32px;
    margin: 4px;
  }

  .plus-icon {
    font-size: 32px;
  }

  #slots-container {
    margin-top: 30px;
    margin-bottom: 40px;
  }

  #color-select-diagram {
    width:160px;
  }
}
</style>

# Color Select

The Color Select menu allows customization of the colors in the current [Mode](mode.html). This collection of colors is commonly referred to as a [Colorset](colorsets.html).

## Try It Yourself

<img id="color-select-diagram" src="assets/images/color-select.png">

Below is an interactive colorset that mimics real color select menu functionality, you can **click to add/edit** colors and **hold to delete** colors.

<div id="slots-container">
    <div class="slot empty" data-slot="0"></div>
    <div class="slot empty" data-slot="1"></div>
    <div class="slot empty" data-slot="2"></div>
    <div class="slot empty" data-slot="3"></div>
    <div class="slot empty" data-slot="4"></div>
    <div class="slot empty" data-slot="5"></div>
    <div class="slot empty" data-slot="6"></div>
    <div class="slot empty" data-slot="7"></div>
</div>

<script src="{{ '/assets/js/ColorSelect.js' | relative_url }}"></script>

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
