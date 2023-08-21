#!/bin/bash

# Variables
patterns=(0)
colorsets=("red" "red,green" "red,green,blue")
max_args=5
max_value=3
TARGETREPO=basic_pattern
create_test_script_path="./create_test.sh -n -m --${TARGETREPO}"  # Update the path to your create_test.sh script if needed

# 1 core
# 2 gloves
# 3 orbit
# 4 handle
# 5 duo
# 6 duo_basicpattern

NEEDONE=1

# Generate tests
for pattern in ${patterns[@]}; do
  for colorset in ${colorsets[@]}; do
    for arg1 in $(seq 0 $max_value); do
      for arg2 in $(seq 0 $max_value); do
        for arg3 in $(seq 0 $max_value); do
          for arg4 in $(seq 0 $max_value); do
            for arg5 in $(seq 0 $max_value); do
              if [[ $arg1 -eq 0 && $arg4 -eq 0 && $NEEDONE -eq 0 ]];then
								NEEDONE=0
                continue
              fi

              # Create the command string
              command_string="-P${pattern} -C${colorset} -A${arg1},${arg2},${arg3},${arg4},${arg5}"

              # Create the test name
              test_name="Pattern_${pattern}_Colorset_${colorset//,/}_Args_${arg1}_${arg2}_${arg3}_${arg4}_${arg5}"

              # Create the test description
              test_descr="Test for pattern ${pattern}, colorset ${colorset} and arguments ${arg1}, ${arg2}, ${arg3}, ${arg4}, ${arg5}"

              # Create the test, only need 500 or so to verify a blink with confidence
              echo -e "${command_string}\nw500q\n${test_name}\n${test_descr}\n" | $create_test_script_path
            done
          done
        done
      done
    done
  done
done

