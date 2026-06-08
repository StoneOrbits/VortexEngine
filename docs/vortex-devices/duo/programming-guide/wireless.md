---
layout: default
title: Wireless Programming
parent: Programming Guide
nav_order: 1
---

# Wireless Programming

Wireless programming uses the Duo’s built-in Mode Sharing feature.

There are two methods for sending a mode wirelessly:

1. Transfer a mode already stored on your Chromadeck using its built-in Mode Sharing menu
2. Use Lightshow.lol with a connected Chromadeck and transmit the mode directly from the website

To access the Chromadeck’s built-in Mode Sharing menu, open it in the same way you would on the Duo.

To use Lightshow.lol for wireless transfer, continue to the next section.

In both cases, only one mode can be transferred at a time.

## Lightshow.lol on PC

Only browsers that support the **Web Serial API** can connect to the Chromadeck. At present, this includes Google Chrome and a limited number of other browsers.

See browser compatibility details here:

https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API#browser_compatibility

If you have not previously connected your Chromadeck to Lightshow.lol, follow the [Chromadeck Upgrade Guide]({{ site.baseurl }}{% link vortex-devices/chromadeck/upgrade-guide.md %}) 

Then return here.

To program a new mode from Lightshow.lol on PC, follow these steps:

1. Connect your Chromadeck to Lightshow.lol
2. Select the mode you want to transfer (import, edit, or create one)
3. Open the Duo’s Mode Sharing menu and ensure it is ready to receive
4. Point the Chromadeck LEDs toward the Duo receiver (button area)
5. While pointing the Chromadeck at the Duo, press **Transmit** in the upper-right corner of the Modes Panel on Lightshow.lol
6. Wait for the transfer to complete, or repeat from step 4 if it fails

The selected mode is sent from Lightshow.lol, through the Chromadeck, and wirelessly to the Duo.

This process can be performed with multiple Duos at the same time.

## Lightshow.lol on Android

Lightshow.lol can also connect to a Chromadeck over Bluetooth from supported Android devices.

This allows you to:

- Create and edit modes on your Android device
- Send modes to the Chromadeck
- Wirelessly transfer those modes to a Duo through Mode Sharing

The process is similar to the PC approach outlined above, and the mobile layout of Lightshow.lol is quite straightforward.

To learn more about Lightshow.lol on Android, see the Android documentation.
