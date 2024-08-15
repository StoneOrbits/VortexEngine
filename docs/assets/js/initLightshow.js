import VortexLib from './VortexLib.js';
import Lightshow from './Lightshow.js';

document.addEventListener('DOMContentLoaded', function() {
  const canvasId = 'vortex-canvas';
  const options = { type: 'scrolling', sectionCount: 20 };

  // Initialize the Vortex Engine WebAssembly module
  VortexLib().then(function(vortexLib) {
    const lightshow = new Lightshow(vortexLib, canvasId, options);
    lightshow.start(); // Start the animation

    // Get the press and release buttons
    const pressButton = document.getElementById('vortex-press-button');
    const releaseButton = document.getElementById('vortex-release-button');

    // Connect the pressButton and releaseButton methods in Lightshow.js
    pressButton.addEventListener('mousedown', function() {
      lightshow.pressButton(); // Call pressButton when the button is pressed
    });

    releaseButton.addEventListener('mouseup', function() {
      lightshow.releaseButton(); // Call releaseButton when the button is released
    });

    // Optionally handle touch events for mobile
    pressButton.addEventListener('touchstart', function() {
      lightshow.pressButton(); // Call pressButton on touchstart
    });

    releaseButton.addEventListener('touchend', function() {
      lightshow.releaseButton(); // Call releaseButton on touchend
    });
  });
});

