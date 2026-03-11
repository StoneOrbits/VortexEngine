---
layout: default
title: Branches
nav_order: 0
parent: Source Code
---

# Source Branches

Vortex Engine itself is the "core" software that powers all devices. However, each device runs a derivation of the core that has been specifically modified for that device.

The primary differences between the core and each device are the hardcoded number of LEDs being controlled (as each device may have a different count) and the device menus, such as Color Selection or Randomizer.

Each device and its specific changes are maintained in branches named after the devices. These branches are permanent and continuously receive reintegration updates whenever the core is modified.

The following sections discuss the various branches and their purpose.

### The Core Branch
The core is stored in the **master** branch, the default main branch of the repository. The core is configured as a 1-LED device with agnostic menus, while each device branch adjusts LED enumeration and potentially modifies menus to match its specific LED count or layout.

The goal is for the core to run on any device, but it may only power the first LED or treat all LEDs the same.

The core also implements features that may only exist on certain devices. For example, wireless senders and receivers are not universally present:

- Older devices have infrared senders and receivers but no light receiver.
- The Duo has a light receiver but no infrared hardware.
- The Chromadeck and Spark have both infrared and light receivers.

All devices have light-sending hardware (LEDs), but only those with light-receiving hardware (Duo, Chromadeck, and Spark) can receive wireless LED blink transfers.

Even though the core includes sender and receiver logic, individual device branches may need to override sender or receiver configurations to match their specific hardware.

### Desktop Library Branch
The Vortex Engine Desktop Library, known as **VortexLib**, is stored in the **desktop** branch.

VortexLib is designed for desktop PCs, enabling software that interfaces with Vortex Engine devices. For example, both the Vortex Editor and lightshow.lol are powered by VortexLib.

### Device Branches
The remaining branches are dedicated to specific devices with unique hardware configurations. Each branch is named after its corresponding device:

- gloves
- handle
- orbit
- duo
- spark
- chromadeck

More details on each device can be found in the [Vortex Devices Section](vortex_devices.html).

## Reintegrating Changes

Whenever changes are made to the core (master), they must be reintegrated across all device branches.

The first step in reintegration is updating **master** with the latest core changes, do this by creating a branch like:  

`git switch -c yourname/core/master-reintegration-update`

Apply the changes to this branch, commit and push, then switch to the first device branch to reintegrate into.

After master has been updated, pull requests for each device can be created by merging master into the respective device branch.

Follow these steps:

1. Create a new branch for the device reintegration, e.g.:  
   `git branch -c name/orbit/reintegrate-new-fix`
2. Create a pull request to merge the new branch into the device branch, e.g.:  
   `name/orbit/reintegrate-new-fix -> orbit`
3. When merging the pull request, **DO NOT SQUASH OR REBASE**.  
   A regular merge commit must be used to retain history.


## Reintegration Script

In the branch `daniel/scripts_backup` there is a script called `reintegrate_as_branch.sh` which will take a branch like `daniel/core/master-change` and reintegrate it across every device interactively.

It will detect if there are conflicts and allow you to stop and edit them, then when you run the script again it will continue from where you left off.

If the changes don't conflict then they can be freely reintegrated, however due to the difference of devices there is a high chance at least one device will conflict.

The most likely conflicting branch is `desktop` which may need global static classes to be switched to local instances.

You can continue making changes to the core branch and running the reintegration script it will distribute the change to all of the matching device branches.

Once each device has been reintegrated the branches can be separately PRs.

It is important to merge them in order, merge the master/core change first and **DO NOT SQUASH COMMITS** otherwise device history will break.

After merging the core branch then merge the device branches.


## Creating new Devices

To create a new device for Vortex Engine the first step is to start with a fully up-to-date master branch. 

From the master branch, a new device can be created simply by creating a branch. 

For example:

   `git switch -c axon`

To start, this can be pushed to github without any changes to it -- so it is identical to master.

   `git push origin head -u`

In order to make the first changes to this device, create a branch off of axon:

   `git branch -c daniel/axon/initial-setup`

Then this branch will be used to performall the initial setup of the device, and a pull request will merge this branch into the official axon branch.

This allows for workflows to be run on the new testing branch without deploying them to Vortex Community for archiving.

Each device branch needs to have a unique name, and up till now they also start with unique letters (G,H,O,D,S,C,A) this has made for naming conventions that allow using only the first letter like 1.0g is meant to be 1.0-gloves.

Unfortunately that was a bad naming convention and only using the first letter should be avoided going forward. 

The versioning code in the build YMLs may need to be updated to account for the new version naming convention, but for now it isn't an issue. However once a device is created which uses the same first letter as another device there will be issues.

The part that really makes a device is the automated build, this exists in the directory:

   `.github/workflows/`

The workflows in this folder are in the format YML and they are detected by github automated runner to trigger the builds when the respective branch has been updated.

The workflow files are global across the entire repo, they must have unique names.

For example the workfow in the master branch is called `core_build.yml` and then each device has a differently named file like `duo_build.yml`. If they share a name then github will only detect one of them even if they appear in separate branches.

So the next step after branching off master into a new device branch is to rename the workflow file accordingly:

   `git mv .github/workflows/core_build.yml .github/workflows/axon_build.yml`

The next step is to edit the build file and update the references inside.

For example it needs to reference the new branch name instead of 'master'.  A good way to do this is to compare another device build YML against the core build YML to see what kind of changes were made on another device.

Referencing other devices will be a powerful tool in creating a new device, especially if the two devices share a platform or hardware.

The core build YML has different versioning code in the 'Determine Version and Build Number' step, it does not reference any `BRANCH_SUFFIX` where as device builds all have a branch suffix (the first letter of the branch).  In the future the branch suffix will be the full branch name after a hyphen. So `1.0a` is 1.0 axon right now, but in the future branch suffixes will be like 1.0-axon.  

These names like `1.0a` and `1.0-axon` would be tags that are placed on the commits at the given version. Then for example 5 commits after `1.0a` would be versioned as `1.0.5`.  Next time the minor version is increased manually to say 1.1a then all versions would be based off that like `1.1.25` (25 commits after 1.1).

The Docs step needs to be updated to reference the correct branch names.

The master build YML does not have a deploy step because the master branch is not a complete device but rather just a starting point for devices to branch from.

The new device must be given a deploy step in it's build YML, see other devices deploy steps for reference. 

The key part of the deploy step is Vortex Community will need to be updated to handle this new device. The update is not that hard to do, the website is already designed to work with any device name that is given. It will store and serve up the file without problems. The issue will be on the Downloads page there is no icon associated with the new device name.

So icons will need to be created for this new device, square 512x512 icons with no border or edge around the image must be created.

The README in the master branch needs to be updated to reference the new device and link to it's build badge.


### Creating the First Diff

In order to create the device after branching off the new device branch, the first thing to be applied should be a single commit full of all the changes to setup this new device.

The work can be done in more than one commit then rebased and squashed into a single commit, but the reason for only having a single commit to start is because this will drive the version counting and having multiple commits would cause the first version to be larger than 1.0.0 for example.

There's a number of things that must be filled out in the core Vortex Engine code in order for a device to be workable.

Going through a chromadeck diff for example, these are the things that I encounter while developing the Axon diff:

 - First the main workflow file `.github/workflows/x_build.yml`
 - The primary makefile for device builds
 - The makefiles for VortexLib and VortexCli
 - The creation of an .ino file for Arduino builds
 - The Doxyfile needs the `OUTPUT_DIRECTORY` field updated

Moving into the actual source code:

 - Button.cpp/h and Buttons.cpp/h may need updates if the number of buttons is different
 - LedTypes needs the Leds enum updated to match number of Leds
 - Leds.cpp needs to be updated to drive the Leds, mainly Leds::init() and Leds::Update() 
 - Log.cpp might need updating to access logging depending on the device type
 - Memory.cpp/h may be unnecessary on modern systems like ESP
 - Menu.cpp/h may need improvements to support various led selections before entering menus

Moving into the menus:

 - Most menus will need to be specifically checked, but some menus will still work
 - The randomizer menu is mostly agnostic and should work with minimal changes
 - Menus.cpp might need updates if there is different number of buttons

The default modes should be updated for the device, based on the max number of modes and number of leds.

Some individual multi-led patterns may need updating if their appearance is not suitable.

Sometimes Serial.cpp/h needs updating depending on the device and how well serial works.

Storage.cpp/h needs to be updated to use the storage apis available for the device. Any ESP device should have similar storage code.

TimeControl may need updates if it's an esp device, along with Button, for delayMicrosecondsCancellable to work. 

This is primarily used when sending Lifi to allow cancelling the send when it's inside the transmit loop.

Next are the configuration changes in VortexConfig.h:

 - The device name needs to be updated `VORTEX_NAME`
 - The `MAX_MODES` needs to be set for the device hardware
 - Check similar devices for any other important config changes

The next is VortexEngine.cpp the heart of the project, this file needs to be updated to initialize any new components.

For example Bluetooth, or IR, they need to be initialized here if they aren't already

This file is where all the main logic happens so if there's any main logic changes then they need to happen here.

Refer to other similar devices for guidance on changing this file.

Might need to add files like Bluetooth.cpp/h as they aren't in the core.

I think I will make an ESP base which all esp devices can branch from in the future.

Lastly, the tests need to be re-recorded and zipped which can be done via the `record_tests.sh` script.

That should mostly conclude all the changes necessary to create the initial diff for the device.

Now, before pushing this first commit to the new branch -- TEST IT! 

Upload, test, fix, commit, rebase.

Creating the first device will take a lot of trial and error, constantly review the diff against other devices.

Constantly test and make sure everything is working.

If you need to save your work and come back another day then put it in a branch, do not push the device branch yet!

If you add a new commit, you can rebase it and fixup it into the previous commit.

That way there is only one initial commit for the device, this is important for later.

The last step is setting the initial version tag for this new device, this is important.




