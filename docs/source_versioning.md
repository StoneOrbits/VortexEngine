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

A Major version difference indicates a product isn't compatible anymore after a large change.

## Minor Version

A Minor version difference indicates a significant update, new feature or fix.

## Build Number

A build version indicates a change made to the source code. Build numbers are derived by counting the number of commits since the last version tag.

# Version Tags

Source code *versions* are recorded as *tags* on the source code. The tags look like **major.minor** then the true device version is derived by counting the number of commits since that tag to produce **major.minor.build**

## Device Version Tags

Each device has separate tags indicating their branch name in the tag. For example the duo is 1.2d and the chromadeck is 1.2c while the orbit is 1.2o. The original goal was to allow each device to increment versions independently, however there's no real solution to inheritance from the core while also deriving version numbers from the devices.

It's almost as if the core needs to have a version number, then each device needs to have a secondary 3 digit version number based off the core...

There is the question of what happens when we run out of letters, it will probably change to something like 1.2-orbit then. 

The way the versioning works with regards to inheritance from the core is simply that


