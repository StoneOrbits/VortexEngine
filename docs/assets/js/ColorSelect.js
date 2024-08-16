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
            console.log(`Hue quadrant selected: ${option.value}°`); // Debugging output
            closeDropdown(); // Close after selecting a quadrant
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
        // Placeholder to move to the next step
    });

    document.body.appendChild(hueQuadrantDropdown);
    positionDropdown(hueQuadrantDropdown, slot);
    activeDropdown = hueQuadrantDropdown;
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

