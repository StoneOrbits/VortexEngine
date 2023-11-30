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
 "master"
 "gloves"
 "orbit"
 "handle"
 "duo"
 "chromadeck"
 "spark"
)

CURBRANCH="$(git branch --show-current)"

for REPO in "${REPOS[@]}"; do
  colored GREEN "Pull branch [${WHITE}$REPO${GREEN}] ${YELLOW}(y/N): "
	read -e confirm
	if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
		continue
	fi
  git switch $REPO && git pull
done

git switch "$CURBRANCH"
