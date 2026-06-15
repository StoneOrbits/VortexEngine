---
layout: default
title: Lightshow.lol
nav_order: 3
has_children: true
---

<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.0/css/all.min.css">

<style>
  .vortex-sync {
    margin: 26px 0;
    padding: 14px;
    border: 1px solid var(--border-color, #444);
    border-radius: 12px;
    font-family: system-ui, sans-serif;
    color: inherit;
  }

  .vortex-sync__grid {
    display: grid;
    grid-template-columns: 1fr 160px 1fr;
    gap: 14px;
    align-items: center;
  }

  .vortex-sync__panel {
    border: 1px solid var(--border-color, #666);
    border-radius: 10px;
    padding: 12px;
    text-align: center;
    min-height: 120px;
  }

  .vortex-sync__panel--device {
    border-color: #4aa3ff;
  }

  .vortex-sync__panel--platform {
    border-color: #c084fc;
  }

  .vortex-sync__title {
    font-weight: 600;
    margin-bottom: 8px;
  }

  .vortex-sync__label {
    font-size: 12px;
    opacity: 0.7;
    margin-bottom: 10px;
  }

  .vortex-sync__mode-list {
    display: flex;
    flex-direction: column;
    gap: 6px;
    font-size: 12px;
    opacity: 0.85;
  }

  .vortex-sync__mode {
    border: 1px solid var(--border-color, #555);
    border-radius: 6px;
    padding: 4px 6px;
    font-size: 12px;
  }

  .vortex-sync__transport {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 10px;
    text-align: center;
    font-size: 12px;
    opacity: 0.8;
  }

  .vortex-sync__arrow {
    font-size: 38px;
    opacity: 0.8;
  }

  .vortex-sync__icons {
    font-size: 28px;
    opacity: 0.9;
    display: flex;
    gap: 20px;
  }

  .vortex-sync__sync-label {
    font-size: 11px;
    opacity: 0.65;
    max-width: 140px;
  }

  .vortex-sync__note {
    margin-top: 12px;
    font-size: 12px;
    opacity: 0.75;
    text-align: center;
    line-height: 1.4;
  }

  @media (max-width: 800px) {
    .vortex-sync__grid {
      grid-template-columns: 1fr;
    }

    .vortex-sync__transport {
      flex-direction: row;
      justify-content: center;
      gap: 16px;
    }

    .vortex-sync__arrow {
      transform: rotate(90deg);
    }
  }

  .vortex-sync__features {
    margin-top: 10px;
    display: flex;
    flex-direction: column;
    gap: 6px;
    font-size: 11px;
    opacity: 0.85;
  }

  .vortex-sync__feature {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 6px;
    border: 1px solid var(--border-color, #555);
    border-radius: 6px;
    padding: 4px 6px;
  }

  .vortex-sync__feature i {
    font-size: 12px;
    opacity: 0.9;
  }
</style>

# Lightshow.lol

[Lightshow.lol](https://lightshow.lol) is an online Mode Editor and general purpose Vortex Engine simulator.

Connect with any [Vortex Device]({{ site.baseurl }}{% link vortex-devices/index.md %}) to edit modes and preview them in real-time.

<div style="border-radius: 5px; border: 3px solid gray; text-align: center; margin: 20px">
  <img src="{{ '/assets/images/lightshow-lol-preview.gif' | relative_url }}" style="box-shadow: 2px 2px 10px rgba(0, 0, 0, 0.2); border-radius: 5px;">
</div>

{: .note }
The **preview** will never look exactly like **real LEDs**.  Although this is a direct simulation, RGB programmable LEDs produce light and colors differently from computer monitors and screens.

## The Basics

Think of lightshow.lol as a second Vortex Device.

If you have an Orbit, then lightshow.lol acts like another Orbit you can copy modes to, edit, and sync back.

    1. Connect device to lightshow.lol
    2. Import modes from device
    3. Edit modes with live previews on the device
    4. Backup or share modes with the community
    5. Sync updated modes back to the device

<div class="vortex-sync">

  <div class="vortex-sync__grid">

    <!-- DEVICE SIDE -->
    <div class="vortex-sync__panel vortex-sync__panel--device">
      <div class="vortex-sync__title">Vortex Device</div>
      <div class="vortex-sync__label">On-device stored mode list</div>

      <div class="vortex-sync__mode-list">
        <div class="vortex-sync__mode">Mode A - stored locally</div>
        <div class="vortex-sync__mode">Mode B - stored locally</div>
        <div class="vortex-sync__mode">Mode C - stored locally</div>
      </div>
    </div>

    <!-- TRANSPORT -->
    <div class="vortex-sync__transport">
      <i class="vortex-sync__arrow fa-solid fa-arrow-right-arrow-left"></i>

      <div class="vortex-sync__sync-label">
        USB / Bluetooth Mode Transfer
      </div>

      <div class="vortex-sync__icons">
        <i class="fab fa-usb" style="font-size:22px; display:inline-block; transform: rotate(-90deg);"></i>
        <i class="fa-brands fa-bluetooth-b"></i>
      </div>


    </div>

    <!-- PLATFORM SIDE -->
    <div class="vortex-sync__panel vortex-sync__panel--platform">
      <div class="vortex-sync__title">Lightshow.lol</div>
      <div class="vortex-sync__label">Independent editable copy</div>

      <div class="vortex-sync__mode-list">
        <div class="vortex-sync__mode">Mode A - editable copy</div>
        <div class="vortex-sync__mode">Mode B - editable copy</div>
        <div class="vortex-sync__mode">Mode C - editable copy</div>
      </div>
    </div>

  </div>

  <div class="vortex-sync__note">
    Both sides maintain independent mode lists. USB/Bluetooth only serves to transfer modes and displays a temporary live preview of the selected mode. 
  </div>

</div>
## Getting Started

To get started with lightshow.lol, check out these tutorials:

 - [Getting Started on Computer]({{ site.baseurl }}{% link lightshow-lol/getting-started.md %})
 - [Getting Started on Mobile]({{ site.baseurl }}{% link lightshow-lol/getting-started-mobile.md %})

For information about the individual controls on desktop look at the [Control Panel Documentation]({{ site.baseurl }}{% link lightshow-lol/control-panels/index.md %}).

