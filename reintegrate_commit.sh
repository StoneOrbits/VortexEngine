#!/bin/bash

# Color definitions
RED="$(tput setaf 1)"
GREEN="$(tput setaf 2)"
YELLOW="$(tput setaf 3)"
WHITE="$(tput setaf 7)"
NC="$(tput sgr0)" # No Color

# Function to display colored text
colored() {
  printf "%s%s%s" "${!1}" "${2}" "${NC}"
}

REPOS=(
	#"master"
	"gloves"
	"orbit"
	"handle"
	"duo"
	"chromadeck"
	"spark"
)
REPO_INDEX=0
FOUND_DOT=0
DOTFILE=".merge_progress"

# Check if there is a dotfile, and if so, read the progress
if [ -f "$DOTFILE" ]; then
  REPO_INDEX=$(cat "$DOTFILE")
  read -p "${YELLOW}Resume previous session from [${WHITE}${REPOS[$REPO_INDEX]}${YELLOW}]? (y/n): ${NC}" confirm
  if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
    FOUND_DOT=1
    echo "Resuming from ${REPOS[$REPO_INDEX]}..."
  else
    echo "Restarting"
    REPO_INDEX=0
  fi
fi

# switch to master
git switch master

# Get the commit ID from the command line or use the latest commit
COMMIT="${1:-$(git rev-parse HEAD)}"

if ! git rev-parse --verify -q "$COMMIT" >/dev/null 2>&1; then
  colored RED "Commit $COMMIT does not exist"
  exit 1
fi

CURBRANCH="$(git branch --show-current)"

# Loop over repos starting from the saved index
for ((i = REPO_INDEX; i < ${#REPOS[@]}; i++)); do
  REPO=${REPOS[i]}

  if [[ -z "$1" && "$REPO" == "$CURBRANCH" ]]; then
    continue
  fi

  read -p "${YELLOW}Cherry-pick commit ${WHITE}$COMMIT ${YELLOW}into ${WHITE}$REPO${YELLOW}? (y/n): ${NC}" confirm
  if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
    continue
  fi

  colored GREEN "Switching to $REPO..."
  git switch "$REPO" || exit
  # Save progress to dotfile
  echo $((i + 1)) > "$DOTFILE"
  if [ "$FOUND_DOT" -eq 0 ] || [ "$REPO_INDEX" -ne "$i" ]; then
    git reset --hard origin/$REPO || exit
  fi
  colored GREEN "Merging $COMMIT into $REPO..."
  git merge master
  if [ $? -ne 0 ]; then
    echo "Merge failed!"
    if git diff --name-only --diff-filter=A | grep -q '^VortexEngine/tests/tests_general/'; then
      read -p "${YELLOW}Merge is only test failures, re-record tests? (y/n): ${NC}" confirm
      if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
        ./record_tests.sh || exit
      fi
    else
      git status
      read -p "${YELLOW}Would you like to continue? (y/n): ${NC}" confirm
      if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
        exit
      fi
    fi
  else
    read -p "${YELLOW}Record tests? (y/n): ${NC}" confirm
    if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
      ./record_tests.sh || exit
    fi
  fi

  colored GREEN "Switching back to $CURBRANCH..."
  git switch "$CURBRANCH" || exit
done

# Clean up the dotfile
rm -f "$DOTFILE"

# Allow the user to push all branches if they want
read -p "${YELLOW}Push all (y/n): ${NC}" confirm
if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
  colored GREEN "Pushing all branches..."
  ./pushall.sh
fi

# Generate diffs after all have been pushed
colored GREEN "Generating diffs..."
./gendiffs.sh

