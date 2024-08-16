let activeDropdown = null;

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
        console.log('Hue quadrant selected:', hueQuadrantValue); // Debugging output
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
        console.log('Refined hue selected:', refinedHueValue); // Debugging output
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
        { value: 75, color: `hsl(${refinedHueValue}, 75%, 50%)` },  // 75% saturation
        { value: 50, color: `hsl(${refinedHueValue}, 50%, 50%)` },  // 50% saturation
        { value: 25, color: `hsl(${refinedHueValue}, 25%, 50%)` }   // 25% saturation
    ];

    const saturationDropdown = createDropdown(saturations, function(saturationValue) {
        console.log('Saturation selected:', saturationValue); // Debugging output
        showBrightnessDropdown(slot, refinedHueValue, saturationValue);
    });

    document.body.appendChild(saturationDropdown);
    positionDropdown(saturationDropdown, slot);
    activeDropdown = saturationDropdown;
}

function showBrightnessDropdown(slot, refinedHueValue, saturationValue) {
    closeDropdown(); // Close previous dropdown

    const brightnesses = [
        { value: 75, color: `hsl(${refinedHueValue}, ${saturationValue}%, 75%)` }, // 75% brightness
        { value: 50, color: `hsl(${refinedHueValue}, ${saturationValue}%, 50%)` }, // 50% brightness
        { value: 35, color: `hsl(${refinedHueValue}, ${saturationValue}%, 35%)` }, // 35% brightness
        { value: 20, color: `hsl(${refinedHueValue}, ${saturationValue}%, 20%)` }  // 20% brightness
    ];

    const brightnessDropdown = createDropdown(brightnesses, function(_, finalColor) {
        console.log('Brightness selected:', finalColor); // Debugging output
        document.getElementById(`slot${slot}`).style.backgroundColor = finalColor;
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

// Entry point for color selection
function editColor(slot) {
    closeDropdown(); // Ensure no other dropdowns are open
    showHueQuadrantDropdown(slot);
}

