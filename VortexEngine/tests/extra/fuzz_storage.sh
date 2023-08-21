LGRIND="valgrind --quiet --leak-check=full --show-leak-kinds=all --error-exitcode=1"
VORTEX="../vortex"
NUM_WORKERS=8
NUM_JOBS=1000
PIDS=()
FLAG_FILE="fuzz_failure.flag"
rm -f "$FLAG_FILE"

function cleanup() {
  rm -f FlashStorage_*.flash valgrind_output_*.txt
}

trap 'cleanup; kill ${PIDS[*]} 2>/dev/null' EXIT

function generate_random_file()
{
  local file="FlashStorage_${BASHPID}.flash"
  dd if=/dev/urandom of="$file" bs=1 count=384 2>/dev/null
  echo "$file"
}

function fuzz() {
  local worker_id=$1
  for i in $(seq 1 $NUM_JOBS); do
    local file=$(generate_random_file)
    VALGRIND_OUTPUT="valgrind_output_${BASHPID}.txt"
    echo -e "Worker $worker_id - Running test $i with file: $file"
    if ! $VALGRIND $VORTEX --storage="$file" --no-timestep --hex <<< "w100q" &> "$VALGRIND_OUTPUT"; then
      if [ ! -e "$FLAG_FILE" ]; then
        touch "$FLAG_FILE"
        echo -e "Valgrind run failed on worker $worker_id, test $i with file: $file"
        cat "$VALGRIND_OUTPUT"
      fi
      kill ${PIDS[*]} 2>/dev/null
      break
    fi
    rm -f "$file" "$VALGRIND_OUTPUT"
  done
}

# Start multiple fuzzing workers in the background
for i in $(seq 1 $NUM_WORKERS); do
  fuzz $i &
  PIDS+=("$!")
done

# Wait for all background workers to finish (if any fail)
wait

# Final cleanup after all jobs are done
cleanup

