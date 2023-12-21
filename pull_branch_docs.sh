#!/bin/bash

# List of branches
branches=("core" "gloves" "orbit" "handle" "duo" "chromadeck" "spark" "desktop")

# fetch any new changes
git fetch

# Loop over each branch
for branch in "${branches[@]}"; do
  # Add worktree for the branch
  git worktree add ${branch}-docs origin/${branch}-docs

  # clear out existing docs
  rm -rf ./docs/${branch}/*

  # Copy the contents of the 'docs' folder to the corresponding folder in the current directory
  cp -a ${branch}-docs/docs/* ./docs/${branch}/

  # Remove the worktree
  git worktree remove ${branch}-docs

  # add the new docs
  git add ./docs/${branch}
done
