---
layout: default
title: Support
nav_order: 10
---

# Support

If you're experiencing issues or if you need further assistance that requires expert help, please reachout to either StoneOrbits or the Vortex Community Discord.

<div id="reveal-section">
  <button onclick="revealLink()">Click to Reveal Discord Link</button>
</div>

<div id="discord-link" style="display:none; margin-top: 20px;"></div>


## Documentation

If you havent yet, be sure to explore our comprehensive documentation to learn more about your Vortex Device:

- [Getting Started](getting_started.html)
- [Quick Use Guides](quick_use_guides.html)

<script src="https://cdnjs.cloudflare.com/ajax/libs/crypto-js/4.1.1/crypto-js.min.js"></script>
<script>
  document.addEventListener("DOMContentLoaded", function () {
    const encrypted = "U2FsdGVkX19f8pJkC6AZMmEYpWEgSnjc6K3+tZz9DwY="; // AES-encrypted https://discord.gg/4R9at8S8Sn
    const key = "vortex"; // encryption key

    function revealLink() {
      try {
        const decrypted = CryptoJS.AES.decrypt(encrypted, key).toString(CryptoJS.enc.Utf8);
        if (!decrypted.startsWith("http")) throw "Invalid decryption";
        document.getElementById("discord-link").innerHTML = `<a href="${decrypted}" target="_blank">${decrypted}</a>`;
        document.getElementById("discord-link").style.display = "block";
        document.getElementById("reveal-section").style.display = "none";
      } catch (e) {
        document.getElementById("discord-link").innerHTML = "<span style='color:red;'>Error decrypting link.</span>";
        document.getElementById("discord-link").style.display = "block";
      }
    }

    document.getElementById("reveal-button").addEventListener("click", revealLink);
  });
</script>
