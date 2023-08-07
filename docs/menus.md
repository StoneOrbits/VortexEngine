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

.section h2 {
    margin-top: 0;
}

.white h2  { color: rgba(255, 255, 255, 0.6); }
.cyan h2   { color: rgba(0, 255, 255, 0.6); }
.purple h2 { color: rgba(128, 0, 128, 0.6); }
.green h2  { color: rgba(0, 128, 0, 0.6); }
.blue h2   { color: rgba(0, 0, 255, 0.6); }
.yellow h2 { color: rgba(255, 255, 0, 0.6); }
.red h2    { color: rgba(255, 0, 0, 0.6); }

.content {
    padding: 10px;
    background: linear-gradient(to bottom, rgba(255, 255, 255, 0.3), transparent);
}
.white .content  { background: linear-gradient(to bottom, rgba(255, 255, 255, 0.3), transparent); }
.cyan .content   { background: linear-gradient(to bottom, rgba(0, 255, 255, 0.3), transparent); }
.purple .content { background: linear-gradient(to bottom, rgba(128, 0, 128, 0.3), transparent); }
.green .content  { background: linear-gradient(to bottom, rgba(0, 128, 0, 0.3), transparent); }
.blue .content   { background: linear-gradient(to bottom, rgba(0, 0, 255, 0.3), transparent); }
.yellow .content { background: linear-gradient(to bottom, rgba(255, 255, 0, 0.3), transparent); }
.red .content    { background: linear-gradient(to bottom, rgba(255, 0, 0, 0.3), transparent); }
</style>

# Menus

Each of these menus serves a specific purpose and allows you to customize your Vortex Device in different ways. When you open the menus on any Vortex Device, whichever mode slot was selected and playing when you open the menus will be targeted as the mode to be configured by whichever menu you may enter.

Here's a brief overview of what each menu does:

<div class="section white">
{% capture section %}
## [Randomizer](randomizer_menu.html)
{% endcapture %}
{% assign title = section | markdownify %}
{{ title }}
<div class="content">
The Randomizer menu allows you to randomize the settings of your current mode. This can be a fun way to discover new combinations of patterns and colorsets.
</div>
</div>

<div class="section cyan">
{% capture section %}
## [Mode Sharing](mode_sharing_menu.html)
{% endcapture %}
{% assign title = section | markdownify %}
{{ title }}
<div class="content">
The Mode Sharing menu allows you to share your current mode with another Vortex Device. This is a great way to quickly and easily share your favorite modes with friends.
</div>
</div>

<div class="section purple">
{% capture section %}
## [Editor Connection](editor_connection_menu.html)
{% endcapture %}
{% assign title = section | markdownify %}
{{ title }}
<div class="content">
The Editor Connection menu is used to connect your Vortex Device to the Vortex Editor. This allows you to program your device using the editor's user-friendly interface. Please note that this menu is not available on the Duo.
</div>
</div>

<div class="section green">
{% capture section %}
## [Color Select](color_select_menu.html)
{% endcapture %}
{% assign title = section | markdownify %}
{{ title }}
<div class="content">
The Color Select menu allows you to change the colorset of the current mode. This is a quick and easy way to customize the appearance of your modes.
</div>
</div>

<div class="section blue">
{% capture section %}
## [Pattern Select](pattern_select_menu.html)
{% endcapture %}
{% assign title = section | markdownify %}
{{ title }}
<div class="content">
The Pattern Select menu allows you to change the pattern of the current mode. This allows you to customize the blinking speed or style of your modes.
</div>
</div>

<div class="section yellow">
{% capture section %}
## [Global Brightness](global_brightness_menu.html)
{% endcapture %}
{% assign title = section | markdownify %}
{{ title }}
<div class="content">
The Global Brightness menu allows you to adjust the overall brightness of your Vortex Device. This can be useful for saving battery life or adjusting the intensity of your light shows.
</div>
</div>

<div class="section red">
{% capture section %}
## [Factory Reset](factory_reset_menu.html)
{% endcapture %}
{% assign title = section | markdownify %}
{{ title }}
<div class="content">
The Factory Reset menu allows you to reset your Vortex Device to its factory settings. This can be useful if you want to start fresh or if you're experiencing issues with your device.
</div>
</div>

