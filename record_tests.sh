#!/bin/bash

echo "Recording tests..."

# cd into the tests folder
cd VortexEngine/tests

# clean the build
make -C .. clean

# record the tests
./recordtests.sh -q --general

# check for diffs
if git diff --quiet; then
  echo "No test changes"
else
  # add all the files in the tests folder
  git add tests_general/* &> /dev/null
fi

# check if we are in a merge
git rev-parse --verify MERGE_HEAD &> /dev/null
if [ $? -eq 0 ]; then
  read -p "${YELLOW}merge --continue? (y/n): ${NC}" confirm
  if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
    # commit all the modified tests, if any
    git merge --continue
  fi
else
  read -p "${YELLOW}Commit? (y/n): ${NC}" confirm
  if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
    # commit all the modified tests, if any
    git commit -m "recorded tests"
  fi
fi

# cd back
cd ../..

