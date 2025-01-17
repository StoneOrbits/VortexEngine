---
layout: default
title: Duo Upgrade Guide
nav_order: 2
parent: Duo Guide
---
<style>
.warning-box {
   background-color: rgba(255,255,0,.25);
   border: 1px solid #ddd;
   border-radius: 8px;
   padding: 15px;
   margin: 20px 0;
   box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
}
.info-box strong {
   color: #0056b3;
}
</style>

<div class= warning-box style="text-align: center; margin: 20px">
  <h1 style="color:orange;">⚠ <b style="color:red;">Warning</b> ⚠<br></h1>
  <h2>Upgrading Duos <i>can</i> cause damage to them if you do not follow instructions!  
  Be patient, careful, and double-check instuctions before proceeding. 
  
  StoneOrbits is not responsible for damage caused this way.</h2>
</div>

---

# Duo Upgrade Guide

<img src="assets/images/duo-logo-square-512.png" style="max-width: 250px; width: 40%; float: right; margin-bottom: 20px">

Both a **Chromadeck** and a **Chromalink** cable are necessary to update the Duo.

Before updating Duo firmware, always ensure the **Chromadeck** is [up to date](chromadeck_upgrade_guide.html)
 first.
<div style="margin-left: 50px; margin-right: 50px; margin-bottom: 30px;" markdown="1">

- **Use a data-capable USB cable, power-only cables won't work.**
- Windows will notify upon successful connection
- Mac may work but is not officially supported.
</div>

With your Duo, Chromadeck, Chromalink, Computer, and USB-C cable ready, proceed to the next step.

---

**Remove the Casing**

 - Start by carefully removing the Duo from it's case. It should come out with little resistance.
 - If you find it does not come out easily, check to see if the bulb is caught on the opening as shown in the 2nd gif.

<div style="text-align: center; margin: 5px">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/duo-case-removal.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/duo-case-removal-2.gif">
</div>

---

**Removing the Battery**


<div style="text-align: center; margin: 5px">
  <p style="color:white;"><span style="color:yellow;">⚠</span> <b>Warning</b> <span style="color:yellow;">⚠</span><br>
  <strong>This is a potentially dangeous step!</strong></p>
</div>

{: .warning}
Failure to remove the battery before connecting to the Chromalink will cause the battery's lifespan to shorten dramatically. 
Removing the battery incorrectly can break the battery connector, rendering the chip broken and unusable. 
StoneOrbits is not responsible for any damage to the Duo or battery caused this way.


- In order to safely update the Duo, the battery must be carefully disconnected first. 
- **Gently** and **peel** the Duo battery away from the Duo like shown below.
<div style="text-align: center; margin: 5px">  
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/battery-removal-1.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/battery-removal-2.gif">
</div>

---

**Setting up the Chromadeck and Chromalink**
 - Connect the **Chromalink** cable to the **Chromadeck**, then plug the **Chromadeck** into the PC and power it on.
<div style="text-align: center; margin: 5px">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/connect-chromalink.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/connect-chromadeck.gif">
</div>

---

**Connecting the Duo to the Chromalink**
 - Seat the Duo into the **Chromalink** dock and slide it into place like shown below

<div style="text-align: center; margin: 20px">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/insert-dock-1.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/insert-dock-2.gif">
</div>

---

**Verify Connection**

 - To verify the Duo is securely connected to the Chromalink, press the button to turn it on.
 - If it does not turn on, review the previous steps to make sure there are no mistakes.
 --- 

 &nbsp;

**Updating the Duo**

 - Navigate a **Google Chrome** browser to [lightshow.lol](https://lightshow.lol) and click the **Connect Device** USB icon:

<div style="text-align: center; margin: 20px">
  <p style="color:white;"><b>Note:</b>
  Other browsers may work if they support <a href="https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API#browser_compatibility">WebSerial</a></p>
  <img src="assets/images/connect-device.png">
</div>

 - Chrome will open a new popup window, select **USB JTAG/serial debug unit** from the list

<div style="text-align: center; margin: 20px">
  <img src="assets/images/connect-chromadeck-serialport.png">
</div>

 - With the **Chromadeck** connected to lightshow.lol, two new panels will be visible: **Device Updates** & **Chromalink Duo**

 <div style="text-align: center; margin: 5px">
  <b style="color:white;">Note: </b> If you haven't <a href="chromadeck_upgrade_guide.html">updated the Chromadeck</a> yet, do it <b style="color:white;">now</b>!</p>
 
  <img style="max-width:280px;" src="assets/images/lightshow-lol-chromadeck-update-done.png">
  <img style="max-width:260px;max-height:190px;" src="assets/images/lightshow-lol-chromalink-duo-basic.png">
</div>
 
- If the **Chromadeck** firmware is up to date, then press the **Update Firmware** button in the **Chromalink Duo Panel** to update the Duo:

<div style="text-align: center; margin-top: 20px;">
  <p><b style="color: white;">Note</b>: It is unnecessary to press Connect Duo when updating Firmware!</p>
</div>
<div style="text-align: center; margin: 20px">
  <img style="max-width:260px;" src="assets/images/update-firmware-ui.png">
  <img style="max-width:260px;" src="assets/images/update-firmware-ui-progress.png">
</div>

 - Let the flash complete.

 - After completion, pressing the button should turn the Duo on.

 - If it powers on successfully, proceed to the next step or go to the [Chromalink Guide](chromalink_guide.html) to see how to use your Duo with Lightshow.lol
 
 ---

 **Puting everything Back Together**

- remove the **Duo** from the **Chromalink** and reconnect the battery. Make sure the corners of the connector are flush for a reliable connection.
- click the button to power on the Duo and verify the battery is fully connected.

- If it works, insert it back into the case. Otherwise recheck the battery connector.

<div style="text-align: center; margin: 20px">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/duo-battery-connect.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/duo-case-insertion.gif">
</div>

<div style="text-align: center; margin-top: 30px;">
  <p><b style="color: white;">Congratulations</b> You have successfully updated the Duo Firmware!</p>
</div>

---

# Troubleshooting

**The Duo doesn't turn on**  
Repeat the process if the Duo doesn't work, make sure the duo was properly seated into the Chromalink and the progress bar in the Chromalink window fills.

**The progress bar doesn't start and the Chromadeck LEDs are yellow**  

This is a well known and common issue we are working to fix, simply remove the Duo from the Chromalink dock and put it back, the firmware flash will begin immediately.

**It doesn't work!**  

If you encounter any other issues, try refreshing the page and unplugging and reconnecting your chromadeck. Also make sure the power switch is in the ON position while it is plugged in.

**It still doesn't work!**  
Join our [discord](https://discord.gg/4R9at8S8Sn) and ask for help!