---
layout: default
title: Context Menus
nav_order: 5
parent: Control Panels
---

<style>
  .menu-demo {
    background: #2a2a2a;
    border: 1px solid #555;
    border-radius: 6px;
    padding: 8px 0;
    display: inline-block;
    min-width: 200px;
  }
  .menu-demo .item {
    padding: 8px 16px;
    color: #e0ddd9;
    font-size: 14px;
  }
  .menu-demo .item.danger {
    color: #e07474;
  }
  .menu-demo .sep {
    height: 1px;
    background: #444;
    margin: 4px 8px;
  }
  .menu-wrap {
    float: right;
    margin: 0 0 10px 15px;
  }
  .menu-wrap .menu-demo {
    margin: 0;
  }
  .menu-wrap .menu-demo + .menu-demo {
    margin-top: 6px;
  }
</style>

# Context Menus

<div class="menu-wrap">
<div class="menu-demo">
  <div class="item">Copy Mode</div>
  <div class="item">Copy Colorset</div>
  <div class="item">Copy Pattern</div>
  <div class="sep"></div>
  <div class="item">Get Link</div>
  <div class="item">Share Mode</div>
  <div class="sep"></div>
  <div class="item">Paste</div>
  <div class="sep"></div>
  <div class="item">Undo</div>
  <div class="item">Redo</div>
  <div class="sep"></div>
  <div class="item">Help</div>
</div>
</div>

Right-click around the editor to copy, paste, share, and get help. Right-clicking the **canvas** shows the full menu for the current mode.

- **Copy Mode / Colorset / Pattern** — copy to your clipboard
- **Get Link** — creates a shareable URL to send to anyone
- **Share Mode** — sends your mode to vortex.community
- **Paste** — pastes whatever's on your clipboard
- **Undo** — reverses your last change
- **Redo** — re-applies the last undone change
- **Help** — opens the wiki without leaving the editor

Right-click any **panel** or **color swatch** and choose **Help** for that specific panel. On the **Modes Panel** you'll also find copy, link, share, paste, and delete options.

### Keyboard Shortcuts

`Ctrl+C` opens the copy menu at your cursor. `Ctrl+V` pastes right away — no prompts.

### Undo & Redo

Every change you make in the editor can be undone. Right-click the canvas and choose **Undo** or **Redo**, or use the keyboard shortcuts below.

| Action | Shortcut |
|--------|----------|
| Undo | `Ctrl+Z` |
| Redo | `Ctrl+Y` or `Ctrl+Shift+Z` |

A small text log appears briefly at the bottom of the canvas showing what was undone or redone (e.g. "Pattern: Solid Red", "Deleted Mode 2"). Undo works across pattern changes, color edits, mode add/delete, colorset generator, import, and more.

### Help Popup

**Help** opens the wiki in a popup. Click the **×** or outside the popup to close it.
