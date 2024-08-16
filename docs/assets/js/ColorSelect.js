let activeDropdown = null;

function createDropdown(options, onSelect) {
    const dropdown = document.createElement('div');
    dropdown.style.position = 'absolute';
    dropdown.style.backgroundColor = '#333';
    dropdown.style.border = '1px solid #777';
    dropdown.style.padding = '10px';
    dropdown.style.display = 'flex';
    dropdown.style.gap = '10px';
    dropdown.style.borderRadius = '8px'; // Round corners

    options.forEach((option) => {
        const box = document.createElement('div');
        box.style.width = '40px';
        box.style.height = '40px';
        box.style.backgroundColor = option.color;
        box.style.cursor = 'pointer';
        box.style.borderRadius = '8px'; // Round corners for boxes
        box.style.border = '2px solid #555'; // Add a border to the boxes

        box.onclick = function() {
            onSelect(option.color, option.value);
            closeDropdown(dropdown);
        };

        dropdown.appendChild(box);
    });

    return dropdown;
}

function closeDropdown(dropdown) {
    if (dropdown) {
        dropdown.remove();
        activeDropdown = null;
    }
}

function editColor(slot) {
    if (deleteMode) return;

    if (activeDropdown) closeDropdown(activeDropdown); // Close any open dropdown

    const hueQuadrants = [
        { value: '1', color: '#ff0000' }, // Red
        { value: '2', color: '#00ff00' }, // Green
        { value: '3', color: '#0000ff' }, // Blue
        { value: '4', color: '#ffff00' }, // Yellow
    ];

    // Create the first dropdown for hue quadrants
    const hueQuadrantDropdown = createDropdown(hueQuadrants, function(selectedColor, hueQuadrantValue) {
        // Proceed to the next step: Hue selection
        const hues = [
            { value: '1', color: adjustHue(hueQuadrantValue, 0) },
            { value: '2', color: adjustHue(hueQuadrantValue, 1) },
            { value: '3', color: adjustHue(hueQuadrantValue, 2) },
            { value: '4', color: adjustHue(hueQuadrantValue, 3) },
        ];

        const hueDropdown = createDropdown(hues, function(selectedColor, hueValue) {
            // Proceed to the next step: Saturation selection
            const saturations = [
                { value: '1', color: adjustSaturation(selectedColor, 1) },
                { value: '2', color: adjustSaturation(selectedColor, 0.75) },
                { value: '3', color: adjustSaturation(selectedColor, 0.5) },
                { value: '4', color: adjustSaturation(selectedColor, 0.25) },
            ];

            const saturationDropdown = createDropdown(saturations, function(selectedColor, saturationValue) {
                // Proceed to the final step: Brightness selection
                const brightnesses = [
                    { value: '1', color: adjustBrightness(selectedColor, 1) },
                    { value: '2', color: adjustBrightness(selectedColor, 0.75) },
                    { value: '3', color: adjustBrightness(selectedColor, 0.5) },
                    { value: '4', color: adjustBrightness(selectedColor, 0.25) },
                ];

                const brightnessDropdown = createDropdown(brightnesses, function(finalColor) {
                    document.getElementById(`slot${slot}`).style.backgroundColor = finalColor;
                    colors[slot - 1] = finalColor;
                    closeDropdown(brightnessDropdown);
                });

                document.body.appendChild(brightnessDropdown);
                positionDropdown(brightnessDropdown, slot);
                activeDropdown = brightnessDropdown;
            });

            document.body.appendChild(saturationDropdown);
            positionDropdown(saturationDropdown, slot);
            activeDropdown = saturationDropdown;
        });

        document.body.appendChild(hueDropdown);
        positionDropdown(hueDropdown, slot);
        activeDropdown = hueDropdown;
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

function adjustHue(hueQuadrant, hueIndex) {
    const hues = {
        '1': ['#ff0000', '#ff3333', '#ff6666', '#ff9999'],
        '2': ['#00ff00', '#33ff33', '#66ff66', '#99ff99'],
        '3': ['#0000ff', '#3333ff', '#6666ff', '#9999ff'],
        '4': ['#ffff00', '#ffff33', '#ffff66', '#ffff99'],
    };
    return hues[hueQuadrant][hueIndex];
}

function adjustSaturation(hex, saturation) {
    const { r, g, b } = hexToRgb(hex);
    const gray = 128; // Middle gray value
    const adjust = (color) => Math.round(gray + (color - gray) * saturation);
    return `rgb(${adjust(r)}, ${adjust(g)}, ${adjust(b)})`;
}

function adjustBrightness(hex, brightness) {
    const { r, g, b } = hexToRgb(hex);
    const adjust = (color) => Math.round(color * brightness);
    return `rgb(${adjust(r)}, ${adjust(g)}, ${adjust(b)})`;
}

function hexToRgb(hex) {
    const bigint = parseInt(hex.slice(1), 16);
    const r = (bigint >> 16) & 255;
    const g = (bigint >> 8) & 255;
    const b = bigint & 255;
    return { r, g, b };
}
