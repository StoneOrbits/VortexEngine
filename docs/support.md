---
layout: default
title: Support
nav_order: 10
---

# Support

If you're experiencing issues or if you need further assistance that requires expert help, please reachout to either StoneOrbits or the Vortex Community Discord.

<div id="reveal-section">
  <button id="reveal-button" onclick="revealLink()">Click to Reveal Discord Link</button>
</div>

<div id="discord-link" style="display:none; margin-top: 20px;"></div>

## Documentation

If you havent yet, be sure to explore our comprehensive documentation to learn more about your Vortex Device:

- [Getting Started](getting_started.html)
- [Quick Use Guides](quick_use_guides.html)

<script>
  document.addEventListener("DOMContentLoaded", function () {
    const encoded = [
      102, 122, 123, 122, 119, 122, 115, 103, 118, 45,
      111, 119, 125, 126, 108, 45, 111, 108, 45,
      55, 94, 76, 112, 45, 112, 126, 124, 117
    ];

    function decode(obf) {
      return obf.map((c, i) => {
        const delta = ((i % 5) * 3 - 2);
        return String.fromCharCode(c - delta);
      }).join('');
    }

    function revealLink() {
      const url = decode(encoded);
      document.getElementById("discord-link").innerHTML = `<a href="${url}" target="_blank">${url}</a>`;
      document.getElementById("discord-link").style.display = "block";
      document.getElementById("reveal-section").style.display = "none";
    }

    document.getElementById("reveal-button").addEventListener("click", revealLink);
  });
</script>
