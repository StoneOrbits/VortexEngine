---
layout: default
title: Editor Connection
nav_order: 3
parent: Menus
grand_parent: Vortex Engine
published: false # temporarily disabling this page
---

# Editor Connection

The Editor Connection menu is used to connect your Vortex Device to the Vortex Editor. This allows you to program your device using the editor's user-friendly interface. Please note that this menu is not available on the StoneOrbits Duo.

## How it Works

When you enter the Editor Connection menu, the device prepares to establish a connection with the Vortex Editor.

The editor will check all devices plugged in and detect when a device plugs in and sends the specific greeting, at this moment the editor
will send back a response and establish a connection with the device.

The Editor can be used to perform two main commands with the Vortex Device:

- **Pull Modes:** The editor requests the current modes from the device. The device sends the modes to the editor.
- **Push Modes:** The editor sends new modes to the device. The device receives these modes and saves them.

There is another action that will happen anytime you click on a mode in the editor, that mode will be sent to the device to play as a demo.

It is important to remember to push your changes after you have finished using the editor on a device, the editor only makes changes locally and sends the mode for demonstration.  Until you hit 'push' the connected device will remain unchanged and can be unplugged at any time.

It is also a good idea to save your savefile to the PC and backup your modes anytime you use the editor.

## Using the Editor Connection Menu

To use the Editor Connection menu, follow these steps:

1. Enter the Editor Connection (purple) menu from any mode
2. If the Vortex Editor is running and connected, it will send commands to the device. The device will respond to these commands as described above.
3. To exit the Editor Connection menu, perform a long click. The device will send a goodbye message to the editor and return to the previous mode.

Remember, the Editor Connection menu is a powerful tool for programming your Vortex Device. It allows you to use the full capabilities of the Vortex Editor, making it easier to create and manage your modes.

If at any time you are having connection difficulties with the editor, try performing a short click in the editor menu. This will reset the editor connection and re-connect if the editor has been closed and re-opened.

