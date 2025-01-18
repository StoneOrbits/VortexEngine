---
layout: default
title: Upgrade Guide
nav_order: 1
parent: Chromalink Guide
---

# Updating the Duo

Now that the Duo is connected to the PC via the Chromalink, follow these steps to update the Duo firmware:

{: .note }
At the time of writing this **Chrome**, **Edge** and **Opera** are the only supported browsers, other browsers may work in the future if they gain support for <a href="https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API#browser_compatibility">WebSerial</a>

 - Navigate a supported browser to [lightshow.lol](https://lightshow.lol) and click the **Connect Device** USB icon:

 - Chrome will open a new popup window, select **USB JTAG/serial debug unit** from the list

<div style="text-align: center; margin: 20px">
  <img src="assets/images/connect-chromadeck-serialport.png">
</div>

 - With the **Chromadeck** connected, two new panels will appear: **Device Updates** & **Chromalink Duo**

<div style="text-align: center; margin: 5px"> 
  <img style="max-width:280px;" src="assets/images/lightshow-lol-chromadeck-update-done.png">
  <img style="max-width:260px;max-height:190px;" src="assets/images/lightshow-lol-chromalink-duo-basic.png">
</div>


 - If you haven't <a href="chromadeck_upgrade_guide.html">updated the Chromadeck</a> yet, do it <b style="color:white;">now</b>!
 
 - If the **Chromadeck** is up to date, then press the **Update Firmware** button in the **Chromalink Duo Panel**:

{: .note }
It is **unnecessary** to press 'Connect Duo' when updating the Firmware! That is only needed for programming the Duo.

<div style="text-align: center; margin: 20px">
  <img style="max-width:260px;" src="assets/images/update-firmware-ui.png">
  <img style="max-width:260px;" src="assets/images/update-firmware-ui-progress.png">
</div>
  
Both the blue progress bar and the Chromadeck leds will fill as the flashing takes place

 - Let the flash complete, after completion the Duo should be able to turn on with a button press

 - If it powers on successfully then the firmware update was complete, if not try repeating this process

<div style="text-align: center; margin-top: 30px;">
  <p><h3 style="color: white;">Congratulations</h3>The Duo firmware is now updated, try <a href="duo_programming_guide.html">programming the Duo</a> or continue reading to reassemble the Duo</p>
</div>

# Duo Reassembly

- Remove the **Duo** from the **Chromalink** and reconnect the battery. Make sure the corners of the connector are flush for a reliable connection.
- Click the button to power on the Duo and verify the battery is fully connected, if it doesn't work then check the battery connector.

<div style="text-align: center; margin: 20px">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/duo-battery-connect.gif">
  <img style="max-width:260px;border-radius:5px;border:1px solid gray;" src="assets/images/duo-case-insertion.gif">
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
