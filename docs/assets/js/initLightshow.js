import VortexLib from './VortexLib.js';
import Lightshow from './Lightshow.js';

document.addEventListener('DOMContentLoaded', function() {
  const canvasId = 'vortex-canvas';
  const options = { type: 'scrolling', sectionCount: 20 };

  VortexLib().then(function(vortexLib) {
    const lightshow = new Lightshow(vortexLib, canvasId, options);
    lightshow.start();

    const cycleButton = document.getElementById('cycle-button');
    const selectButton = document.getElementById('select-button');

    // Handle Cycle button click
    cycleButton.addEventListener('click', function() {
      lightshow.vortex.shortClick(0); // Example: cycle through modes
    });

    // Handle Select button click
    selectButton.addEventListener('click', function() {
      lightshow.vortex.longClick(0); // Example: select the current mode or pattern
    });
  });
});
