#!/bin/bash

# need megatinycore installed for this

if [ "$(uname -o)" == "Msys" ]; then
	AVR_SIZE="C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/avr8/avr8-gnu-toolchain/bin/avr-size.exe"
else
	AVR_SIZE="${HOME}/atmel_setup/avr8-gnu-toolchain-linux_x86_64/bin/avr-size"
fi

# Replace this with the path to your .elf file
ELF_FILE=$1

if [ "$ELF_FILE" == "" ]; then
  echo "Please specify a file: $0 <file>"
  exit 1
fi

# Constants for program storage and dynamic memory size
PROGRAM_STORAGE=32768
DYNAMIC_MEMORY=2048

# Run avr-size and parse the output
if [ "$(uname -o)" == "Msys" ]; then
	OUTPUT=$("$AVR_SIZE" -A $ELF_FILE)
else
	OUTPUT=$($AVR_SIZE -A $ELF_FILE)
fi

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
echo "Sketch uses $PROGRAM_STORAGE_USED bytes ($PROGRAM_STORAGE_PERCENT%) of program storage space. Maximum is $PROGRAM_STORAGE bytes. (Hex: $(printf '%x' $PROGRAM_STORAGE_USED)/$(printf '%x' $PROGRAM_STORAGE))"
echo "Global variables use $DYNAMIC_MEMORY_USED bytes ($DYNAMIC_MEMORY_PERCENT%) of dynamic memory, leaving $(($DYNAMIC_MEMORY - $DYNAMIC_MEMORY_USED)) bytes for local variables. Maximum is $DYNAMIC_MEMORY bytes. (Hex: $(printf '%x' $DYNAMIC_MEMORY_USED)/$(printf '%x' $DYNAMIC_MEMORY))"
