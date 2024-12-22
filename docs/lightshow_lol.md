---
layout: default
title: Lightshow.lol
nav_order: 0
parent: Vortex Editors
---

# Lightshow.lol

{: .warning }
This page is under construction and will be ready soon

Welcome to the Vortex Engine wiki! This page details how to use the web-based Vortex Editor [lightshow.lol](https://lightshow.lol). 

## Basics

Lightshow.lol is an online tool that allows you to design, preview, and export custom [modes](modes.html) for any Vortex devices. 

The lightshow preview is the main focus of the page, with various control panels serving to modify the pattern, colorset, and preview animation.

{: .important }
Although the preview is an exact simulation of a real device, the preview will never look exactly the same as LEDs.

## Preview Animation

{: .important }
The animation panel only affects the virtual preview on screen, not the actual device!

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-animation.png">

The five animation shape options are self-explanatory, but the six slider options in this panel are:

- **Speed**: This represents how fast the animation will progress.
- **Trail**: This corresponds to how long the colors will remain visible (i.e., the length of the trail).
- **Size**: This is the size of each blink of color on the screen.
- **Blur**: This controls the appearance of the blink on the screen, increasing softness and diffusion.
- **Radius**: This controls the size of the animation pattern on the screen.
- **Spread**: With many LEDs, this controls the distance between LEDs in the preview.

## Device Controls

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-device.png">

The **Device Controls** panel is the most basic of panels, offering only two simple purposes: selecting or connecting a device.

- If a device is simply **selected**, the editor will allow creation of a mode tailored to that specific device (matching the number of LEDs).
- If a device is **connected** via USB, the editor will lock the device selection to the connected device type, unlocking further controls in the **Modes Panel** for communicating with the device.

In both cases—whether **Selecting** or **Connecting** a device—the **LED Selection** panel will appear, showing an image of the device and all of its LEDs.

## Modes Panel

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-modes.png">

The **Modes Panel** organizes and manages your lightshow's overall structure. A **mode** is a collection of patterns (one for each LED), which can be switched on the device.

### Key Features

- **Mode Creation:** Add, edit, or delete modes with ease.
- **Mode Preview:** Simulate switching between modes to see their effect in real-time.
- **Import/Export:** Import existing modes from your device or export them for sharing.
- **Sharing:** Quickly share your mode with the Vortex Community.

## LED Selection Panel

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-led-select.png">

The **LED Selection Panel** allows you to interact with and assign configurations to individual LEDs on your device.

### Key Features

- **Visual Interface:** View a layout of all LEDs on your device.
- **Individual Customization:** Select individual LEDs to assign patterns and colors.
- **Batch Selection:** Group LEDs for simultaneous modifications.

## Pattern Panel

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-pattern.png">

The **Pattern Panel** is where you design and assign animations for your LEDs. Patterns determine how LEDs blink or animate over time.

### Key Features

- **Pattern Library:** Choose from a library of predefined patterns (e.g., strobe, dops, blend).
- **Custom Parameters:** Adjust parameters like blink size, gap size, blend speed, etc

## Colorset Panel

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-colorset.png">

The **Colorset Panel** represents the list of colors for the selected leds of the current mode.

### Key Features

- **Drag and Drop** Drag to rearrange colors in your colorset.
- **Dynamic Color Picker:** Click to modify the color with the Color Picker

## Color Picker

<img style="float:right;max-width:40%;margin:10px;" src="assets/images/lightshow-lol-color-picker.png">

The **Color Picker** is a detailed interface for picking new colors for a colorset.

### Key Features

- **Precision Control:** Fine-tune colors with RGB/HEX inputs.
- **Quick Adjustments:** Easily modify brightness, saturation, and hue.
