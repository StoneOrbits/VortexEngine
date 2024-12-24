---
layout: default
title: Spark Upgrade Guide
nav_order: 5
parent: Upgrade Guides
---

# Spark Upgrade Guide

<img src="assets/images/spark-logo-square-512.png" style="max-width: 250px; width: 40%; float: right">

This page details how to update the firmware on the Vortex Spark.

It is important to keep the Spark up to date to avoid bugs and use the latest safety fixes.

<div style="margin-left: 50px; margin-right: 50px; margin-bottom: 30px;" markdown="1">

- **Use a data-capable USB cable, power-only cables won't work.**
- Windows will notify upon successful connection
- Mac may work but is not officially supported.

</div>

 - With the Spark plugged in, navigate a **Google Chrome** browser to [lightshow.lol](https://lightshow.lol) and click the **Connect Device** USB icon:

<div style="text-align: center; margin: 20px">
  <p style="color:white;"><b>Note:</b>
  Other browsers may work if they support <a href="https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API#browser_compatibility">WebSerial</a></p>
  <img src="assets/images/connect-device.png">
</div>

 - Chrome will popup a new window, select 'USB JTAG/serial debug unit' from the list

<div style="text-align: center; margin: 20px">
  <img src="assets/images/connect-chromadeck-serialport.png">
</div>

 - Once the spark is connected the UI should look like this, simply hit 'Update Firmware Now' in the Device Updates panel:

<div style="text-align: center; margin: 20px; display:flex; flex-direction: column; align-items: center;">
  <p style="color: white;"><b>NOTE</b>: Ignore the 'Update Firmware' in the Chromalink Duo panel!</p>
  <img src="assets/images/spark-firmware-update.png">
</div>

 - Let the firmware flash complete, unplug the Spark, then flip the power switch to check that it turns on
 
 - Cycle through all of the modes to verify the flash was successful

## Troubleshooting

**It doesn't work!**
If you encounter any other issues, try refreshing to page and unplugging and reconnecting your spark. Although it shouldn't be necessary, make sure the power switch is in the ON position while it is plugged in.

**It still doesn't work!**
Join our [discord](https://discord.gg/4R9at8S8Sn) and ask for help!
