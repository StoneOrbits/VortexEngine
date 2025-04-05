---
layout: default
title: Getting Started with Lightshow.lol 
nav_order: 0
parent: Lightshow.lol
has_children: true
---

<style>
.image-container {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 15px;
  margin: 0 auto;
  padding: 0 15px;
  margin-top: 20px;
}

.image-container img {
  /* Adjust max-width as needed for your layout */
  max-width: 20%;
  height: auto;
  display: block;
}
</style>


# Using a Vortex Device with Lightshow.lol

Welcome to Lightshow.lol, our powerful Vortex Engine mode designing tool. If you’re new or recently acquired a vortex device then you might be wondering:
- *How does this work?*
- *How do I get started?*

This guide will walk you through the basics and help familiarize you with the platform.

## **Connecting Your Device**
While it’s not necessary to connect a device to Lightshow.lol to start designing new modes, chances are you already own a **Vortex Device** and are eager to start customizing it! 

1. Find a **data-capable USB cable**.
2. Use it to connect your device to your computer.

<div class="image-container">
    <img src="assets/images/vortex-device.png" alt="Vortex Device">
    <img style="max-width:30%" src="assets/images/USB-Cable.png" alt="USB Cable">
    <img src="assets/images/computer-icon.png" alt="Computer">
</div>

## **Opening the Device Control Panel**
Once connected, follow these steps:

<div style="display: flex; align-items: center; justify-content: space-between; gap: 20px; flex-wrap: wrap; margin-top: 20px;">
  <div style="flex: 1; font-size: 1.1em;">
    <p>Go to the <a href="lightshow_lol_device_controls.html">Device Control Panel</a> in the <b>upper right corner</b> of Lightshow.lol.</p>
    <ul>
      <li>No need to select a device first!</li>
      <li>Simply <b>click the USB button</b> on the right side of this panel to open the device port list.</li>
    </ul>
  </div>
  <img style="width: 35%; height: auto; min-width: 250px;" src="assets/images/lightshow-lol-device-panel-connect.png" alt="Device Panel Connect">
</div>


## **Selecting the Correct Port**
<div style="display: flex; align-items: center; justify-content: space-between; gap: 20px; flex-wrap: wrap; margin-top: 20px;">
  <img style="width: 35%; height: auto; min-width: 250px;" src="assets/images/lightshow-lol-ports.png" alt="Device Panel Ports">
  <div style="flex: 1; font-size: 1.1em;">
    <p>In the <b>device port list</b>, you may see multiple ports:</p>
    <ul>
      <li>Select the one called <b>"JTAG/serial debug unit (COM__)"</b></li>
      <li>Click <b>Connect</b> to proceed.</li>
      <li>The number after COM will be different for each user.</li>
    </ul>
  </div>
</div>


## **Successful Connection & Firmware Update**
If everything is set up correctly, you should see a **successful connection notification**, and the **firmware update panel** will open.

**Always update your firmware** if a newer version is available!

<div style="text-align: center; max-width: 700px; margin: 20px auto;">
  <div style="display: flex; flex-direction: column; align-items: center; gap: 15px;">
    <img height="50" src="assets/images/Successfully-connected.png" alt="Successful Connection">
    <img height="300" src="assets/images/spark-firmware-update.png" alt="Firmware Update Panel">
  </div>
</div>

## What to do next

Frome here, s common next step is to [load the savefile](pulling_modes.html) from your device to lightshow.lol in order to edit it.

If you want to start from scratch instead you can try [creating new modes](creating_modes.html).

Instead of creating your own modes, there are many [community built modes](community.html) you can choose from to add to your device.

Try any combination of these options to create the ultimate device for you!

