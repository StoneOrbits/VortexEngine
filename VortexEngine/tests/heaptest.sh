#!/bin/bash
#
# Heap tests
#
# Runs each test in heap_tests.txt on a slim build of the cli, which is the
# engine exactly as the duo builds it, with the heap limited to what the
# attiny3217 has (see HEAP_TRACKING_DEVICE_SIZE). A test fails if any engine
# allocation fails, the output says whether it failed because the heap was
# fragmented (enough free bytes but no single piece big enough) or full.
#
# Usage: ./heaptest.sh [-v] [-n] [--max-heap=<size>]
#   -v               print the heap stats of every test
#   -n               don't rebuild the slim cli
#   --max-heap=size  heap size to test with instead of the device size
#

cd "$(dirname "$0")" || exit 1

BUILD="heap_build"
VORTEX="$BUILD/vortex"
TESTS="heap_tests.txt"
MAXHEAP="device"
VERBOSE=0
NOMAKE=0

for arg in "$@"; do
  case "$arg" in
    -v) VERBOSE=1 ;;
    -n) NOMAKE=1 ;;
    --max-heap=*) MAXHEAP="${arg#*=}" ;;
  esac
done

if [ $NOMAKE -eq 0 ]; then
  echo -e -n "\e[33mBuilding slim Vortex...\e[0m"
  mkdir -p $BUILD
  # this is a separate build from the regular cli because VORTEX_LIB_SLIM
  # changes the output of the regular tests
  FLAGS="-O1 -g -fcheck-new -D VORTEX_LIB -D VORTEX_LIB_SLIM -I ../VortexLib -I ../src"
  SOURCES="$(find ../src -name '*.cpp') ../VortexLib/VortexLib.cpp ../VortexCLI/VortexCLI.cpp"
  if ! echo $SOURCES | tr ' ' '\n' | \
      xargs -P "$(nproc)" -I{} sh -c "g++ $FLAGS -c {} -o $BUILD/\$(echo {} | sed 's,^\.\./,,; s,/,_,g').o" ||
      ! g++ $BUILD/*.o -o $VORTEX; then
    echo -e "\e[31mFailed to build slim Vortex!\e[0m"
    exit 1
  fi
  echo -e "\e[32mSuccess\e[0m"
fi

NUMTESTS=$(grep -c "^[^#]" $TESTS)
echo -e "\e[33m== [\e[97mRUNNING $NUMTESTS HEAP TESTS (max heap: $MAXHEAP)\e[33m] ==\e[0m"

ALLSUCCESS=1
TESTCOUNT=0
while IFS='|' read -r INPUT BRIEF; do
  INPUT="$(echo $INPUT)"
  BRIEF="$(echo $BRIEF)"
  TESTCOUNT=$((TESTCOUNT + 1))
  echo -e -n "\e[33mTesting heap ($TESTCOUNT/$NUMTESTS) [\e[97m$BRIEF\e[33m] ... \e[0m"
  OUTPUT="$($VORTEX --silent --no-timestep --max-heap=$MAXHEAP <<< "$INPUT" 2>&1)"
  FAILURES="$(echo "$OUTPUT" | grep "^HEAP: failed")"
  if [ -z "$FAILURES" ]; then
    echo -e "\e[32mSUCCESS\e[0m"
  else
    echo -e "\e[31mFAILURE\e[0m"
    ALLSUCCESS=0
    # only show the first failure, the rest are usually the same
    echo "    $(echo "$FAILURES" | head -n 1)"
  fi
  if [ $VERBOSE -eq 1 ]; then
    echo "$OUTPUT" | grep "^HEAP" | sed -E 's/^HEAP \[[^]]*\]/HEAP/; s/^/    /'
  fi
done < <(grep "^[^#]" $TESTS)

if [ $ALLSUCCESS -eq 1 ]; then
  echo -e "\e[33m== [\e[32mSUCCESS ALL HEAP TESTS PASSED\e[33m] ==\e[0m"
else
  echo -e "\e[31m== FAILURE ==\e[0m"
  exit 1
fi
