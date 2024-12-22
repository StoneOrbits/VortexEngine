---
layout: default
title: Spark Upgrade Guide
nav_order: 5
parent: Upgrade Guides
---

# Spark Upgrade Guide

The latest firmware for your device can be found on the Vortex Community [downloads page](https://vortex.community/downloads).

First, connect your device to the PC via USB.

<div style="margin-left: 50px; margin-right: 50px" markdown="1">

- **Use a data-capable USB cable, power-only cables won't work.**
- Windows will notify upon successful connection
- Mac may work but is not officially supported.

</div>

![Device Buttons](assets/images/device-spark-buttons.png)

Next, double click the <span style="color: #ff0000">reset button</span> and a new drive should appear, this may take a few tries.

<div style="text-align: center; margin: 20px">
  <img src="assets/images/trinketboot-drive.png" style="box-shadow: 2px 2px 10px rgba(0, 0, 0, 0.2); border-radius: 5px;">
</div>

Finally, Drag & Drop the device firmware into the Trinketboot drive to automatically upgrade.

As long as the file transfer completes, the firmware should be updated.

## Troubleshooting

**The drive doesn't show up**
Most likely you are not using a data-capable usb cable, try different usb cables and different usb ports on the PC.

**The file transfer freezes**
Some pcs have a rare issue where the file transfer will freeze, the only known workaround is to use a different PC.

**The file inside the trinketboot drive doesn't have a new name**
This is normal, copying the firmware to the drive is not like a normal file copy. Opening the drive as a folder will always show the same file.
