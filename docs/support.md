---
layout: default
title: Support
nav_order: 10
---

# Support

If you're experiencing issues or if you need further assistance that requires expert help, please reachout to either StoneOrbits or the Vortex Community Discord.

<div id="reveal-section">
  <button id="reveal-button">Click to Reveal Discord Link</button>
</div>

<div id="discord-link" style="display:none; margin-top: 20px;"></div>

## Documentation

If you havent yet, be sure to explore our comprehensive documentation to learn more about your Vortex Device:

- [Getting Started](getting_started.html)
- [Quick Use Guides](quick_use_guides.html)

<script>
  document.addEventListener("DOMContentLoaded", function () {
    const base64 = "aHR0cHM6Ly9kaXNjb3JkLmdnLzRSOWF0OFM4U24=";
    function revealLink() {
      const url = atob(base64);
      document.getElementById("discord-link").innerHTML = `<a href="${url}" target="_blank">${url}</a>`;
      document.getElementById("discord-link").style.display = "block";
      document.getElementById("reveal-section").style.display = "none";
    }
    document.getElementById("reveal-button").addEventListener("click", revealLink);
  });
</script>