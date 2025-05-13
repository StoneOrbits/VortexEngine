---
layout: default
title: Sharing on Duos
nav_order: 1
parent: Mode Sharing
grand_parent: Menus
---

<style>
.device-card {
  float: right;
  flex-direction: column;
  align-items: center;
  text-align: center;
  width: 100px;
}
.device-card img {
  width: 10em;
  height: 8em;
  object-fit: contain;
  margin-bottom: 0.5rem;
}
</style>

<div>
  <div class="device-card">
    <img src="assets/images/duo-logo-square-512.png" alt="Duo">
  </div>
</div>

# Mode Sharing on Duos

The Duo only supports **Visible Light Mode Sharing** — it does **not support Infrared**.

In **May 2025**, the visible light Mode Sharing protocol was upgraded. All Duos manufactured after this date use a **new, faster, more reliable protocol**. However, for compatibility, the new firmware retains a **Legacy Mode Sharing** option for communicating with older Duos.


## Protocol Difference

- **New Protocol (May 2025+)**  
  - Faster transfer speed  
  - Improved reliability and error correction  
  - Continuous sending (hold to broadcast)  
  - Default for new Duos

- **Legacy Protocol (Pre May 2025 Duos)**  
  - Slower and less reliable  
  - Not compatible with new protocol
  - Click to send once (bad design)
  - Default for old Duos

If your Duo **only shows a dim white LED** in the Mode Sharing menu, it is still using **legacy modesharing**. [Updating wth a Chromadeck](duo_chromalink_guide.html) is recommended to access the new protocol.

---

## Mode Sharing Menu

The Mode Sharing menu (<span style="color:rgb(0, 255, 255);">cyan</span>) offers three options:

1. **Send / Receive**:  Send/receive with new protocol (LED color: <span style="color:rgb(0, 255, 155);">teal</span>)
2. **Send / Receive Legacy**: Send/receive legacy compatibility mode (LED color: <span style="color:rgb(255, 255, 255);">white</span>)
3. **Exit**: Leave the menu (LED color: <span style="color:rgb(255, 0, 0);">blinking red</span>)

Use a **long click** to activate the currently selected option.

### Sending a Mode

1. **Choose Mode to Send**  
   Navigate to the mode and hold the button until LEDs flash white.

2. **Enter Mode Sharing Menu**  
   Cycle to cyan and long click.

3. **Select Protocol**  
   Short click to select either "Send / Receive" (new) or "Send / Receive Legacy" (for older Duos).

4. **Start Sending**  
   Long click to begin broadcasting.  
   Hold the Duo so its LED touches the other device’s button area for best results.

5. **Exit**  
   Short click to select "Exit" (blinking red), long click to leave.

### Receiving a Mode

1. **Choose Slot to Overwrite**  
   Navigate to the slot and hold until LEDs flash white.

2. **Enter Mode Sharing Menu**  
   Cycle to cyan and long click.

3. **Select Protocol**  
   Short click to select either "Send / Receive" or "Send / Receive Legacy".

4. **Wait for Sender**  
   Hold the receiving Duo so its button touches the sender's LED.  
   The Duo will automatically save the incoming mode and exit when complete.

---

## Tips for Best Results

- **Touch the Sender to the Receiver** — especially LED to button.
- **Avoid Obstructions** — fabric or diffuser covers may interfere.
- **Hold to Send** — keep the button held while sending to maintain transmission.
- **Use Legacy Only When Needed** — for older, pre-update Duos.
