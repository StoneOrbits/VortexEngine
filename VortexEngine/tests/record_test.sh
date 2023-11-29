#!/bin/bash

VORTEX="../vortex"

FILE=$1
VALIDATE=$2
TESTCOUNT=$3
NUMFILES=$4
QUIET=$5

if [ "$FILE" == "" ]; then
  echo "$0 <test file> <validate> <total tests>"
  exit 1
fi

if [ "$VALIDATE" == "" ]; then
  VALIDATE=0
fi

if [ "$NUMFILES" == "" ]; then
  NUMFILES=1
fi

INPUT="$(grep "Input=" $FILE | cut -d= -f2)"
BRIEF="$(grep "Brief=" $FILE | cut -d= -f2)"
ARGS="$(grep "Args=" $FILE | cut -d= -f2)"
TESTNUM="$(echo $FILE | cut -d/ -f2 | cut -d_ -f1 | cut -d/ -f2)"
TESTNUM=$((10#$TESTNUM))

if [ "$QUIET" -eq 0 ]; then 
  echo -e -n "\e[31mRecording $PROJECT ($TESTCOUNT/$NUMFILES) \e[33m[\e[97m$BRIEF\e[33m] \e[33m[\e[97m$ARGS\e[33m]...\e[0m"
fi
TEMP_FILE="tmp/${FILE}.out"
# Append the output of the $VORTEX command to the temp file
# NOTE: When recording the tests we don't use valgrind because
#       the valgrind output should be clean anyway. But when running
#       the test valgrind is used with --leak-check=full --show-leak-kinds=all
echo "Input=${INPUT}" > "$TEMP_FILE"
echo "Brief=${BRIEF}" >> "$TEMP_FILE"
echo "Args=${ARGS}" >> "$TEMP_FILE"
echo "--------------------------------------------------------------------------------" >> "$TEMP_FILE"

# strip any \r in case this was run on windows
$VORTEX $ARGS --no-timestep --hex <<< $INPUT >> $TEMP_FILE

sed -i 's/\r//g' $TEMP_FILE
# Replace the original file with the modified temp file
mv $TEMP_FILE $FILE
if [ "$QUIET" -eq 0 ]; then 
  echo -e "\e[96mOK\e[0m"
else
  echo -n "."
fi
if [ "$VALIDATE" -eq 1 ]; then
  $VORTEX $ARGS --no-timestep --color <<< $INPUT
  echo -e "\e[31mRecorded \e[33m[\e[97m$BRIEF\e[33m] \e[33m[\e[97m$ARGS\e[33m]\e[0m"
  echo -en "${YELLOW}Is this correct? (Y/n):${WHITE} "
  read -e CONFIRM
  if [[ $CONFIRM == [nN] || $CONFIRM == [nN][oO] ]]; then
    exit
  fi
fi

