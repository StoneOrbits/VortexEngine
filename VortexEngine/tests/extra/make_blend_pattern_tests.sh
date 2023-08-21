#!/bin/bash

# this is the list of pattern ids to create, just create id 25 (blend)
patterns=(25)
# the list of colorsets to generate tests for
colorsets=("red,green,blue" "red,yellow,white" "blue,orange,green,red")
# the max value of the args
max_value=3
# the folder to make a test in
TARGETREPO=blend_pattern

create_test_script_path="./create_test.sh -n -m --${TARGETREPO}"  # Update the path to your create_test.sh script if needed

NEEDONE=1

arg1=1
arg2=1
arg3=0
arg4=0
arg5=0

# Generate tests
for pattern in ${patterns[@]}; do
  for colorset in ${colorsets[@]}; do
		for arg6 in $(seq 0 200); do
			for arg7 in $(seq 0 3); do
				if [[ $arg1 -eq 0 && $arg4 -eq 0 && $NEEDONE -eq 0 ]];then
					continue
				fi
				NEEDONE=0

				# Create the command string
				command_string="-P${pattern} -C${colorset} -A${arg1},${arg2},${arg3},${arg4},${arg5},${arg6},${arg7}"

				# Create the test name
				test_name="Pattern_${pattern}_Colorset_${colorset//,/}_Args_${arg1}_${arg2}_${arg3}_${arg4}_${arg5}_${arg6}_${arg7}"

				# Create the test description
				test_descr="Test for pattern ${pattern}, colorset ${colorset} and arguments ${arg1}, ${arg2}, ${arg3}, ${arg4}, ${arg5}, ${arg6}, ${arg7}"

				# Create the test, need a lot of results to verify some slower blends
				echo -e "${command_string}\nw1000q\n${test_name}\n${test_descr}\n" | $create_test_script_path
			done
		done
	done
done
