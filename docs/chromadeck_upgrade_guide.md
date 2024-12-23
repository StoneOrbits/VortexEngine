---
layout: default
title: Chromadeck Upgrade Guide
nav_order: 4
parent: Upgrade Guides
---

# Chromadeck Upgrade Guide

<img src="assets/images/chromadeck-logo-square-512.png" style="max-width: 250px; width: 40%; float: right">

This page covers how to update the firmware on the Vortex Chromadeck.

<div style="margin-left: 50px; margin-right: 50px" markdown="1">

- **Use a data-capable USB cable, power-only cables won't work.**
- Windows will notify upon successful connection
- Mac may work but is not officially supported.

</div>

First, navigate a **Google Chrome** browser to https://lightshow.lol

 - With the Chromadeck plugged in via USB, click the 'Connect Device' USB icon:

<div style="text-align: center; margin: 20px">
  <img style="max-width:400px;" src="assets/images/connect-device.png">
</div>

 - Chrome will popup a new window, select 'USB JTAG/serial debug unit' from the list

<div style="text-align: center; margin: 20px">
  <img style="max-width:400px;" src="assets/images/connect-chromadeck-serialport.png">
</div>

 - Once the chromadeck is connected the UI should look like this, simply hit 'Update Firmware Now' in the Device Updates panel:

<div style="text-align: center; margin: 20px">
  <p style="color: white;"><b>NOTE</b>: Ignore the 'Update Firmware' button in the Chromalink Duo panel!
  <img style="margin-top:10px;" src="assets/images/chromadeck-firmware-update.png">
</div>

 - Wait for the firmware flash to complete
 
 - Unplug the Chromadeck and cycle the power switch to verify it turns on
 
 - Cycle through all of the modes to verify the flash was successful

## Troubleshooting

**It doesn't work!**
If you encounter any other issues, try refreshing to page and unplugging and reconnecting your chromadeck. Although it shouldn't be necessary, make sure the power switch is in the ON position while it is plugged in.

**It still doesn't work!**
Join our [discord](https://discord.gg/4R9at8S8Sn) and ask for help!