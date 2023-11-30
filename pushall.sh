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

YES=0
for arg in "$@"
do
    if [ "$arg" == "-y" ]; then
        YES=1
    fi
done

CURBRANCH="$(git branch --show-current)"

for REPO in "${REPOS[@]}"; do
  colored GREEN "Push branch [${WHITE}$REPO${GREEN}] ${YELLOW}(y/N): "
  if [ $YES -eq 0 ]; then
    read -e confirm
    if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
      continue
    fi
  fi
  git switch $REPO && git push
done

git switch "$CURBRANCH"
