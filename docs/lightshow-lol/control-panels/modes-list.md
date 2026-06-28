---
layout: default
title: Modes List
nav_order: 15
parent: Control Panels
---
<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.1/css/all.min.css">
<style>
  .panel-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 16px;
    margin: 0 auto;
    max-width: 80%;
    margin-top: 10px;
  }

  .panel-link {
    background-color: #2e2e2e;
    border-radius: 8px;
    text-decoration: none;
    color: #ffffff;
    padding: 16px;
    display: flex;
    flex-direction: column;
    align-items: center;
    transition: transform 0.2s;
    border: 1px solid #080808;
  }

  .panel-link:hover {
    transform: scale(1.02);
  }

  .panel-title {
    margin-bottom: 8px;
    font-weight: bold;
    color: #ffcc00;
  }

  .panel-img {
    max-width: 100%;
  }
</style>
# Modes List

<img style="float:right;max-width:40%;margin:10px;" src="{{ '/assets/images/lightshow-lol-modes.png' | relative_url }}">

The **Modes Panel** manages the list of modes on your device.
<br>
A [mode]({{ site.baseurl }}{% link vortex-engine/modes/index.md %}) is a complete set of patterns and colors — one per LED.
<br>
You can add, delete, copy, share, and transfer modes between your device and the editor.

### Toolbar Buttons

These buttons sit at the top of the panel:

| Button | Action |
|--------|--------|
| <i class="fas fa-plus-circle"></i> **Add** | Creates a new random mode at the end of the list |
| <i class="fa-solid fa-paste"></i> **Paste** (Import) | Imports a mode from JSON on your clipboard |
| <i class="fa-solid fa-upload fa-flip-vertical"></i> **Load** (Pull) | Loads all modes from your connected device into the editor |
| <i class="fa-solid fa-download fa-flip-vertical"></i> **Save** (Push) | Saves all modes from the editor to your connected device |
| <i class="fa-solid fa-satellite-dish"></i> **Broadcast** | Transmits the selected mode to a Duo device |
| <i class="fa-solid fa-satellite-dish"></i> **Listen** | Listens for an incoming mode from a Duo device |

The Push/Pull and Duo buttons are disabled until a device is connected.

### Mode Entry Buttons

Click a mode in the list to select it. When selected, action buttons appear next to its name:

<img style="max-width:100%;margin:6px 0;" src="{{ '/assets/images/lightshow-lol-mode-entry.png' | relative_url }}">

- <i class="fas fa-share-alt"></i> **Share** — opens a dialog to share the mode with the Vortex Community
- <i class="fas fa-link"></i> **Link** — generates a compressed URL for the mode and copies it to your clipboard
- <i class="fa-solid fa-copy"></i> **Copy** — copies the mode's JSON data to your clipboard
- **× Delete** — removes the mode from the list (cannot be undone)

The Share, Link, and Copy buttons only show when a mode is selected. The Delete button is always visible.

### Working with Modes

- **Select a mode** by clicking it — the editor switches to it so you can edit its patterns, colorsets, and parameters.
- **Pull from device** to load your device's modes into the editor, edit them, then **Push** to save your changes back.
- **Import** a mode from a shared URL or JSON snippet to load it into the editor.

## Other Panels

<div class="panel-grid">
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/device-controls.md %}" class="panel-link">
    <span class="panel-title">← Device Control 🔗</span>
    <img src="{{ '/assets/images/lightshow-lol-device.png' | relative_url }}" class="panel-img">
  </a>
  <a href="{{ site.baseurl }}{% link lightshow-lol/control-panels/led-selection.md %}" class="panel-link">
    <span class="panel-title">🔗 Led Selection →</span>
    <img src="{{ '/assets/images/lightshow-lol-led-select.png' | relative_url }}" class="panel-img">
  </a>
</div>

