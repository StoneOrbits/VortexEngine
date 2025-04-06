---
layout: default
title: Versioning
nav_order: 5
parent: Source Code
---

# Versioning

Vortex Engine follows a three digit versioning system:

    x.y.z


Where x is the Major release version, y is the minor release version, and z is the build number.

## Major Version

A Major version difference happens when a product isn't compatible anymore after a large change.

## Minor Version

A Minor version difference happens when there is a significant update, new feature or fix.

## Build Number

And finally, a build version happens every time a change is made to the code. Build numbers are counted as the number of commits since the last minor version tag.

# Version Tags

The way versions are recorded is by tagging major.minor versions on the core source code and each device separately.

## Device Version Tags

Each device has separate tags indicating their branch name in the tag. For example the duo is 1.2d and the chromadeck is 1.2c while the orbit is 1.2o. The original goal was to allow each device to increment versions independently, however there's no real solution to inheritance from the core while also deriving version numbers from the devices.

It's almost as if the core needs to have a version number, then each device needs to have a secondary 3 digit version number based off the core...

There is the question of what happens when we run out of letters, it will probably change to something like 1.2-orbit then. 

The way the versioning works with regards to inheritance from the core is simply that


