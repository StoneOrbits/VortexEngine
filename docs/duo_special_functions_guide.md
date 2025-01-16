---
layout: default
title: Special Duo Functions
nav_order: 1
parent: Duo Guide
published: true
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

<img align="right" width="" height="220" class="device-icon" src="assets/images/duo-logo-square-512.png">

# Duo Special Functions

These are functions unique to the Duo Light.

## Force Shut Down

The Duo has the ability to be powered down from any mode or menu. This can help in case the user is unsure what is happening on the chip and wants to start over without accidentally changing settings. 

**To Force Shut Down:** Hold the button for 5 or more seconds until the LEDs turn off.

## One-Click Mode

**While off:** 
- Hold the button until the LEDs show white to activate One-Click mode. 
- The mode that was displayed before the chip was powered off will be selected for one-click mode. 

**While in One-Click mode**:
- Only one mode will be available.
- Click the button to toggle the chip on or off.
- While off, hold the button till the LEDs show white to deactivate One-Click mode.

## Battery Lock

To prevent the Duo from turning on by accidentally while being stored or transported, the battery can be locked.

**While off:** Click the button rapidly 5 times to activate the lock feature. 
**When locked:** Click the button rapidly 5 times to unlock.

## Advanced Menus

Most menus have an advanced version which gives greater control over that menu's features. These advanced menus are inaccessible by default. 

### Unlocking Advanced Menus

 - Enter the menu list by holding the button. 
 - Cycle through the menu colors by rapidly clicking 12 times to toggle the advanced menu lock. 
   - The LEDs will flash pink to indicate advanced menus are now available.
   - The LEDs will flash red to indicate advanced menus are now locked.
 - The advanced menus will remain unlocked even when the chip is powered down.

### Entering Advanced Menus

##### **This is only accessible when advanced menus are unlocked*

 - Enter the menu list by holding the button.
 - Click to cycle to the desired menu color.
 - Hold the button and release after the menu color blinks faster to enter that advanced menu.

## Advanced Menus

Coming Soon

<!-- {% raw %}
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
{% endraw %} -->





