#!/bin/bash

create_test_script_path="./create_test.sh -n -m"  # Update the path to your create_test.sh script if needed

# 1 core
# 2 gloves
# 3 orbit
# 4 handle
# 5 duo
# 6 duo_basicpattern
TARGETREPO=5

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
	echo -e "$TARGETREPO\n\n${input}q\n${name}\n${desc}\n"  | $create_test_script_path
	echo "Successfully created test ${name}"
done < duo_tests
