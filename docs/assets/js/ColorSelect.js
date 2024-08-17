let colorset = []; // Array to store the colors selected by the user
let deleteMode = false; // Track if delete mode is active
let activeDropdown = null; // Track the active dropdown menu

// Initialize with default colors
function initializeColorset() {
  colorset = ['#ff0000', '#00ff00', '#0000ff', '#000000']; // Default Red, Green, Blue, and Black
  renderSlots(); // Render the initial slots
}

// Function to render the slots based on the colorset array
function renderSlots() {
  const slotsContainer = document.getElementById('slots-container');
  slotsContainer.innerHTML = ''; // Clear existing slots

  colorset.forEach((color, index) => {
    const slot = document.createElement('div');
    slot.className = 'slot';
    slot.style.backgroundColor = color;
    slot.dataset.slot = index;
    slot.style.cursor = 'pointer';

    let holdTimer;
    let tapHandled = false;

    // Function to start the hold timer
    const startHold = () => {
      holdTimer = setTimeout(() => {
        tapHandled = true; // Prevent tap/click handling after hold is triggered
        startFlashingRed(index);
      }, 500); // Start flashing red after holding for 500ms
    };

    // Function to cancel the hold
    const cancelHold = () => {
      clearTimeout(holdTimer);
      if (deleteMode) {
        stopFlashingRed(index);
      }
    };

    // Function to handle click/tap
    const handleTap = () => {
      if (!tapHandled) {
        editColor(index);
      }
      tcolselapHandled = false; // Reset tapHandled for future interactions
    };

    slot.addEventListener('click', () => {
      cancelHold();
      editColor(index);
    });

    // Mouse events
    slot.addEventListener('mousedown', () => {
      startHold();
    });

    const releaseFunc = () => {
      if (!tapHandled) {
        handleTap();
        cancelHold();
        return;
      }
      if (deleteMode) {
        deleteColor(index);
        renderSlots();
      }
    }

    slot.addEventListener('mouseup', releaseFunc);

    slot.addEventListener('mouseleave', cancelHold);

    // Touch events
    slot.addEventListener('touchstart', (event) => {
      event.preventDefault(); // Prevent scrolling
      startHold();
    });

    slot.addEventListener('touchend', releaseFunc);

    slot.addEventListener('touchmove', (event) => {
      const touch = event.touches[0];
      const rect = slot.getBoundingClientRect();
      if (
        touch.clientX < rect.left ||
        touch.clientX > rect.right ||
        touch.clientY < rect.top ||
        touch.clientY > rect.bottom
      ) {
        cancelHold();
      }
    });

    slotsContainer.appendChild(slot);
  });

  // Render the "Add" button in the next available slot
  if (colorset.length < 8) {
    const addSlot = document.createElement('div');
    addSlot.className = 'slot add-slot';
    addSlot.innerHTML = '<div class="plus-icon">+</div>';
    addSlot.dataset.slot = colorset.length;
    addSlot.addEventListener('click', () => addNewColor(addSlot));
    slotsContainer.appendChild(addSlot);
  }

  // Render the remaining empty slots
  for (let i = colorset.length + 1; i < 8; i++) {
    const emptySlot = document.createElement('div');
    emptySlot.className = 'slot empty';
    slotsContainer.appendChild(emptySlot);
  }
}

// Start flashing red for delete mode
function startFlashingRed(index) {
  const slotElement = document.querySelector(`[data-slot="${index}"]`);
  slotElement.style.animation = 'flashRed 1s infinite';
  deleteMode = true;
}

// Stop flashing red for delete mode
function stopFlashingRed(index) {
  const slotElement = document.querySelector(`[data-slot="${index}"]`);
  slotElement.style.animation = '';
  deleteMode = false;
}

function createDropdown(options, onSelect, title) {
  const dropdown = document.createElement('div');
  dropdown.style.position = 'absolute';
  dropdown.style.backgroundColor = '#333';
  dropdown.style.border = '1px solid #777';
  dropdown.style.padding = '10px';
  dropdown.style.display = 'flex';
  dropdown.style.flexDirection = 'column';
  dropdown.style.gap = '10px';
  dropdown.style.borderRadius = '8px';
  dropdown.style.zIndex = 1000;

  // Create the title element
  const titleElement = document.createElement('div');
  titleElement.textContent = title;
  titleElement.style.fontFamily = 'Arial, sans-serif';
  titleElement.style.color = '#fff';
  titleElement.style.marginBottom = '10px';
  titleElement.style.textAlign = 'center';
  dropdown.appendChild(titleElement);

  // Create the container for the options
  const optionsContainer = document.createElement('div');
  optionsContainer.style.display = 'flex';
  optionsContainer.style.gap = '10px';

  options.forEach((option) => {
    const box = document.createElement('div');
    box.className = 'slot';
    box.style.width = '40px';
    box.style.height = '40px';
    box.style.cursor = 'pointer';
    box.style.borderRadius = '8px';
    box.style.border = '2px solid #555';

    // Apply the gradient or color
    if (option.backgroundImage) {
      box.style.backgroundImage = option.backgroundImage;
    } else {
      box.style.backgroundColor = option.color;
    }

    // Handle highlighting
    box.addEventListener('click', (event) => {
      event.stopPropagation();

      // Remove highlight from all boxes
      document.querySelectorAll('.highlighted').forEach(el => {
        el.classList.remove('highlighted');
        el.style.boxShadow = '';
      });

      // Add highlight to the selected box
      box.classList.add('highlighted');
      box.style.boxShadow = `0 0 10px 2px ${option.color}`; // Add glow effect

      onSelect(option.value, option.color);
    });

    optionsContainer.appendChild(box);
  });

  dropdown.appendChild(optionsContainer);

  return dropdown;
}

function closeDropdown() {
  if (activeDropdown) {
    // Remove highlight from all boxes when closing dropdown
    document.querySelectorAll('.highlighted').forEach(el => {
      el.classList.remove('highlighted');
      el.style.boxShadow = ''; // Reset glow effect
    });
    activeDropdown.remove();
    activeDropdown = null;
  }
}

function addNewColor(addSlot) {
  if (colorset.length < 8) {
    colorset.push('#000000');
    renderSlots();
    editColor(colorset.length - 1);
  }
}

function editColor(index) {
  showHueQuadrantDropdown(index);
}

function deleteColor(index) {
  colorset.splice(index, 1);
}

function handleDelete(slot) {
  let holdTimer;
  let tapTimer;
  let isHolding = false;

  const slotElement = document.querySelector(`[data-slot="${slot}"]`);

  const startHold = () => {
    isHolding = true;
    holdTimer = setTimeout(() => {
      startFlashingRed(slot);
    }, 500); // Start flashing red after holding for 500ms
  };

  const cancelHold = () => {
    clearTimeout(holdTimer);
    clearTimeout(tapTimer);
    isHolding = false;
    if (deleteMode) {
      stopFlashingRed(slot);
    }
  };

  const completeDelete = () => {
    if (isHolding && deleteMode) {
      deleteColor(index); // Delete the color if it's flashing red
      stopFlashingRed(slot);
      renderSlots(); // Re-render the slots to reflect the change
    }
    cancelHold();
  };

  const handleTap = () => {
    if (!isHolding) {
      editColor(index);
    }
  };

  // Desktop (mouse) events
  slotElement.addEventListener('mousedown', () => {
    startHold();
    tapTimer = setTimeout(() => {
      handleTap();
    }, 200); // Delay to differentiate tap from hold
  });
  slotElement.addEventListener('mouseleave', cancelHold);
  slotElement.addEventListener('mouseup', completeDelete);

  // Mobile (touch) events
  slotElement.addEventListener('touchstart', (event) => {
    event.preventDefault();
    startHold();
    tapTimer = setTimeout(() => {
      handleTap();
    }, 50); // Short delay to differentiate tap from hold
  });

  slotElement.addEventListener('touchend', (event) => {
    clearTimeout(tapTimer);
    completeDelete();
  });

  slotElement.addEventListener('touchmove', (event) => {
    const touch = event.touches[0];
    const rect = slotElement.getBoundingClientRect();
    if (
      touch.clientX < rect.left ||
      touch.clientX > rect.right ||
      touch.clientY < rect.top ||
      touch.clientY > rect.bottom
    ) {
      cancelHold();
    }
  });

  document.addEventListener('touchend', (event) => {
    if (!slotElement.contains(event.target)) {
      cancelHold();
    }
  });
}

function showHueQuadrantDropdown(index) {
  closeDropdown(); // Ensure previous dropdown is closed

  const hueQuadrants = [
    {
      value: 0,
      backgroundImage: 'linear-gradient(to right, hsl(0, 100%, 50%), hsl(70, 100%, 50%))'
    }, // 0° - 90° (Red to Yellow)
    {
      value: 90,
      backgroundImage: 'linear-gradient(to right, hsl(90, 100%, 50%), hsl(160, 100%, 50%))'
    }, // 90° - 180° (Green to Teal)
    {
      value: 180,
      backgroundImage: 'linear-gradient(to right, hsl(180, 100%, 50%), hsl(250, 100%, 50%))'
    }, // 180° - 270° (Cyan to Blue)
    {
      value: 270,
      backgroundImage: 'linear-gradient(to right, hsl(270, 100%, 50%), hsl(340, 100%, 50%))'
    } // 270° - 360° (Purple to Pink)
  ];

  const hueQuadrantDropdown = createDropdown(hueQuadrants, (hueQuadrantValue) => {
    showHueDropdown(index, hueQuadrantValue);
  }, 'Select Quadrant:'); // Adding the title

  document.body.appendChild(hueQuadrantDropdown);
  positionDropdown(hueQuadrantDropdown, index);
  activeDropdown = hueQuadrantDropdown;
}

// Similar changes for showHueDropdown, showSaturationDropdown, and showBrightnessDropdown
function showHueDropdown(index, hueQuadrantValue) {
  closeDropdown(); // Close previous dropdown

  const hues = [];
  for (let i = 0; i < 4; i++) {
    const hue = hueQuadrantValue + (i * 22.5); // 11.25° steps within the quadrant
    hues.push({ value: hue, color: `hsl(${hue}, 100%, 50%)` });
  }

  const hueDropdown = createDropdown(hues, (refinedHueValue) => {
    showSaturationDropdown(index, refinedHueValue);
  }, 'Select Hue:');

  document.body.appendChild(hueDropdown);
  positionDropdown(hueDropdown, index);
  activeDropdown = hueDropdown;
}

function showSaturationDropdown(index, refinedHueValue) {
  closeDropdown(); // Close previous dropdown

  const saturations = [
    { value: 100, color: `hsl(${refinedHueValue}, 100%, 50%)` }, // Pure color
    { value: 66, color: `hsl(${refinedHueValue}, 66%, 60%)` },  // Lighter color
    { value: 33, color: `hsl(${refinedHueValue}, 33%, 80%)` },  // Even lighter
    { value: 0, color: `hsl(${refinedHueValue}, 0%, 100%)` }    // White
  ];

  const saturationDropdown = createDropdown(saturations, (saturationValue) => {
    const baseLightness = calculateLightnessForSaturation(saturationValue);
    showBrightnessDropdown(index, refinedHueValue, saturationValue, baseLightness);
  }, 'Select Saturation:');

  document.body.appendChild(saturationDropdown);
  positionDropdown(saturationDropdown, index);
  activeDropdown = saturationDropdown;
}

function calculateLightnessForSaturation(saturation) {
  // A simple function to calculate base lightness
  return 50 + (100 - saturation) / 2;
}

function showBrightnessDropdown(index, refinedHueValue, saturationValue, baseLightness) {
  closeDropdown(); // Close the previous dropdown

  const brightnesses = [
    { value: baseLightness, color: `hsl(${refinedHueValue}, ${saturationValue}%, ${baseLightness}%)` }, // Starting color
    { value: baseLightness * 0.66, color: `hsl(${refinedHueValue}, ${saturationValue}%, ${baseLightness * 0.66}%)` }, // Darker
    { value: baseLightness * 0.33, color: `hsl(${refinedHueValue}, ${saturationValue}%, ${baseLightness * 0.33}%)` }, // Even darker
    { value: 0, color: `hsl(${refinedHueValue}, ${saturationValue}%, 0%)` }    // Black
  ];

  const brightnessDropdown = createDropdown(brightnesses, (_, finalColor) => {
    colorset[index] = finalColor; // Update the color in the colorset array
    renderSlots(); // Re-render the slots to reflect the change
    closeDropdown(); // Close the dropdown after selection
  }, 'Select Brightness:');

  document.body.appendChild(brightnessDropdown);
  positionDropdown(brightnessDropdown, index);
  activeDropdown = brightnessDropdown;
}

function positionDropdown(dropdown, index) {
  const slotElement = document.querySelector(`[data-slot="${index}"]`);
    if (!slotElement) {
      return;
    }
  const rect = slotElement.getBoundingClientRect();
  const dropdownWidth = dropdown.offsetWidth;
  const dropdownHeight = dropdown.offsetHeight;

  // Calculate initial positions
  let top = rect.bottom + window.scrollY + 10;
  let left = rect.left + window.scrollX + (rect.width / 2) - (dropdownWidth / 2);

  const margin = 10;

  // Ensure dropdown stays within the viewport horizontally
  if (left < margin) {
    left = margin; // Minimum padding from the left edge
  } else if (left + dropdownWidth > (window.innerWidth - margin)) {
    left = window.innerWidth - dropdownWidth - margin; // Minimum padding from the right edge
  }

  // Ensure dropdown stays within the viewport vertically
  if (top + dropdownHeight > window.innerHeight + window.scrollY) {
    top = rect.top + window.scrollY - dropdownHeight - margin; // Place above the slot element if there's no space below
  }

  // Apply calculated positions
  dropdown.style.top = `${top}px`;
  dropdown.style.left = `${left}px`;
}

document.addEventListener('click', (event) => {
  let targetElement = event.target;
  let isClickInside = false;

  // Traverse up the DOM tree to see if the click was inside a .slot, .add-slot, or the active dropdown
  while (targetElement) {
    if (
      targetElement.classList.contains('slot') ||
      targetElement.classList.contains('add-slot') ||
      (activeDropdown && activeDropdown.contains(targetElement))
    ) {
      isClickInside = true;
      break;
    }
    targetElement = targetElement.parentElement;
  }

  // Close the dropdown if the click was outside all relevant elements
  if (activeDropdown && !isClickInside) {
    closeDropdown();
  }
});


document.addEventListener('DOMContentLoaded', () => {
  initializeColorset();
});
