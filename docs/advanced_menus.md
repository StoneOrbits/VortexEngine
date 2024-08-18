---
layout: default
title: Advanced Menus
nav_order: 8
parent: Menus
grand_parent: Vortex Engine
---

<style>
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
</style>

# Advanced Menus

Some devices have additional menu features that are disabled by default.

For example:
- <span class="rounded-box white"></span>[The Randomizer](randomizer_menu.html) can randomize just the colorset, just the pattern, or both.
- <span class="rounded-box red"></span>[Factory Reset](randomizer_menu.html) can restore just a specific mode or LED to factory default.
- The Duo has other advanced menus explained in its usage guide.

To enable **advanced menu features**:

1. Enter the **menu selection** but don't enter a menu.
2. **Rapidly Short Click at least 10 times** until you see a noticeable <span style="font-weight:bold;color:pink;">solid pink</span> flash.

Once enabled, advanced menus are accessible by **holding down on the chosen menu color a bit longer**.  

For example, enter the **Advanced Randomizer** by holding down on **white** till it strobes faster, then release.