#!/bin/bash

VALGRIND="valgrind --quiet --leak-check=full --show-leak-kinds=all"
VORTEX="../vortex"

VALIDATE=0
TODO=

declare -a REPOS
# Iterate through all the folders that start with "test_" in the current directory
for folder in tests_*/; do
  # Remove the "./tests_" prefix and the final slash
  folder_name=${folder#tests_}
  folder_name=${folder_name%/}
  # Add the folder name to the array
  REPOS+=("$folder_name")
done

for arg in "$@"
do
  # -v for validate
  if [ "$arg" == "-v" ]; then
    VALIDATE=1
  fi
  # -t=test num
  if [[ $arg =~ ^-t=([0-9]*)$ ]]; then
    TODO="${BASH_REMATCH[1]}"
  fi
  # --repo or like --general
  for repo in "${REPOS[@]}"; do
    if [ "--${repo}" == "$arg" ]; then
      echo "Repo = $repo"
      TARGETREPO="$repo"
      break
    fi
  done
done

# Function to display colored text
colored() {
  printf "%s%s%s" "${!1}" "${2}" "${NC}"
}

select_repo() {
  local original_PS3=$PS3
  local repo

  PS3='Please choose a repository: '

  select repo in "${REPOS[@]}"; do
    if [ -n "$repo" ]; then
      break
    fi
  done

  PS3=$original_PS3

  echo $repo
}

# select the target repo to create a test for
if [ -z "$TARGETREPO" ]; then
  TARGETREPO=$(select_repo)
fi

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

function record_tests() {
  PROJECT="tests_${1// /_}"

  FILES=

  rm -rf tmp/$PROJECT
  mkdir -p tmp/$PROJECT

  if [ "$TODO" != "" ]; then
    FILES=$(find $PROJECT -name $(printf "%04d" $TODO)*.test)
    if [ "$FILES" == "" ]; then
      echo "Could not find test $TODO"
      exit
    fi
    NUMFILES=1
  else
    # Iterate through the test files
    for file in "$PROJECT"/*.test; do
      # Check if the file exists
      if [ -e "$file" ]; then
        NUMFILES=$((NUMFILES + 1))
        FILES="${FILES} $file"
      fi
    done
    if [ $NUMFILES -eq 0 ]; then
      echo -e "\e[31mNo tests found in $PROJECT folder\e[0m"
      exit
    fi
  fi

  NUMFILES="$(echo $FILES | wc -w)"

  echo -e "\e[33m== [\e[31mRECORDING \e[97m$NUMFILES INTEGRATION TESTS\e[33m] ==\e[0m"

  TESTCOUNT=0

  for FILE in $FILES; do
    ./record_test.sh $FILE $VALIDATE $TESTCOUNT $NUMFILES &
    TESTCOUNT=$((TESTCOUNT + 1))
  done

  #rm -rf tmp/$PROJECT
}

record_tests $TARGETREPO
