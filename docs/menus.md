---
layout: default
title: Menus
nav_order: 8
parent: Vortex Engine
has_children: true
---

<style>
/* Updated colors for dark theme compatibility */
.white { background-color: rgba(200, 200, 200, 0.6); }
.cyan { background-color: rgba(0, 200, 200, 0.6); }
.purple { background-color: rgba(100, 0, 100, 0.6); }
.green { background-color: rgba(0, 100, 0, 0.6); }
.blue { background-color: rgba(0, 0, 100, 0.6); }
.yellow { background-color: rgba(200, 200, 0, 0.6); }
.red { background-color: rgba(200, 0, 0, 0.6); }

.rounded-box { 
   display: inline-block;
   width: 16px;
   height: 16px;
   margin-right: 8px;
   border-radius: 2px;
   border: 2px solid #555;
   vertical-align: middle;
}

.color-list-entry {
   display: flex;
   align-items: center;
   font-size: 22px;
   font-weight: bold;
   margin-bottom: 0; /* Remove bottom margin for seamless transition */
   padding: 8px;
   border: 1px solid #333;
   border-bottom: none; /* Remove bottom border */
   border-radius: 4px 4px 0 0; /* Round top corners only */
   background-color: #222; /* Darker background */
   color: #eee; /* Light text for contrast */
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
}

.device-icon {
   margin: 20px;
   width: 30%;
   height: 30%;
}
</style>

# Menus

The menus provide a way to use the navigation buttons to customize your Vortex Device and manage its configuration.

All Vortex Devices provide menus that allow you to:

- Randomize the colors/patterns of a mode
- Transfer a mode to other devices
- Pick specific colors of a mode
- Pick specific blinking patterns of a mode
- Adjust the overall brightness of all LEDs
- Reset the device to default settings
- Connect to the PC for updates or building/saving modes

The menus to perform each of these actions work nearly identical on all devices, with only very minor differences due to LED count and orientations.

## Entering Menus

**Holding** the navigation button until the LEDs blink white will open the **menu selection**.

When using a menu to configure the device, it will only make changes to the mode that was playing right before opening the menu.

To make changes to a mode, you first cycle to that mode, then **hold** the navigation button till the menu selection opens.

From here, a menu can be chosen based on the desired changes to make.

## Menu List

Here's a brief overview of what each menu does:

<div class="color-list-entry"><span class="rounded-box white"></span>Randomizer</div>
<div>The Randomizer menu allows you to randomize the settings of your current mode. This can be a fun way to discover new combinations of patterns and colorsets.</div>

<div class="color-list-entry"><span class="rounded-box cyan"></span>Mode Sharing</div>
<div>The Mode Sharing menu allows you to share your current mode with another Vortex Device. This is a great way to quickly and easily share your favorite modes with friends.</div>

<div class="color-list-entry"><span class="rounded-box purple"></span>Editor Connection</div>
<div>The Editor Connection menu is used to connect your Vortex Device to the desktop based editors. This allows you to program your device using the editor's user-friendly interface. Please note that this menu is not available on the Duo.</div>

<div class="color-list-entry"><span class="rounded-box green"></span>Color Select</div>
<div>The Color Select menu allows you to change the colors of the current mode. This is a quick and easy way to customize the colorful appearance of your modes.</div>

<div class="color-list-entry"><span class="rounded-box blue"></span>Pattern Select</div>
<div>The Pattern Select menu allows you to change the pattern of the current mode. This allows you to customize the blinking speed or style of your modes.</div>

<div class="color-list-entry"><span class="rounded-box yellow"></span>Global Brightness</div>
<div>The Global Brightness menu allows you to adjust the overall brightness of all leds. This can be useful for saving battery life or adjusting the intensity of your lightshows.</div>

<div class="color-list-entry"><span class="rounded-box red"></span>Factory Reset</div>
<div>The Factory Reset menu allows you to reset your Vortex Device to its factory settings. This allows you to start fresh with default modes, and is a good first step if you experience any issues with your device.</div>
