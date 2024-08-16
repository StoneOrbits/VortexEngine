let colorset = []; // Array to store the colors selected by the user
let deleteMode = false; // Track if delete mode is active
let activeDropdown = null; // Track the active dropdown menu

// Function to render the slots based on the colorset array
function renderSlots() {
  const slotsContainer = document.getElementById('slots-container');
  slotsContainer.innerHTML = ''; // Clear existing slots

  // Render each color in the colorset array
  colorset.forEach((color, index) => {
    const slot = document.createElement('div');
    slot.className = 'slot';
    slot.style.backgroundColor = color;
    slot.dataset.slot = index;
    slot.style.cursor = 'pointer';

    // Add event listeners for editing and deleting
    slot.addEventListener('click', () => editColor(index));
    slot.addEventListener('mousedown', () => handleDelete(index));

    slotsContainer.appendChild(slot);
  });

  // Render the "Add" button in the next available slot
  if (colorset.length < 8) {
    const addSlot = document.createElement('div');
    addSlot.className = 'slot add-slot';
    addSlot.innerHTML = '<div class="plus-icon">+</div>';
    addSlot.addEventListener('click', () => addNewColor());

    slotsContainer.appendChild(addSlot);
  }

  // Render the remaining empty slots
  for (let i = colorset.length + 1; i < 8; i++) {
    const emptySlot = document.createElement('div');
    emptySlot.className = 'slot empty';
    slotsContainer.appendChild(emptySlot);
  }
}

// Initialize with default colors
function initializeColorset() {
  colorset = ['#ff0000', '#00ff00', '#0000ff', '#000000']; // Default Red, Green, Blue, and Black
  renderSlots(); // Render the initial slots
}

// Start flashing red for delete mode
function startFlashingRed(slot) {
  const slotElement = document.querySelector(`[data-slot="${slot}"]`);
  slotElement.style.animation = 'flashRed 1s infinite';
  deleteMode = true;
}

// Stop flashing red for delete mode
function stopFlashingRed(slot) {
  const slotElement = document.querySelector(`[data-slot="${slot}"]`);
  slotElement.style.animation = '';
  deleteMode = false;
}

function createDropdown(options, onSelect) {
  const dropdown = document.createElement('div');
  dropdown.style.position = 'absolute';
  dropdown.style.backgroundColor = '#333';
  dropdown.style.border = '1px solid #777';
  dropdown.style.padding = '10px';
  dropdown.style.display = 'flex';
  dropdown.style.gap = '10px';
  dropdown.style.borderRadius = '8px';
  dropdown.style.zIndex = 1000;

  options.forEach((option) => {
    const box = document.createElement('div');
    box.style.width = '40px';
    box.style.height = '40px';
    box.style.backgroundColor = option.color;
    box.style.cursor = 'pointer';
    box.style.borderRadius = '8px';
    box.style.border = '2px solid #555';

    box.addEventListener('click', (event) => {
      event.stopPropagation();
      onSelect(option.value, option.color);
    });

    dropdown.appendChild(box);
  });

  return dropdown;
}

function addNewColor() {
  if (colorset.length < 8) {
    colorset.push('#000000'); // Add a default color (black) as a placeholder
    renderSlots(); // Re-render slots
    editColor(colorset.length - 1); // Open the color picker for the new slot
  }
}

function editColor(slot) {
  if (!deleteMode) {
    closeDropdown(); // Ensure no other dropdowns are open
    showHueQuadrantDropdown(slot); // Start with the Hue Quadrant selection
  }
}

function handleDelete(slot) {
  let holdTimer = setTimeout(() => {
    startFlashingRed(slot);
  }, 500); // Start flashing red after holding for 500ms

  const slotElement = document.querySelector(`[data-slot="${slot}"]`);

  const onMouseUp = () => {
    clearTimeout(holdTimer);
    if (deleteMode) {
      deleteColor(slot); // Delete the color if it's flashing red
      stopFlashingRed(slot);
    }
    document.removeEventListener('mouseup', onMouseUp); // Ensure the event listener is removed
  };

  slotElement.addEventListener('mouseup', onMouseUp, { once: true }); // Attach the event listener to the slot
}

function showHueQuadrantDropdown(slot) {
  closeDropdown(); // Ensure previous dropdown is closed

  const hueQuadrants = [
    { value: 0, color: 'hsl(0, 100%, 50%)' },    // 0° - 90° (Red to Yellow)
    { value: 90, color: 'hsl(90, 100%, 50%)' },  // 90° - 180° (Green to Teal)
    { value: 180, color: 'hsl(180, 100%, 50%)' },// 180° - 270° (Cyan to Blue)
    { value: 270, color: 'hsl(270, 100%, 50%)' } // 270° - 360° (Purple to Pink)
  ];

  const hueQuadrantDropdown = createDropdown(hueQuadrants, (hueQuadrantValue) => {
    showHueDropdown(slot, hueQuadrantValue);
  });

  document.body.appendChild(hueQuadrantDropdown);
  positionDropdown(hueQuadrantDropdown, slot);
  activeDropdown = hueQuadrantDropdown;
}

function showHueDropdown(slot, hueQuadrantValue) {
  closeDropdown(); // Close previous dropdown

  const hues = [];
  for (let i = 0; i < 4; i++) {
    const hue = hueQuadrantValue + (i * 11.25); // 11.25° steps within the quadrant
    hues.push({ value: hue, color: `hsl(${hue}, 100%, 50%)` });
  }

  const hueDropdown = createDropdown(hues, (refinedHueValue) => {
    showSaturationDropdown(slot, refinedHueValue);
  });

  document.body.appendChild(hueDropdown);
  positionDropdown(hueDropdown, slot);
  activeDropdown = hueDropdown;
}

function showSaturationDropdown(slot, refinedHueValue) {
  closeDropdown(); // Close previous dropdown

  const saturations = [
    { value: 100, color: `hsl(${refinedHueValue}, 100%, 50%)` }, // Full saturation
    { value: 75, color: `hsl(${refinedHueValue}, 66%, 50%)` },  // 75% saturation
    { value: 50, color: `hsl(${refinedHueValue}, 33%, 50%)` },  // 50% saturation
    { value: 25, color: `hsl(${refinedHueValue}, 0%, 50%)` }   // 25% saturation
  ];

  const saturationDropdown = createDropdown(saturations, (saturationValue) => {
    showBrightnessDropdown(slot, refinedHueValue, saturationValue);
  });

  document.body.appendChild(saturationDropdown);
  positionDropdown(saturationDropdown, slot);
  activeDropdown = saturationDropdown;
}

function showBrightnessDropdown(slot, refinedHueValue, saturationValue) {
  closeDropdown(); // Close the previous dropdown

  const brightnesses = [
    { value: 75, color: `hsl(${refinedHueValue}, ${saturationValue}%, 50%)` },
    { value: 50, color: `hsl(${refinedHueValue}, ${saturationValue}%, 33%)` },
    { value: 35, color: `hsl(${refinedHueValue}, ${saturationValue}%, 13%)` },
    { value: 20, color: `hsl(${refinedHueValue}, ${saturationValue}%, 0%)` }
  ];

  const brightnessDropdown = createDropdown(brightnesses, (_, finalColor) => {
    colorset[slot] = finalColor; // Update the color in the colorset array
    renderSlots(); // Re-render the slots to reflect the change
    closeDropdown(); // Close the dropdown after selection
  });

  document.body.appendChild(brightnessDropdown);
  positionDropdown(brightnessDropdown, slot);
  activeDropdown = brightnessDropdown;
}

function positionDropdown(dropdown, slot) {
  const slotElement = document.querySelector(`[data-slot="${slot}"]`);
  const rect = slotElement.getBoundingClientRect();
  dropdown.style.top = `${rect.bottom + window.scrollY + 10}px`;
  dropdown.style.left = `${rect.left + window.scrollX}px`;
}

function closeDropdown() {
  if (activeDropdown) {
    activeDropdown.remove();
    activeDropdown = null;
  }
}

document.addEventListener('DOMContentLoaded', () => {
  initializeColorset(); // Initialize and render the colorset
});

