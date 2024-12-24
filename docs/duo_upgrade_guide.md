---
layout: default
title: Duo Upgrade Guide
nav_order: 3
parent: Upgrade Guides
---

<div style="text-align: center; margin: 20px">
  <h1 style="color:orange;">⚠ <b style="color:red;">Warning</b> ⚠<br></h1>
  <h2>Upgrading Duos <i>can</i> be dangerous! Be patient and follow these steps carefully, StoneOrbits is not responsible for any damage</h2>
</div>

# Duo Upgrade Guide

<img src="assets/images/duo-logo-square-512.png" style="max-width: 250px; width: 40%; float: right; margin-bottom: 20px">

Both a **Chromadeck** and a **Chromalink** cable are necessary to update the Duo.

Before updating the Duo firmware, always ensure the **Chromadeck** is [up to date](chromadeck_upgrade_guide.html)

<div style="margin-left: 50px; margin-right: 50px; margin-bottom: 30px;" markdown="1">

- **Use a data-capable USB cable, power-only cables won't work.**
- Windows will notify upon successful connection
- Mac may work but is not officially supported.

</div>

 - Plug the **Chromadeck** into the PC, turn the power switch on, and connect the **Chromalink** cable to the side of the **Chromadeck**
&nbsp;

 - **Gently** and **carefully** peel the Duo battery away from the Duo like shown below, leaving it connected will degrade the battery

<div style="text-align: center; margin: 5px">
  <p style="color:white;">⚠ <b>Warning</b> ⚠<br>
  StoneOrbits is not responsible for damage to the Duo or battery!</p>
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/battery-removal-1.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/battery-removal-2.gif">
</div>

 - Seat the Duo into the **Chromalink** dock and slide it into place like shown below

<div style="text-align: center; margin: 20px">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/insert-dock-1.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/insert-dock-2.gif">
</div>

 - **[Optional]** Turn the Duo on to verify it is receiving power from the Chromadeck

 - Navigate a **Google Chrome** browser to [lightshow.lol](https://lightshow.lol) and click the 'Connect Device' USB icon:

<div style="text-align: center; margin: 20px">
  <p style="color:white;"><b>Note:</b>
  Other browsers may work if they support <a href="https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API#browser_compatibility">WebSerial</a></p>
  <img src="assets/images/connect-device.png">
</div>

 - Chrome will open a new popup window, select **USB JTAG/serial debug unit** from the list

<div style="text-align: center; margin: 20px">
  <img src="assets/images/connect-chromadeck-serialport.png">
</div>

 - When the Chromadeck connects the **Chromalink Duo** panel will appear, press the **Update Firmware** button to update the Duo:

<div style="text-align: center; margin: 20px; display:flex; flex-direction: column; align-items: center;">
  <p style="color: white;"><b>NOTE</b>: It is unnecessary to press Connect Duo when updating Firmware!</p>
  <img src="assets/images/update-firmware-ui.png">
</div>

 - Let the flash complete then remove the Duo from the Chromalink and reconnect the battery
&nbsp; 
 
 - Turn the Duo on and cycle through all of it's modes to verify the flash was successful

# Troubleshooting

**The Duo doesn't turn on**  
Repeat the process if the Duo doesn't work, make sure the duo was properly seated into the Chromalink and the progress bar in the Chromalink window fills

**The progress bar doesn't start and the Chromadeck LEDs are yellow**  

This is a well known and common issue we are working to fix, simply remove the Duo from the Chromalink dock and put it back, the firmware flash will begin immediately.

**It doesn't work!**  

If you encounter any other issues, try refreshing the page and unplugging and reconnecting your chromadeck. Also make sure the power switch is in the ON position while it is plugged in.

**It still doesn't work!**  
Join our [discord](https://discord.gg/4R9at8S8Sn) and ask for help!