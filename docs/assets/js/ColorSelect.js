let activeDropdown = null;
let filledSlots = 4; // Start with 4 filled slots
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
  closeDropdown(); // Close previous dropdown

  const brightnesses = [
    { value: 75, color: `hsl(${refinedHueValue}, ${saturationValue}%, 50%)` }, // 100% brightness
    { value: 50, color: `hsl(${refinedHueValue}, ${saturationValue}%, 33%)` }, // 50% brightness
    { value: 35, color: `hsl(${refinedHueValue}, ${saturationValue}%, 13%)` }, // 35% brightness
    { value: 20, color: `hsl(${refinedHueValue}, ${saturationValue}%, 0%)` }  // 20% brightness
  ];

  const brightnessDropdown = createDropdown(brightnesses, function(_, finalColor) {
    document.getElementById(`slot${slot}`).style.backgroundColor = finalColor;
    if (slot === filledSlots + 1 && filledSlots < 8) {
      moveAddButton(slot);
    }
    closeDropdown(); // Ensure dropdown closes after final selection
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
  const currentSlot = document.getElementById(`slot${slot}`);

  // Make the current slot a filled slot
  currentSlot.classList.remove('empty');
  currentSlot.classList.remove('add-slot');
  currentSlot.innerHTML = '';

  filledSlots++;

  if (filledSlots < 8) {
    const nextSlot = document.getElementById(`slot${filledSlots + 1}`);
    nextSlot.classList.remove('empty');
    nextSlot.classList.add('add-slot');
    nextSlot.innerHTML = '<div class="plus-icon">+</div>';
    nextSlot.onclick = function() {
      editColor(filledSlots + 1);
    };
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

function deleteSlot(slot) {
  const slotElement = document.getElementById(`slot${slot}`);
  slotElement.style.backgroundColor = '';
  slotElement.classList.add('empty');
  slotElement.classList.remove('add-slot');
  slotElement.innerHTML = ''; // Ensure no '+' sign is left

  // Move other slots up
  for (let i = slot + 1; i <= 8; i++) {
    const currentSlotElement = document.getElementById(`slot${i}`);
    const prevSlotElement = document.getElementById(`slot${i - 1}`);

    if (!currentSlotElement.classList.contains('empty')) {
      prevSlotElement.style.backgroundColor = currentSlotElement.style.backgroundColor;
      prevSlotElement.classList.remove('empty');
      prevSlotElement.innerHTML = '';
      currentSlotElement.style.backgroundColor = '';
      currentSlotElement.classList.add('empty');
    }
  }

  filledSlots--;

  if (filledSlots < 8) {
    const nextSlot = document.getElementById(`slot${filledSlots + 1}`);
    nextSlot.classList.remove('empty');
    nextSlot.classList.add('add-slot');
    nextSlot.innerHTML = '<div class="plus-icon">+</div>';
    nextSlot.onclick = function() {
      editColor(filledSlots + 1);
    };
  }
}

// Handle holding and deleting
document.querySelectorAll('.slot').forEach((slot, index) => {
  let holdTimer;

  slot.addEventListener('mousedown', () => {
    holdTimer = setTimeout(() => {
      startFlashingRed(index + 1);
    }, 500); // Start flashing red after holding for 500ms
  });

  slot.addEventListener('mouseup', () => {
    clearTimeout(holdTimer);
    if (deleteMode) {
      deleteSlot(index + 1); // Delete the slot if it's flashing red
      stopFlashingRed(index + 1);
    } else {
      editColor(index + 1); // Only open the color selection if not in delete mode
    }
  });

  slot.addEventListener('mouseleave', () => {
    clearTimeout(holdTimer);
    stopFlashingRed(index + 1);
  });
});

function editColor(slot) {
  if (!deleteMode) {
    closeDropdown(); // Ensure no other dropdowns are open
    showHueQuadrantDropdown(slot);
  }
}

