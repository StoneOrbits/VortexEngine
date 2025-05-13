---
layout: default
title: Mode Sharing
nav_order: 2
parent: Menus
grand_parent: Vortex Engine
---

<style>
.device-grid {
  display: flex;
  flex-wrap: wrap;
  gap: 1.5rem;
  justify-content: center;
  margin-top: 0.5rem;
}

.device-card {
  display: flex;
  flex-direction: column;
  align-items: center;
  text-align: center;
  width: 100px;
  text-decoration: none;
  color: inherit;
  transition: transform 0.2s ease;
}

.device-card img {
  width: 80px;
  height: 80px;
  object-fit: contain;
  margin-bottom: 0.5rem;
}

.device-card:hover {
  transform: scale(1.05);
}

.device-section {
  margin-bottom: 1.5rem;
  border: 1px solid #444;
  border-radius: 8px;
  padding: 1rem;
  background: #1a1a1a;
}

.device-section h2 {
  margin-top: 0.25em;
}
</style>

{: .warning }
This page is under construction and may have some small mistakes, apologies and thank you for your patience while we work to update this section

# Mode Sharing Menu

The Mode Sharing menu allows wirelessly sending and receiving of modes from one Vortex Device to another. Depending on the device, there are slightly different capabilities of modesharing.

Some Mode Sharing uses **Visible Light** and some uses **Infrared Light**, visible light is slower and more error prone but more suitable for small hardware.

## Device Specific Details

Read one of the guides below to learn about Mode Sharing on each device

<div class="device-section">
  <h2>Duo</h2>
  <p>The Duo only uses Visible Light mode sharing, optimized for its compact size.</p>
  <div class="device-grid">
    <a class="device-card" href="mode_sharing_menu_duo.html">
      <img src="assets/images/duo-logo-square-512.png" alt="Duo">
      <div>Duo</div>
    </a>
  </div>
</div>

<div class="device-section">
  <h2>Next-Gen Hybrid Devices</h2>
  <p>These newer devices are a hybrid design that fully support both Visible Light and Infrared.</p>
  <div class="device-grid">
    <a class="device-card" href="mode_sharing_menu_nextgen.html">
      <img src="assets/images/spark-logo-square-512.png" alt="Spark Orbit">
      <div>Spark Orbit</div>
    </a>
    <a class="device-card" href="mode_sharing_menu_nextgen.html">
      <img src="assets/images/sparkhandle-logo-square-512.png" alt="Spark Handle">
      <div>Spark Handle</div>
    </a>
    <a class="device-card" href="mode_sharing_menu_nextgen.html">
      <img src="assets/images/chromadeck-logo-square-512.png" alt="Chromadeck">
      <div>Chromadeck</div>
    </a>
  </div>
</div>

<div class="device-section">
  <h2>Legacy Infrared Devices</h2>
  <p>These older devices only support Infrared and Visible Light sending, they lack Visible Light receivers.</p>
  <div class="device-grid">
    <a class="device-card" href="mode_sharing_menu_legacy.html">
      <img src="assets/images/orbit-logo-square-512.png" alt="Vortex Orbit">
      <div>Vortex Orbit</div>
    </a>
    <a class="device-card" href="mode_sharing_menu_legacy.html">
      <img src="assets/images/handle-logo-square-512.png" alt="Omega Handle">
      <div>Handles</div>
    </a>
    <a class="device-card" href="mode_sharing_menu_legacy.html">
      <img src="assets/images/gloves-logo-square-512.png" alt="Vortex Glove">
      <div>Gloves</div>
    </a>
  </div>
</div>