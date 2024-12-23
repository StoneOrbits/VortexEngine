---
layout: default
title: Duo Upgrade Guide
nav_order: 3
parent: Upgrade Guides
---

# Duo Upgrade Guide

<img src="assets/images/duo-logo-square-512.png" style="max-width: 250px; width: 40%; float: right">

To update your Duo you will need a Chromadeck and a Chromalink cable.

First, connect the Chromadeck to the PC via USB and make sure it is [up to date](chromadeck_upgrade_guide.html)

<div style="margin-left: 50px; margin-right: 50px" markdown="1">

- **Use a data-capable USB cable, power-only cables won't work.**
- Windows will notify upon successful connection
- Mac may work but is not officially supported.

</div>

Next, navigate a **Google Chrome** browser to https://lightshow.lol

 1. With the Chromadeck plugged in via USB, click the 'Connect Device' USB icon:

<div style="text-align: center; margin: 20px">
  <img style="max-width:400px;" src="assets/images/connect-device.png">
</div>

 2. Chrome will popup a new window, select 'USB JTAG/serial debug unit' from the list

<div style="text-align: center; margin: 20px">
  <img style="max-width:400px;" src="assets/images/connect-chromadeck-serialport.png">
</div>

 3. Once the chromadeck is connected the UI should look like this, simply hit 'Update Firmware' in the Chromalink panel:

<div style="text-align: center; margin: 20px">
  <a style="color: white;"><b>NOTE</b>: It is unnecessary to press Connect Duo!</a>
  <img style="margin-top:10px;" src="assets/images/update-firmware-ui.png">
</div>

 4. Wait for the firmware flash to complete
 
 5. Remove the Duo from the Chromalink and reconnect the battery
 
 6. Turn the Duo on and cycle through all of it's modes to verify the flash was successful

## Troubleshooting

**The firmware flash doesn't start and the Chromadeck is yellow**  

This is a well known and common issue we are working to fix, simply remove your duo from the holder and re-seat it in the holder and the firmware flash will begin

**It doesn't work!**  

If you encounter any other issues, try refreshing to page and unplugging and reconnecting your chromadeck. Although it shouldn't be necessary, make sure the power switch is in the ON position while it is plugged in.

**It still doesn't work!**
Join our [discord](https://discord.gg/4R9at8S8Sn) and ask for help!