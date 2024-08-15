import Lightshow from './Lightshow.js';

document.addEventListener('DOMContentLoaded', function() {
  const canvasId = 'vortex-canvas';
  const options = { type: 'scrolling', sectionCount: 20 };

  VortexLib().then(function(vortexLib) {
    const lightshow = new Lightshow(vortexLib, canvasId, options);
    lightshow.start(); // Start the animation
  });
});

