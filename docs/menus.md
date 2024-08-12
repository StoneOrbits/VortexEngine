---
layout: default
title: Menus
nav_order: 8
parent: Vortex Engine
has_children: true
---

<style>
/* Updated colors for dark theme compatibility */
.white { background-color: rgba(255, 255, 255); }
.cyan { background-color: rgba(0, 255, 255); }
.purple { background-color: rgba(150, 0, 150); }
.green { background-color: rgba(0, 255, 0); }
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
   font-size: 22px;
   font-weight: bold;
   margin-bottom: 0; /* Remove bottom margin for seamless transition */
   padding: 8px;
   border: 1px solid #333;
   border-bottom: none; /* Remove bottom border */
   border-radius: 4px 4px 0 0; /* Round top corners only */
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

# Menus

The menus provide a way to use the navigation buttons to customize your Vortex Device and manage its configuration.

The menus to perform each of these actions work nearly identical on all devices, with only very minor differences due to LED count and orientations.

## Using Menus

To access the menu selection, first switch to the mode you'd like to edit and then hold the navigation button until the LEDs blink white.

The <span style="color: #00ff00;">Color Select</span>, <span style="color: #3366ff;">Pattern Select</span>, <span style="color: #ffffff;">Randomizer</span>, and <span style="color: #00ffff;">Mode Sharing</span> menus will only affect this mode.

<span style="color: #ff0000;">Factory Reset</span>, <span style="color: #ffff00;">Global Brightness</span>, and <span style="color: #b300b3;">Editor Connection</span> will apply to the entire device.

Once the menu selection opens, navigate to the color that corresponds to the menu you'd like to enter and select it.

## Menu List

<div class="menu-wrapper" markdown="1">
{% raw %}
<a href="randomizer_menu.html">
   <div class="color-list-entry"><span class="rounded-box white"></span>Randomizer</div>
   <div>The Randomizer menu allows you to randomize the settings of your current mode. This can be a fun way to discover new combinations of patterns and colorsets.</div>
</a>

<a href="mode_sharing_menu.html">
   <div class="color-list-entry"><span class="rounded-box cyan"></span>Mode Sharing</div>
   <div>The Mode Sharing menu allows you to share your current mode with another Vortex Device. This is a great way to quickly and easily share your favorite modes with friends.</div>
</a>

<a href="editor_connection_menu.html">
   <div class="color-list-entry"><span class="rounded-box purple"></span>Editor Connection</div>
   <div>The Editor Connection menu is used to connect your Vortex Device to the desktop-based editors. This allows you to program your device using the editor's user-friendly interface. Please note that this menu is not available on the Duo.</div>
</a>

<a href="color_select_menu.html">
   <div class="color-list-entry"><span class="rounded-box green"></span>Color Select</div>
   <div>The Color Select menu allows you to change the colors of the current mode. This is a quick and easy way to customize the colorful appearance of your modes.</div>
</a>

<a href="pattern_select_menu.html">
   <div class="color-list-entry"><span class="rounded-box blue"></span>Pattern Select</div>
   <div>The Pattern Select menu allows you to change the pattern of the current mode. This allows you to customize the blinking speed or style of your modes.</div>
</a>

<a href="global_brightness_menu.html">
   <div class="color-list-entry"><span class="rounded-box yellow"></span>Global Brightness</div>
   <div>The Global Brightness menu allows you to adjust the overall brightness of all LEDs. This can be useful for saving battery life or adjusting the intensity of your light shows.</div>
</a>

<a href="factory_reset_menu.html">
   <div class="color-list-entry"><span class="rounded-box red"></span>Factory Reset</div>
   <div>The Factory Reset menu allows you to reset your Vortex Device to its factory settings. This allows you to start fresh with default modes and is a good first step if you experience any issues with your device.</div>
</a>
{% endraw %}
</div>
