---
layout: default
title: Duo Upgrade Guide
nav_order: 3
parent: Upgrade Guides
---

<div style="text-align: center; margin: 20px">
  <h1 style="color:orange;">⚠ <b style="color:red;">Warning</b> ⚠</br></h1>
  <h2>Upgrading Duos <i>can</i> be dangerous! Be patient and following these steps carefully, StoneOrbits is not responsible for any damage</h2>
</div>

# Duo Upgrade Guide

<img src="assets/images/duo-logo-square-512.png" style="max-width: 250px; width: 40%; float: right">

To update your Duo you will need a Chromadeck and a Chromalink cable.

Before updating the Duo, connect the **Chromadeck** to the PC via USB and make sure it is [up to date](chromadeck_upgrade_guide.html)

<div style="margin-left: 50px; margin-right: 50px; margin-bottom: 30px;" markdown="1">

- **Use a data-capable USB cable, power-only cables won't work.**
- Windows will notify upon successful connection
- Mac may work but is not officially supported.

</div>

 - First, plug the **Chromadeck** into the PC and connect the **Chromalink** cable to the side of the **Chromadeck**, and turn the Chromadeck power switch on
&nbsp;

 - **Gently** and **carefully** peel the Duo battery away from the Duo like shown below, failure to remove will degrade the battery

<div style="text-align: center; margin: 5px">
  <p style="color:white;">⚠ <b>Warning</b> ⚠<br>
  Damage to the Duo or battery is not covered by warranty!</p>
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/battery-removal-1.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/battery-removal-2.gif">
</div>

 
 - Slot the Duo into the **Chromalink** by seating it flat in the center then sliding it into place like shown below

<div style="text-align: center; margin: 20px">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/insert-dock-1.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/insert-dock-2.gif">
</div>

 - **[Optional]** Turn the Duo on to verify it is connected properly

 - Navigate a **Google Chrome** browser to [lightshow.lol](https://lightshow.lol) and click the 'Connect Device' USB icon:

<div style="text-align: center; margin: 20px">
  <img style="max-width:400px;" src="assets/images/connect-device.png">
</div>

 - Chrome will open a new popup window, select **USB JTAG/serial debug unit** from the list

<div style="text-align: center; margin: 20px">
  <img style="max-width:400px;" src="assets/images/connect-chromadeck-serialport.png">
</div>

 - Now the UI should look like this, press **Update Firmware** in the **Chromalink Duo Panel**:

<div style="text-align: center; margin: 20px">
  <a style="color: white;"><b>NOTE</b>: It is unnecessary to press Connect Duo!</a>
  <img style="margin-top:10px;" src="assets/images/update-firmware-ui.png">
</div>

 - Wait for the firmware flash to complete
 
 - Remove the Duo from the Chromalink and reconnect the battery
 
 - Turn the Duo on and cycle through all of it's modes to verify the flash was successful

## Troubleshooting

**The firmware flash doesn't start and the Chromadeck is yellow**  

This is a well known and common issue we are working to fix, simply remove your duo from the holder and re-seat it in the holder and the firmware flash will begin

**It doesn't work!**  

If you encounter any other issues, try refreshing to page and unplugging and reconnecting your chromadeck. Although it shouldn't be necessary, make sure the power switch is in the ON position while it is plugged in.

**It still doesn't work!**  
Join our [discord](https://discord.gg/4R9at8S8Sn) and ask for help!