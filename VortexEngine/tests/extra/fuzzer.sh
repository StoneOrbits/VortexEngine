#!/bin/bash

VALGRIND="valgrind --quiet --leak-check=full --show-leak-kinds=all --error-exitcode=1"
VORTEX="../vortex"
NUM_WORKERS=32
NUM_JOBS=3000
PIDS=()
FLAG_FILE="fuzz_failure.flag"
rm -f "$FLAG_FILE"

trap 'kill ${PIDS[*]} 2>/dev/null' EXIT

function generate_random_input() 
{
  local length="$((RANDOM % 1000 + 1))"
  local input=""
  local letters=('c' 'l' 'm' 'a' 's' 't' 'r')

  for ((i = 0; i < length; i++)); do
    local letter="${letters[RANDOM % 4]}"
    local count="$((RANDOM % 50))"
    input+="${letter}"
    if [ "$count" -gt 1 ]; then
      input+="${count}"
    fi
  done

  input+="q"
  echo "$input"
}

function truncate_input() {
  local input=$1
  local max_length=10
  if [ ${#input} -gt $max_length ]; then
    echo "${input:0:max_length}..."
  else
    echo "$input"
  fi
}

function fuzz() {
  local test_number=0
  local worker_id=$1
  for i in $(seq 1 $NUM_JOBS); do
    test_number=$((test_number+1))
    input=$(generate_random_input)
    truncated_input=$(truncate_input "$input")
    echo -e "\e[33mWorker \e[97m$worker_id\e[33m - Running test \e[97m$test_number\e[33m with input: \e[97m$truncated_input\e[0m"
    VALGRIND_OUTPUT="valgrind_output_${BASHPID}.txt"
    if ! $VALGRIND $VORTEX --hex --no-timestep --autowake <<< "$input" &> "$VALGRIND_OUTPUT"; then
      if [ ! -e "$FLAG_FILE" ]; then
        touch "$FLAG_FILE"
        echo -e "\e[31mValgrind run failed on worker \e[97m$worker_id\e[31m, test \e[97m$test_number\e[31m with input: \e[97m$input\e[0m"
        cat "$VALGRIND_OUTPUT"
      fi
      kill ${PIDS[*]} 2>/dev/null
      break
    fi
    # cleanup the output file because it was successful
    rm $VALGRIND_OUTPUT
  done
}

# Build Vortex
echo -e -n "\e[33mBuilding Vortex...\e[0m"
make -C ../ &> /dev/null
if [ $? -ne 0 ]; then
  echo -e "\e[31mFailed to build Vortex!\e[0m"
  exit
fi
if [ ! -x "$VORTEX" ]; then
  echo -e "\e[31mCould not find Vortex!\e[0m"
  exit
fi
echo -e "\e[32mSuccess\e[0m"

# Start multiple fuzzing workers in the background
for i in $(seq 1 $NUM_WORKERS); do
  fuzz $i &
  PIDS+=("$!")
done

# Wait for all background workers to finish (if any fail)
wait
