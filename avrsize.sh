#!/bin/bash

# Replace this with the path to your avr-size executable
AVR_SIZE="C:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\DxCore\\tools\\avr-gcc\\7.3.0-atmel3.6.1-azduino6/bin/avr-size"

# Replace this with the path to your .elf file
ELF_FILE="main.elf"

# Constants for program storage and dynamic memory size
PROGRAM_STORAGE=32768
DYNAMIC_MEMORY=2048

# Run avr-size and parse the output
OUTPUT=$($AVR_SIZE -A $ELF_FILE)

# Extract sizes of .text, .data, .rodata, and .bss sections
TEXT_SIZE=$(echo "$OUTPUT" | grep -E '\.text' | awk '{print $2}')
DATA_SIZE=$(echo "$OUTPUT" | grep -E '\.data' | awk '{print $2}')
RODATA_SIZE=$(echo "$OUTPUT" | grep -E '\.rodata' | awk '{print $2}')
BSS_SIZE=$(echo "$OUTPUT" | grep -E '\.bss' | awk '{print $2}')

# Calculate used program storage and dynamic memory
PROGRAM_STORAGE_USED=$((TEXT_SIZE + DATA_SIZE + RODATA_SIZE))
DYNAMIC_MEMORY_USED=$((DATA_SIZE + BSS_SIZE))

# Calculate percentages
PROGRAM_STORAGE_PERCENT=$(awk -v used="$PROGRAM_STORAGE_USED" -v total="$PROGRAM_STORAGE" 'BEGIN { printf("%.2f", used / total * 100) }')
DYNAMIC_MEMORY_PERCENT=$(awk -v used="$DYNAMIC_MEMORY_USED" -v total="$DYNAMIC_MEMORY" 'BEGIN { printf("%.2f", used / total * 100) }')

# Display the results
echo "Sketch uses $PROGRAM_STORAGE_USED bytes ($PROGRAM_STORAGE_PERCENT%) of program storage space. Maximum is $PROGRAM_STORAGE bytes."
echo "Global variables use $DYNAMIC_MEMORY_USED bytes ($DYNAMIC_MEMORY_PERCENT%) of dynamic memory, leaving $(($DYNAMIC_MEMORY - $DYNAMIC_MEMORY_USED)) bytes for local variables. Maximum is $DYNAMIC_MEMORY bytes."
