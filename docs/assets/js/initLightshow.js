import VortexLib from './VortexLib.js';
import Lightshow from './Lightshow.js';

document.addEventListener('DOMContentLoaded', function() {
  const canvasId = 'vortex-canvas';
  const options = { type: 'scrolling', sectionCount: 20 };

  // Initialize the Vortex Engine WebAssembly module
  VortexLib().then(function(vortexLib) {
    const lightshow = new Lightshow(vortexLib, canvasId, options);
    lightshow.start(); // Start the animation

    const canvas = document.getElementById(canvasId);

    // Handle mouse/touch press on the canvas
    canvas.addEventListener('mousedown', function() {
      lightshow.pressButton(); // Call pressButton on canvas mousedown
    });

    // Handle mouse/touch release on the canvas
    canvas.addEventListener('mouseup', function() {
      lightshow.releaseButton(); // Call releaseButton on canvas mouseup
    });

    // Handle touch events for mobile
    canvas.addEventListener('touchstart', function() {
      lightshow.pressButton(); // Call pressButton on touchstart
    });

    canvas.addEventListener('touchend', function() {
      lightshow.releaseButton(); // Call releaseButton on touchend
    });
  });
});

