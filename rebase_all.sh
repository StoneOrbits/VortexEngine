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
 "gloves"
 "orbit"
 "handle"
 "duo"
)

CURBRANCH="$(git branch --show-current)"

for REPO in "${REPOS[@]}"; do
  colored YELLOW "Rebase branch [${WHITE}$REPO${YELLOW}] (y/N): "
  read -e confirm
  if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
    continue
  fi
  echo -e "${GREEN}Switching to $REPO...${NC}"
  git switch "$REPO" || exit
  echo -e "${GREEN}Rebasing... ${NC}"
  git rebase master || exit
  read -p "${YELLOW}Push $REPO? (y/n): ${NC}" confirm
  if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
    continue
  fi
  echo -e "${GREEN}Pushing... ${NC}"
  git push -f || exit
done

git switch $CURBRANCH

# Generate diffs after all have been pushed
echo -e "${GREEN}Generating diffs...${NC}"
./gendiffs.sh

