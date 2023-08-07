---
layout: default
title: Menus
nav_order: 4
parent: Vortex Engine
has_children: true
---

<style>
.section {
    padding: 20px;
    margin: 20px 0;
}
.white  { background: linear-gradient(to bottom, rgba(255, 255, 255, 0.3), transparent); }
.cyan   { background: linear-gradient(to bottom, rgba(0, 255, 255, 0.3), transparent); }
.purple { background: linear-gradient(to bottom, rgba(128, 0, 128, 0.3), transparent); }
.green  { background: linear-gradient(to bottom, rgba(0, 128, 0, 0.3), transparent); }
.blue   { background: linear-gradient(to bottom, rgba(0, 0, 255, 0.3), transparent); }
.yellow { background: linear-gradient(to bottom, rgba(255, 255, 0, 0.3), transparent); }
.red    { background: linear-gradient(to bottom, rgba(255, 0, 0, 0.3), transparent); }
</style>

# Menus

Each of these menus serves a specific purpose and allows you to customize your Vortex Device in different ways. When you open the menus on any Vortex Device, whichever mode slot was selected and playing when you open the menus will be targeted as the mode to be configured by whichever menu you may enter.

Here's a brief overview of what each menu does:

{% capture section %}
## [Randomizer](randomizer_menu.html)
The Randomizer menu allows you to randomize the settings of your current mode. This can be a fun way to discover new combinations of patterns and colorsets.
{% endcapture %}
{% assign content = section | markdownify %}
<div class="section white">{{ content }}</div>

{% capture section %}
## [Mode Sharing](mode_sharing_menu.html)
The Mode Sharing menu allows you to share your current mode with another Vortex Device. This is a great way to quickly and easily share your favorite modes with friends.
{% endcapture %}
{% assign content = section | markdownify %}
<div class="section cyan">{{ content }}</div>

{% capture section %}
## [Editor Connection](editor_connection_menu.html)
The Editor Connection menu is used to connect your Vortex Device to the Vortex Editor. This allows you to program your device using the editor's user-friendly interface. Please note that this menu is not available on the Duo.
{% endcapture %}
{% assign content = section | markdownify %}
<div class="section purple">{{ content }}</div>

{% capture section %}
## [Color Select](color_select_menu.html)
The Color Select menu allows you to change the colorset of the current mode. This is a quick and easy way to customize the appearance of your modes.
{% endcapture %}
{% assign content = section | markdownify %}
<div class="section green">{{ content }}</div>

{% capture section %}
## [Pattern Select](pattern_select_menu.html)
The Pattern Select menu allows you to change the pattern of the current mode. This allows you to customize the blinking speed or style of your modes.
{% endcapture %}
{% assign content = section | markdownify %}
<div class="section blue">{{ content }}</div>

{% capture section %}
## [Global Brightness](global_brightness_menu.html)
The Global Brightness menu allows you to adjust the overall brightness of your Vortex Device. This can be useful for saving battery life or adjusting the intensity of your light shows.
{% endcapture %}
{% assign content = section | markdownify %}
<div class="section yellow">{{ content }}</div>

{% capture section %}
## [Factory Reset](factory_reset_menu.html)
The Factory Reset menu allows you to reset your Vortex Device to its factory settings. This can be useful if you want to start fresh or if you're experiencing issues with your device.
{% endcapture %}
{% assign content = section | markdownify %}
<div class="section red">{{ content }}</div>

