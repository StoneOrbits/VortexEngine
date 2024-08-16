let activeDropdown = null;
let slots = 5; // Start with 5 slots filled (including the add button)

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
        moveAddButton(slot);
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
    const addButton = document.querySelector('.add-slot');
    const currentSlot = document.getElementById(`slot${slot}`);
    
    // Make the current slot a filled slot
    currentSlot.classList.remove('empty');
    currentSlot.classList.remove('add-slot');
    currentSlot.innerHTML = '';

    // Increment the slots count
    slots++;
    
    if (slots <= 8) {
        const nextSlot = document.getElementById(`slot${slots}`);
        if (nextSlot) {
            nextSlot.classList.remove('empty');
            nextSlot.classList.add('add-slot');
            nextSlot.innerHTML = '<div class="plus-icon">+</div>';
        }
    }

    if (slots === 8) {
        // Remove the add button if we've reached the maximum number of colors
        addButton.remove();
    }
}

function startFlashingRed(slot) {
    const slotElement = document.getElementById(`slot${slot}`);
    slotElement.style.animation = 'flashRed 1s infinite';
}

function stopFlashingRed(slot) {
    const slotElement = document.getElementById(`slot${slot}`);
    slotElement.style.animation = '';
}

function deleteSlot(slot) {
    const slotElement = document.getElementById(`slot${slot}`);
    slotElement.style.backgroundColor = '';
    slotElement.classList.add('empty');

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

    slots--;

    if (slots < 8) {
        const nextSlot = document.getElementById(`slot${slots + 1}`);
        nextSlot.classList.remove('empty');
        nextSlot.classList.add('add-slot');
        nextSlot.innerHTML = '<div class="plus-icon">+</div>';
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
        const slotElement = document.getElementById(`slot${index + 1}`);
        if (slotElement.style.animation) {
            deleteSlot(index + 1); // Delete the slot if it's flashing red
        } else {
            stopFlashingRed(index + 1); // Otherwise, stop flashing
        }
    });

    slot.addEventListener('mouseleave', () => {
        clearTimeout(holdTimer);
        stopFlashingRed(index + 1);
    });
});

function editColor(slot) {
    closeDropdown(); // Ensure no other dropdowns are open
    showHueQuadrantDropdown(slot);
}

