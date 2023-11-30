#!/bin/bash

# Color definitions
RED="$(tput setaf 1)"
GREEN="$(tput setaf 2)"
YELLOW="$(tput setaf 3)"
WHITE="$(tput setaf 7)"
NC="$(tput sgr0)" # No Color

# Function to display colored text
colored() {
  printf "%s%s%s\n" "${!1}" "${2}" "${NC}"
}

git fetch

FILE=$1
BASE=master

REPOS=(
 "gloves"
 "orbit"
 "handle"
 "duo"
 "chromadeck"
 "spark"
)
TARFILES=

for REPO in "${REPOS[@]}"; do
  colored YELLOW "Generating diff for ${WHITE}$REPO${YELLOW}..."
  git diff $BASE $REPO -- $FILE &> ${REPO}.diff
  git diff --name-only $BASE $REPO -- $FILE &> ${REPO}_files.diff
  TARFILES="$TARFILES $REPO.diff ${REPO}_files.diff"
done

DIFFS="diffs.tar.gz"

colored GREEN "Zipping diffs into ${WHITE}$DIFFS${YELLOW}..."
tar -zcvf $DIFFS $TARFILES
