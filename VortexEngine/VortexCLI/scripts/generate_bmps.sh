#!/bin/bash

VORTEX=../vortex
PATTERN_DIR=./default_patterns
BMP_DIR=./bmp_patterns

# Default values
CYCLE_COUNT=2
INPUT_COMMANDS="410wq"
INCLUDE_GENERIC_FLASHING_PATTERNS=0
NUM_PATTERNS=20
NUM_DEFAULT_PATTERNS=6

# Ensure bmp_patterns directory exists
mkdir -p "$BMP_DIR"

make -C ../
if [ $? -ne 0 ]; then
    echo "Failed to build helios"
    exit 1
fi

if [ ! -x "$VORTEX" ]; then
    echo "Cannot find helios program: $VORTEX"
    exit 1
fi

# Parse command-line options
while getopts ":c" opt; do
  case ${opt} in
    c )
      INPUT_COMMANDS=""
      ;;
    \? )
      echo "Invalid option: $OPTARG" 1>&2
      exit 1
      ;;
  esac
done

# Iterate over .pattern files
for pattern_file in "$PATTERN_DIR"/*.pattern; do
    filename=$(basename -- "$pattern_file")
    filename="${filename%.*}"
    COLOR_SET=$(grep "COLOR_SET=" "$pattern_file" | cut -d= -f2)
    PATTERN_ID=$(grep "PATTERN_ID=" "$pattern_file" | cut -d= -f2)
    PATTERN_ARGS=$(grep "PATTERN_ARGS=" "$pattern_file" | cut -d= -f2)
    BRIGHTNESS_SCALE=$(grep "BRIGHTNESS_SCALE=" "$pattern_file" | cut -d= -f2)

    # build the pattern args string based on whether a pattern ID or args was specified
    PATTERN_ARG_STR=
    if [ "$PATTERN_ARGS" == "" ]; then
      # just use pattern id no args specified
      PATTERN_ARG_STR="--pattern $PATTERN_ID"
    else
      # otherwise use the pattern args
      PATTERN_ARG_STR="--pattern-args $PATTERN_ARGS"
    fi

    # Use extracted parameters to generate the pattern
    if [ -z "$INPUT_COMMANDS" ]; then
        $VORTEX \
            --quiet \
            --no-timestep \
            --brightness-scale "$BRIGHTNESS_SCALE" \
            --colorset "$COLOR_SET" \
            $PATTERN_ARG_STR \
            --bmp "$BMP_DIR/${filename}.bmp" \
            --cycle "$CYCLE_COUNT"
    else
        $VORTEX \
            --quiet \
            --no-timestep \
            --brightness-scale "$BRIGHTNESS_SCALE" \
            --colorset "$COLOR_SET" \
            $PATTERN_ARG_STR \
            --bmp "$BMP_DIR/${filename}.bmp" \
            <<< "$INPUT_COMMANDS"
    fi

    if [ $? -ne 0 ]; then
        echo "Helios command failed for $filename with COLOR_SET=$COLOR_SET, PATTERN_ID=$PATTERN_ID"
    fi
done

if [ "$INCLUDE_GENERIC_FLASHING_PATTERNS" -eq 1 ]; then
  # Generate patterns 0 to 19
  for ((i = 0; i <= NUM_PATTERNS - 1; i++)); do
      echo "Generating pattern $i..."
      if [ -z "$INPUT_COMMANDS" ]; then
          $VORTEX \
              --quiet \
              --no-timestep \
              --brightness-scale "2.0" \
              --colorset "red,green,blue" \
              --pattern "$i" \
              --bmp "$BMP_DIR/$(printf "%03d_Pattern.bmp" $((i + NUM_DEFAULT_PATTERNS + 1)))" \
              --cycle "$CYCLE_COUNT"
      else
          $VORTEX \
              --quiet \
              --no-timestep \
              --brightness-scale "2.0" \
              --colorset "red,green,blue" \
              --pattern "$i" \
              --bmp "$BMP_DIR/$(printf "%03d_Pattern.bmp" $((i + NUM_DEFAULT_PATTERNS + 1)))" \
              <<< "$INPUT_COMMANDS"
      fi
  done
fi
