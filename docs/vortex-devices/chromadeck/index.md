---
layout: default
title: Chromadeck
nav_order: 4
parent: Vortex Devices
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

<img align="right" width="" height="220" class="device-icon" src="{{ '/assets/images/device-chromadeck-square-512.png' | relative_url }}">

# Chromadeck

Welcome to the Chromadeck Guide, this page covers the basic features of the Chromadeck.

On the front of the Chromadeck are three <strong style="color: #00c6ff">navigation buttons</strong>, and a small <strong style="color: #00ff00;">power switch</strong>.

On the notched left side of the Chromadeck is a <strong style="color: #ff4646;">USB-C Port</strong> for charging and PC connectivity.  On the opposite notch is the <strong style="color: #c446ff;">Chromalink Port</strong>.

## The Chromalink

The [Chromalink]({% link vortex-devices/chromadeck/chromalink.md %}) is the primary accessory of the Chromadeck that enables updating and programming the [Duo]({% link vortex-devices/duo/index.md %}) from a PC.

{: .warning }
Please follow the [Chomadeck Update Guide]({% link vortex-devices/chromadeck/upgrade-guide.md %}) before using the Chromalink!

With an updated Chromadeck the [Duo Chromalink Guide]({% link vortex-devices/duo/programming-guide/chromalink-guide/index.md %}) will explain how to get started using the Chromalink with a Duo.

Alternatively, continue reading to learn how to use the Chromadeck as a mode storage bank or mode transfer utility.

## Basic Usage

When the Chromadeck is not connected to the PC it can be used just like any other Vortex Device, it can play modes and offers the same menus as other devices.

When the Chromadeck is first powered on it will display a rainbow gradient around it's leds, this is the one special part about a Chromadeck that no other device has known as **Profile Selection**.

### Profile Selection

- When powered on, the Chromadeck will always begin with profile selection. 
- The Chromadeck has 10 profiles, each with 14 mode slots. 

### Buttons
- First use the left and right <span style="color: #00c6ff">navigation buttons</span> to highlight a profile, and the middle button to select it. 
- Short click the left and right <span style="color: #00c6ff">navigation buttons</span> to cycle through the 14 modes slots of the select profile.
- Navigate to a mode then hold the <span style="color: #00c6ff">navigation button</span> until the lights flash white to open the menus.

## Menus

Once the menus are open, use short clicks to cycle through the following menu options, long click to enter a menu.

<div class="menu-wrapper">
<a href="{% link vortex-engine/menus/randomizer.md %}">
  <div class="color-list-entry"><span class="rounded-box white"></span>Randomizer</div>
</a>

<a href="{% link vortex-engine/menus/mode-sharing/index.md %}">
  <div class="color-list-entry"><span class="rounded-box cyan"></span>Mode Sharing</div>
</a>

<a href="{% link vortex-engine/menus/color-select.md %}">
  <div class="color-list-entry"><span class="rounded-box green"></span>Color Select</div>
</a>

<a href="{% link vortex-engine/menus/pattern-select.md %}">
  <div class="color-list-entry"><span class="rounded-box blue"></span>Pattern Select</div>
</a>

<a href="{% link vortex-engine/menus/global-brightness.md %}">
  <div class="color-list-entry"><span class="rounded-box yellow"></span>Global Brightness</div>
</a>

<a href="{% link vortex-engine/menus/factory-reset.md %}">
  <div class="color-list-entry"><span class="rounded-box red"></span>Factory Reset</div>
</a>
</div>

## Mode Storage

The Chromadeck can be used to store a large number of modes in an organized collection. 

## Mode Transfers

The Chromadeck also acts as a mode transfer utility, offering all forms of mode transfer for Vortex Device it can be used to facility a mode transfer between devices that previously were not supported. For example a Duo mode could be transferred to a Vortex Orbit using the Chromadeck.

---

## Next Steps
<div class="next-steps">
  <ul>
    <li><a href="{% link vortex-engine/basic-usage.md %}">Explore Vortex Engine</a>: Learn key terms, concepts, and how to use on-device menus.</li>
    <li><a href="{% link vortex-devices/chromadeck/upgrade-guide.md %}">Upgrade Your Device</a>: Ensure your device has the latest firmware for optimal performance.</li>
  </ul>
</div>

---

If you have any questions or need further assistance, check out the [Support Page]({% link support/index.md %}).
