#!/bin/bash

FILE_PATH="$HOME/.arduino15/packages/adafruit/hardware/samd/1.7.14/cores/arduino/USB/CDC.cpp"

# Check if the file exists
if [ ! -f "$FILE_PATH" ]; then
    echo "Error: File does not exist."
fi
# Read the specific line and check its content
CURRENT_LINE=$(sed -n '258p' "$FILE_PATH")

if [ "$CURRENT_LINE" != "	delay(10);" ]; then
    echo "No changes made: line 258 does not match the expected content or has already been modified."
    exit 0
fi

# Replace the content of line 258
sed -i '258s/.*/	\/\/delay(10);/' "$FILE_PATH"

echo "Line 258, a delay(10), has been commented out in: [$FILE_PATH]"
