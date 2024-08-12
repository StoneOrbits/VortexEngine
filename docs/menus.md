---
layout: default
title: Menus
nav_order: 8
parent: Vortex Engine
has_children: true
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
   width:16px;
   height:16px;
   margin-right:8px;
   margin-top: 24px;
   border-radius: 2px;
   border: 2px solid #dfdfdf;
   align-items: center;
}

.color-list-entry {
   align-items: center;
   font-size: 22px;
   font-weight: bold;
}

.device-icon {
   margin: 20px;
   width: 30%;
   height: 30%
}
</style>

# Menus

The menus provide a way to use the navigation buttons to customize your Vortex Device and manage it's configuration.

All Vortex Devices provide menus that allow you to:

 - Randomize the colors/patterns of a mode
 - Transfer a mode to other devices
 - Pick specific colors of a mode
 - Pick specific blinking patterns of a mode
 - Adjust the overall brightness of all leds
 - Reset the device to default settings
 - Connect to the PC for updates or building/saving modes

The menus to perform each of these actions work nearly identical on all devices, with only very minor differences due to led count and orientations.

## Entering Menus
**Holding** the navigation button until the leds blink white will open the **menu selection**.

When using a menu to configure the device it will only make changes to the mode that was playing right before opening the menu.

In order to make changes to a mode, you first cycle to that mode then **hold** the navigation button till the menu selection opens.

From here a menu can be chosen based on the desired changes to make.

## Menu List
Here's a brief overview of what each menu does:

<div class="color-list-entry"><span class="rounded-box white"></span>Randomizer</div>
The Randomizer menu allows you to randomize the settings of your current mode. This can be a fun way to discover new combinations of patterns and colorsets.
<div class="color-list-entry"><span class="rounded-box cyan"></span>Mode Sharing</div>
The Mode Sharing menu allows you to share your current mode with another Vortex Device. This is a great way to quickly and easily share your favorite modes with friends.
<div class="color-list-entry"><span class="rounded-box purple"></span>Editor Connection</div>
The Editor Connection menu is used to connect your Vortex Device to the Vortex Editor. This allows you to program your device using the editor's user-friendly interface. Please note that this menu is not available on the Duo.
<div class="color-list-entry"><span class="rounded-box green"></span>Color Select</div>
The Color Select menu allows you to change the colorset of the current mode. This is a quick and easy way to customize the appearance of your modes.
<div class="color-list-entry"><span class="rounded-box blue"></span>Pattern Select</div>
The Pattern Select menu allows you to change the pattern of the current mode. This allows you to customize the blinking speed or style of your modes.
<div class="color-list-entry"><span class="rounded-box yellow"></span>Global Brightness</div>
The Global Brightness menu allows you to adjust the overall brightness of your Vortex Device. This can be useful for saving battery life or adjusting the intensity of your light shows.
<div class="color-list-entry"><span class="rounded-box red"></span>Factory Reset</div>
The Factory Reset menu allows you to reset your Vortex Device to its factory settings. This can be useful if you want to start fresh or if you're experiencing issues with your device.
</div>

