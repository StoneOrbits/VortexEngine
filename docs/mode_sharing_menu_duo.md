---
layout: default
title: Sharing on Duos
nav_order: 1
parent: Mode Sharing
grand_parent: Menus
---

# Mode Sharing on Duo

The Duo supports wireless Mode Sharing using **Visible Light only** — it does **not have Infrared**. This allows modes to be sent between Duos or to a [Chromadeck](chromadeck_guide.html) or [Spark Orbit](spark_orbit_guide.html) / [Handle](spark_handle_guide.html) using visible light pulses from the LEDs.

In **May 2025**, the Visible Light Mode Sharing protocol was upgraded. All Duos manufactured after this date use the **new protocol**, which includes:

- Faster transfer speeds with more reliability
- Stronger error detection and faster recovery
- Enhanced user interface (continuous send while holding)

Although the new protocol is incompatible with the old one, a **Legacy Mode Sharing** option is still available in the new version. 

---

## Overview

Mode Sharing uses rapid LED blinks to encode and transmit data. Devices must be aligned so the sender's LED is visible to the receiver's light sensor.

This feature transfers the current mode onto another Duo (or Chromadeck, etc) wirelessly.0.

---

## Menu Options

Once in the Mode Sharing menu (cyan), there are **three** options to cycle through with **short clicks**:

The menu will start on <span style="color:rgb(0, 255, 155);">teal</span> which is the modern Mode Sharing option, the legacy option is second, and exit is last.

- **Send / Receive** (<span style="color:rgb(0, 255, 155);">teal</span>) – The new 2025 protocol
- **Send / Receive Legacy** (<span style="color:rgb(255, 255, 255);">white</span>) – for compatibility with pre-2025 Duos
- **Exit Mode Sharing** (blinking <span style="color:rgb(255, 0, 0);">red</span>)

**Long click** on any option to activate it.

---

## Sending a Mode

1. **Choose a Mode**  
   Navigate to the mode you want to send and hold the button until LEDs flash white.

2. **Enter Mode Sharing Menu**  
   Cycle to <span style="color: #00ffff;">cyan</span> and long click.

3. **Broadcast**  
   Touch the sending duos led to the receiver button and **hold the sender button** to continuously send.

4. **Finish**  
   Once done, short click to move to **Exit** (red blinking), then long click to leave.

---

## Receiving a Mode

1. **Select Slot to Overwrite**  
   Navigate to the slot you want to overwrite and hold until LEDs flash white.

2. **Enter Mode Sharing Menu**  
   Cycle to <span style="color: #00ffff;">cyan</span> and long click.

3. **Wait for Transmission**  
   Hold the receiving Duo with it's button touching the sender led while the sender transmits

4. **Finish**
   Upon successfully receiving the mode the Mode Sharing menu closes and the new mode begins playing

---

## Tips for Best Results

- **Touch Sender to Receiver**: The transfers often work best when the sender Led is touching the receiver button area.
- **Remove Obstructions**: Although it should work with diffusers and through fabrics, try removing these obstructions if you have issues.
- **Watch Receiver for Progress**: On the sender, hold the button to keep sending and pay attention to the receiver to see if it's making progress.

---

Need to transfer to older Duos? See [Legacy Mode Sharing →](mode_sharing_menu_legacy.md)





# Legacy Mode Sharing

This page is for older Duos purchased before **May 2025** that have **not yet been updated** to the new Mode Sharing system.

If a Duo only shows a dim white in the mode sharing menu then this is **legacy mode sharing**.

Legacy sharing is slower and less reliable than the updated version.

## How Legacy Mode Sharing Works

Devices must be **very close together**, with the sender LED touching the reciever light detector.  The receiver light detector is located immediately below the button, to line up the sender with receiver point the LED at the receiver's button.

## Sending a Mode (Legacy)

### Sending a Mode
1. **Pick a Mode to Share**: Navigate to the mode you want to share and hold the button until the leds flash white
2. **Enter Mode Sharing**: Cycle to <span style="color: #00ffff;">cyan</span> and long click to enter
3. **Switch to Sending Mode**: Perform a single click to start broadcasting the current mode.
4. **Exit Menu**: Long click when done to stop sending and exit the menu.

### Receiving a Mode
1. **Pick a Mode to Overwrite**: Navigate to the mode you want to overwrite and hold the button until the leds flash white
2. **Enter Mode Sharing**: Cycle to <span style="color: #00ffff;">cyan</span> and long click to enter
3. **Receive Mode**: Hold the sender device up to the receiver, with the navigation buttons facing each other, the receiver will automatically save and exit when a mode is successfully received