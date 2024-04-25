#!/bin/bash

create_test_script_path="./create_test.sh --general -n -m"  # Update the path to your create_test.sh script if needed

# 1 general
TARGET_TESTS=1

# Read the file line by line
while IFS= read -r line
do
  # Add each column to the respective array
  input=("$(echo "$line" | cut -d'|' -f1 | xargs)")
  name=("$(echo "$line" | cut -d'|' -f2 | xargs)")
  desc=("$(echo "$line" | cut -d'|' -f3 | xargs)")
	if [ -z "$name" ] || [ -z "$desc" ] || [ -z "$input" ]; then
		continue
	fi
	# Create the test
	echo -e "\n${input}\n${name}\n${desc}\n"  | $create_test_script_path
	echo "Successfully created test ${name}"
done < test_list
