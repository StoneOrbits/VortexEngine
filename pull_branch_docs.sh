#!/bin/bash

# List of branches
branches=("core" "gloves" "orbit" "handle" "duo")

# Loop over each branch
for branch in "${branches[@]}"; do
  # Add worktree for the branch
  git worktree add ${branch}-docs ${branch}-docs

  # Copy the contents of the 'docs' folder to the corresponding folder in the current directory
  cp -a ${branch}-docs/docs/* ./docs/${branch}/

  # Remove the worktree
  git worktree remove ${branch}-docs
done
