#!/bin/bash
#
# Input fuzzer
#
# Feeds random input strings to a slim heap tracking build of the cli (the
# engine exactly as the duo builds it, see ../heaptest.sh) that is also built
# with AddressSanitizer and UndefinedBehaviorSanitizer. A run fails if:
#   - the sanitizers report a memory error (overflow, use after free, double
#     free, leak..) or undefined behaviour, the free blocks of the memory pools
#     and the free space of the ByteStream arena are poisoned in this build so
#     those are covered too (see MEMORY_POISON in Memory.h)
#   - the heap tracker reports a problem, like a pool running out of device
#     blocks or the ByteStream arena overflowing the attiny3217 (see --max-heap)
#   - the cli crashes or exits with an error
#
# Every failing input and its output is saved to fuzz_failures/ so it can be
# replayed with: fuzz_build/vortex --no-timestep --autowake --max-heap=device < input
#
# Usage: ./fuzzer.sh [-n] [--workers=N] [--jobs=N] [--length=N] [--max-heap=size]
#   -n               don't rebuild the fuzz cli
#   --workers=N      number of parallel workers (default: nproc)
#   --jobs=N         inputs each worker runs (default: 200)
#   --length=N       max commands in each input (default: 300)
#   --max-heap=size  arena size to fuzz with, 'device' for the attiny3217 or
#                    'none' to only look for memory errors (default: device)
#

cd "$(dirname "$0")" || exit 1

BUILD="fuzz_build"
VORTEX="$BUILD/vortex"
FAILDIR="fuzz_failures"
NUM_WORKERS=$(nproc)
NUM_JOBS=200
MAX_LENGTH=300
MAXHEAP="device"
NOMAKE=0

for arg in "$@"; do
  case "$arg" in
    -n) NOMAKE=1 ;;
    --workers=*) NUM_WORKERS="${arg#*=}" ;;
    --jobs=*) NUM_JOBS="${arg#*=}" ;;
    --length=*) MAX_LENGTH="${arg#*=}" ;;
    --max-heap=*) MAXHEAP="${arg#*=}" ;;
  esac
done

HEAPARG="--max-heap=$MAXHEAP"
if [ "$MAXHEAP" == "none" ]; then
  HEAPARG=""
fi

# stop on the first error of each run and make it an error exit
export ASAN_OPTIONS="detect_leaks=1:abort_on_error=0:exitcode=99"
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1:exitcode=98"

PIDS=()
trap 'kill ${PIDS[*]} 2>/dev/null' EXIT

# the commands the cli takes on stdin, the ones that pass time are listed more
# than once so the engine gets time to reach the deeper menus and timers
COMMANDS=(c c c c l l m m a d s f t t w w w w r v H)

# the number of default modes, v<N> with N past the end is ignored by the cli
# so a few of those are thrown in too
NUM_DEFAULT_MODES=9

function generate_random_input()
{
  local length="$((RANDOM % MAX_LENGTH + 1))"
  local input=""
  for ((i = 0; i < length; i++)); do
    local cmd="${COMMANDS[RANDOM % ${#COMMANDS[@]}]}"
    case "$cmd" in
      # rapid click needs a click count
      r) input+="r$((RANDOM % 20 + 1))" ;;
      # another device sends a default mode over VL
      v) input+="v$((RANDOM % (NUM_DEFAULT_MODES + 2)))" ;;
      # waits are short or long enough to hold the button through menus
      w) input+="w$((RANDOM % 4 == 0 ? RANDOM % 500 + 1 : RANDOM % 30 + 1))" ;;
      *)
        input+="$cmd"
        local count="$((RANDOM % 8))"
        if [ "$count" -gt 1 ]; then
          input+="$count"
        fi
        ;;
    esac
  done
  # make sure the button isn't left pressed then quit
  input+="w100q"
  echo "$input"
}

function fuzz()
{
  local worker_id=$1
  for i in $(seq 1 $NUM_JOBS); do
    local input
    input=$(generate_random_input)
    local output="$FAILDIR/worker${worker_id}_${i}.out"
    "$VORTEX" --no-timestep --silent --autowake $HEAPARG <<< "$input" &> "$output"
    local result=$?
    # the heap stats lines (HEAP [label]: ...) are from H, anything else from
    # the heap tracker is a problem
    local problem
    problem="$(grep -m 1 -E "^HEAP:|ERROR: AddressSanitizer|ERROR: LeakSanitizer|runtime error:" "$output")"
    if [ $result -ne 0 ] || [ -n "$problem" ]; then
      echo "$input" > "$FAILDIR/worker${worker_id}_${i}.input"
      echo -e "\e[31mWorker \e[97m$worker_id\e[31m test \e[97m$i\e[31m failed (exit $result): \e[97m${problem:-crash}\e[0m"
      echo -e "    saved to \e[97m$FAILDIR/worker${worker_id}_${i}.input\e[0m"
    else
      rm -f "$output"
    fi
    if [ $((i % 50)) -eq 0 ]; then
      echo -e "\e[33mWorker \e[97m$worker_id\e[33m ran \e[97m$i\e[33m/$NUM_JOBS\e[0m"
    fi
  done
}

if [ $NOMAKE -eq 0 ]; then
  echo -e -n "\e[33mBuilding fuzz Vortex...\e[0m"
  rm -rf $BUILD
  mkdir -p $BUILD
  FLAGS="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -D VORTEX_LIB -D VORTEX_LIB_SLIM -D HEAP_TRACKING=1 -I ../../VortexLib -I ../../src"
  SOURCES="$(find ../../src -name '*.cpp') ../../VortexLib/VortexLib.cpp ../../VortexCLI/VortexCLI.cpp"
  if ! echo $SOURCES | tr ' ' '\n' | \
      xargs -P "$(nproc)" -I{} sh -c "g++ $FLAGS -c {} -o $BUILD/\$(echo {} | sed 's,^\.\./\.\./,,; s,/,_,g').o" ||
      ! g++ -fsanitize=address,undefined $BUILD/*.o -o $VORTEX; then
    echo -e "\e[31mFailed to build fuzz Vortex!\e[0m"
    exit 1
  fi
  echo -e "\e[32mSuccess\e[0m"
fi
if [ ! -x "$VORTEX" ]; then
  echo -e "\e[31mCould not find $VORTEX!\e[0m"
  exit 1
fi

rm -rf $FAILDIR
mkdir -p $FAILDIR

echo -e "\e[33m== [\e[97mFUZZING $NUM_WORKERS WORKERS x $NUM_JOBS INPUTS (max heap: $MAXHEAP)\e[33m] ==\e[0m"
for i in $(seq 1 $NUM_WORKERS); do
  fuzz $i &
  PIDS+=("$!")
done
wait
PIDS=()

NUMFAILS=$(ls $FAILDIR/*.input 2>/dev/null | wc -l)
if [ "$NUMFAILS" -eq 0 ]; then
  rmdir $FAILDIR
  echo -e "\e[33m== [\e[32mSUCCESS NO FAILURES\e[33m] ==\e[0m"
else
  echo -e "\e[31m== $NUMFAILS FAILURES, see $FAILDIR/ ==\e[0m"
  exit 1
fi
