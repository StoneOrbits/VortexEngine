let activeDropdown = null;
let filledSlots = 4; // Start with 4 filled slots (Red, Green, Blue, Black)
let deleteMode = false; // Track if delete mode is active

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

    box.onclick = function(event) {
      event.stopPropagation();
      onSelect(option.value, option.color);
    };

    dropdown.appendChild(box);
  });

  return dropdown;
}

function closeDropdown() {
  if (activeDropdown) {
    activeDropdown.remove();
    activeDropdown = null;
  }
}

function showHueQuadrantDropdown(slot) {
  closeDropdown(); // Ensure previous dropdown is closed

  const hueQuadrants = [
    { value: 0, color: 'hsl(0, 100%, 50%)' },    // 0° - 90° (Red to Yellow)
    { value: 90, color: 'hsl(90, 100%, 50%)' },  // 90° - 180° (Green to Teal)
    { value: 180, color: 'hsl(180, 100%, 50%)' },// 180° - 270° (Cyan to Blue)
    { value: 270, color: 'hsl(270, 100%, 50%)' } // 270° - 360° (Purple to Pink)
  ];

  const hueQuadrantDropdown = createDropdown(hueQuadrants, function(hueQuadrantValue) {
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

  const hueDropdown = createDropdown(hues, function(refinedHueValue) {
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

  const saturationDropdown = createDropdown(saturations, function(saturationValue) {
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

  const brightnessDropdown = createDropdown(brightnesses, function(_, finalColor) {
    const slotElement = document.getElementById(`slot${slot}`);

    // Apply the selected color to the slot
    slotElement.style.backgroundColor = finalColor;

    // Mark the slot as filled and ensure it's clickable
    slotElement.classList.add('color-filled');
    slotElement.style.cursor = 'pointer';
    slotElement.onclick = function() {
      editColor(slot);
    };

    closeDropdown(); // Close the dropdown after selection
  });

  document.body.appendChild(brightnessDropdown);
  positionDropdown(brightnessDropdown, slot);
  activeDropdown = brightnessDropdown;
}

function positionDropdown(dropdown, slot) {
  const slotElement = document.getElementById(`slot${slot}`);
  const rect = slotElement.getBoundingClientRect();
  dropdown.style.top = `${rect.bottom + window.scrollY + 10}px`;
  dropdown.style.left = `${rect.left + window.scrollX}px`;
}

function moveAddButton(slot) {
  const prevAddSlot = document.querySelector('.add-slot');

  // Ensure previous add slot becomes an empty slot if it exists
  if (prevAddSlot) {
    prevAddSlot.classList.remove('add-slot');
    prevAddSlot.classList.add('empty');
    prevAddSlot.style.backgroundColor = '#222'; // Darker background for empty slots
    prevAddSlot.innerHTML = '';
    prevAddSlot.style.cursor = 'default'; // Remove pointer cursor
    prevAddSlot.onclick = null; // Disable clicking on empty slots
  }

  // Find the next available empty slot
  const nextSlot = document.getElementById(`slot${slot + 1}`);
  if (nextSlot && nextSlot.classList.contains('empty')) {
    nextSlot.style.backgroundColor = '#444'; // Light background for add slot
    nextSlot.classList.remove('empty');
    nextSlot.classList.add('add-slot');
    nextSlot.innerHTML = '<div class="plus-icon">+</div>';
    nextSlot.style.cursor = 'pointer'; // Add pointer cursor for the add button

    // Delay the activation of the onclick to avoid triggering it accidentally
    setTimeout(() => {
      nextSlot.onclick = function() {
        addColor(slot + 1);
      };
    }, 300); // 300ms delay to prevent accidental click
  } else {
    // Handle the case where the last slot is filled and no more slots are available
    filledSlots++; // Increment the filledSlots count to handle full slots
  }
}

function startFlashingRed(slot) {
  const slotElement = document.getElementById(`slot${slot}`);
  slotElement.style.animation = 'flashRed 1s infinite';
  deleteMode = true;
}

function stopFlashingRed(slot) {
  const slotElement = document.getElementById(`slot${slot}`);
  slotElement.style.animation = '';
  deleteMode = false;
}

// Handle holding and deleting
document.querySelectorAll('.slot').forEach((slot, index) => {
  let holdTimer;

  slot.addEventListener('mousedown', () => {
    // If the slot is empty, do nothing
    if (slot.classList.contains('empty')) return;

    holdTimer = setTimeout(() => {
      startFlashingRed(index + 1);
    }, 500); // Start flashing red after holding for 500ms
  });

  slot.addEventListener('mouseup', () => {
    // If the slot is empty, do nothing
    if (slot.classList.contains('empty')) return;

    clearTimeout(holdTimer);
    if (deleteMode) {
      deleteSlot(index + 1); // Delete the slot if it's flashing red
      stopFlashingRed(index + 1);
    } else if (!deleteMode) {
      editColor(index + 1); // Only open the color selection if not in delete mode
    }
  });

  slot.addEventListener('mouseleave', () => {
    clearTimeout(holdTimer);
    stopFlashingRed(index + 1);
  });
});

function deleteSlot(slot) {
  const slotElement = document.getElementById(`slot${slot}`);

  // Shift all slots to the left
  for (let i = slot; i < 8; i++) {
    const currentSlotElement = document.getElementById(`slot${i}`);
    const nextSlotElement = document.getElementById(`slot${i + 1}`);

    if (nextSlotElement && !nextSlotElement.classList.contains('empty')) {
      currentSlotElement.style.backgroundColor = nextSlotElement.style.backgroundColor;
      currentSlotElement.classList.remove('empty');
      currentSlotElement.classList.remove('add-slot');
      currentSlotElement.innerHTML = '';
      currentSlotElement.style.cursor = 'pointer';
      currentSlotElement.onclick = () => editColor(i); // Ensure it's editable
    } else {
      currentSlotElement.style.backgroundColor = '#222'; // Darker background for empty slots
      currentSlotElement.classList.add('empty');
      currentSlotElement.classList.remove('add-slot');
      currentSlotElement.innerHTML = '';
      currentSlotElement.style.cursor = 'default'; // Remove pointer cursor
      currentSlotElement.onclick = null; // Disable click on the now-empty slot
    }
  }

  // Reposition the add button to the first empty slot
  filledSlots--;
  const nextSlot = document.getElementById(`slot${filledSlots + 1}`);
  if (nextSlot) {
    nextSlot.style.backgroundColor = '#444'; // Light background for add slot
    nextSlot.classList.remove('empty');
    nextSlot.classList.add('add-slot');
    nextSlot.innerHTML = '<div class="plus-icon">+</div>';
    nextSlot.style.cursor = 'pointer'; // Add pointer cursor for the add button
    // Delay the activation of the onclick to avoid triggering it accidentally
    setTimeout(() => {
      nextSlot.onclick = function() {
        editColor(filledSlots + 1);
      };
    }, 300); // 300ms delay to prevent accidental click
  }

  // Ensure that the slot after the add button is correctly updated
  for (let i = filledSlots + 2; i <= 8; i++) {
    const slotToCheck = document.getElementById(`slot${i}`);
    if (slotToCheck) {
      slotToCheck.classList.add('empty');
      slotToCheck.classList.remove('add-slot');
      slotToCheck.style.backgroundColor = '#222'; // Darker background for empty slots
      slotToCheck.innerHTML = '';
      slotToCheck.style.cursor = 'default';
      slotToCheck.onclick = null; // Disable interaction with empty slots
    }
  }
}

document.addEventListener('click', (event) => {
  if (activeDropdown && !activeDropdown.contains(event.target)) {
    closeDropdown(); // Close the dropdown if clicked outside
  }
});

function editColor(slot) {
  if (!deleteMode) {
    closeDropdown(); // Ensure no other dropdowns are open
    showHueQuadrantDropdown(slot);
    event.stopPropagation(); // Prevent this click from closing the dropdown
  }
}

function addColor(slot) {
  const currentSlot = document.getElementById(`slot${slot}`);

  // Mark this slot as filled and remove 'empty' and 'add-slot' classes
  currentSlot.classList.remove('empty');
  currentSlot.classList.remove('add-slot');
  currentSlot.classList.add('color-filled');
  currentSlot.style.cursor = 'pointer';

  // Move the add button to the next empty slot before editing
  moveAddButton(slot);

  // Now proceed to edit the color in this slot
  editColor(slot);
}
