---
layout: default
title: Programming Guide
nav_order: 1
parent: Duo
has_children: true
---

<style>
.device-icon {
   margin: 20px;
   width: 20%;
   height: 20%
}
</style>

<img align="right" height="220" class="device-icon" src="{{ '/assets/images/duo-logo-square-512.png' | relative_url }}">

# Duo Programming Guide

There are two recommended methods for programming Duos. Which method is best depends on how many modes you want to update and whether you want to change the total number of modes.

## Choosing a Programming Method

### Wireless Programming (Recommended for Most Cases)

If your goal is to replace only a few modes on your Duo, wireless programming is the fastest and easiest option.

With wireless programming, a mode is transmitted from a Chromadeck to a Duo using the built-in Mode Sharing system. This allows you to update individual modes without removing the Duo casing or its battery.

Wireless programming is the only option if you are using a mobile device with your Chromadeck.

**Recommended when:**

- Replacing only one or a few modes
- Making quick updates on-the-go
- Programming Duos frequently
- You do not want to disassemble the Duo
- No access to a PC

See: [Wireless Programming]({{ site.baseurl }}{% link vortex-devices/duo/programming-guide/wireless.md %})

### Chromalink Programming

Chromalink programming enables Lightshow.lol to directly access and modify the Duo’s memory via a Chromalink cable connected to a Chromadeck.

This method is primarily used when replacing all nine modes on a Duo. It is also the only supported way to change the total number of modes on the device (1–9).

Because the Duo must be removed from its sleeve and disconnected from its battery before being placed into the Chromalink holder, this process carries some risk and requires more preparation than wireless programming. The battery connector is relatively delicate, so unnecessary disassembly should be avoided.

Chromalink programming is not available when connecting to the Chromadeck over Bluetooth on Mobile devices.

As a result, Chromalink programming is recommended only when replacing all nine modes, performing firmware updates, or adjusting the total number of modes on the device.

**Recommended when:**

- Replacing all nine modes
- Completely reorganizing your mode list
- Programming many Duos with the same list of modes
- Performing firmware updates
- Reducing total number of modes

See: [Chromalink Programming]({{ site.baseurl }}{% link vortex-devices/duo/programming-guide/chromalink-guide/index.md %})

## Additional Reading

- [Wireless Programming]({{ site.baseurl }}{% link vortex-devices/duo/programming-guide/wireless.md %})
- [Chromalink Programming]({{ site.baseurl }}{% link vortex-devices/duo/programming-guide/chromalink-guide/index.md %})
