---
layout: default
title: Color Select
nav_order: 4
parent: Menus
grand_parent: Vortex Engine
---

<style>
.static-box {
  width: 20px;
  height: 20px;
  margin-right: 7px;
  border-radius: 4px;
  display: inline-block;
  box-shadow: 3px 3px 10px 4px rgba(0, 0, 0, 0.3);
  vertical-align: middle;
}

.static-box-container {
  display: flex;
  align-items: center;
  margin: 10px;
  margin-left: 20px;
}

.static-box-container span {
  margin-left: 8px;
  font-size: 14px;
}

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

## Adding or Editing a Color
The **add** option will appear as a blinking white light on the first empty slot as long as the colorset isn't full.

To add a new color select the **add** option, to edit a color select just select the color itself.

### Choosing Colors

<div style="margin-bottom:15px" markdown="1">
  <div class="static-box-container">
      <div class="static-box" style="background: linear-gradient(to right, hsl(0, 100%, 50%), hsl(70, 100%, 50%));"></div>
      <div class="static-box" style="background: linear-gradient(to right, hsl(90, 100%, 50%), hsl(160, 100%, 50%));"></div>
      <div class="static-box" style="background: linear-gradient(to right, hsl(180, 100%, 50%), hsl(250, 100%, 50%));"></div>
      <div class="static-box" style="background: linear-gradient(to right, hsl(270, 100%, 50%), hsl(340, 100%, 50%));"></div>
      <span>1. **Select Hue Quadrant**: Pick which quadrant contains the desired hue.</span>
  </div>

  <div class="static-box-container">
      <div class="static-box" style="background-color: hsl(0, 100%, 50%);"></div>
      <div class="static-box" style="background-color: hsl(22.5, 100%, 50%);"></div>
      <div class="static-box" style="background-color: hsl(45, 100%, 50%);"></div>
      <div class="static-box" style="background-color: hsl(67.5, 100%, 50%);"></div>
      <span>2. **Select Hue**: Pick the specific hue to decide the base color.</span>
  </div>

  <div class="static-box-container">
      <div class="static-box" style="background-color: hsl(0, 100%, 50%);"></div>
      <div class="static-box" style="background-color: hsl(0, 66%, 60%);"></div>
      <div class="static-box" style="background-color: hsl(0, 33%, 80%);"></div>
      <div class="static-box" style="background-color: hsl(0, 0%, 100%);"></div>
      <span>3. **Select Saturation**: Pick the saturation to adjust how white the color appears.</span>
  </div>

  <div class="static-box-container">
      <div class="static-box" style="background-color: hsl(0, 100%, 50%);"></div>
      <div class="static-box" style="background-color: hsl(0, 100%, 33%);"></div>
      <div class="static-box" style="background-color: hsl(0, 100%, 17%);"></div>
      <div class="static-box" style="background-color: hsl(0, 100%, 0%);"></div>
      <span>4. **Select Brightness**: Pick the brightness to adjust how dark the color appears.</span>
  </div>
</div>

After selecting brightness the menu will go back to the slot selection and reflect the new choice.

## Deleting Colors

To **delete** a color:
 1. cycle to the color slot
 2. **hold** the button until the LED glows red
 3. release the button **while the LED is red**

**Warning**: If all colors are deleted, the mode will not display anything and appear as if the device is off.

## Saving

The **save** option always appears at the end, selecting this option will save the changes and exit the menu.

You can always exit **without** saving by turning the device off.