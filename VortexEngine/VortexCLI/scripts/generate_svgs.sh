#!/bin/bash

# Directory containing BMP files
BMP_FOLDER="bmp_patterns"
# Output directory for PNG files
OUTPUT_FOLDER="circular_patterns_svg"

# Create output directory if it does not exist
mkdir -p "$OUTPUT_FOLDER"

# Loop through all BMP files in the BMP_FOLDER
for bmp_file in "$BMP_FOLDER"/*.bmp; do
    filename=$(basename -- "$bmp_file")
    output_filename="${filename%.bmp}.svg"
    output_path="$OUTPUT_FOLDER/$output_filename"

    # Call the Python script to generate PNG from BMP
    python convert_bmp_to_svg.py "$bmp_file" "$output_path"
done
