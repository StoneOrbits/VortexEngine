---
layout: default
title: Building
nav_order: 9
parent: Source Code
---

# Building Vortex Engine

The steps to build Vortex Engine differ based on the device/branch you are targeting.

If you have access to Linux, this is the easiest way to build for the target device.

Alternatively windows is available but significantly more complicated and less supported.

# Common Build Steps

Whether building for Linux or Windows the first few steps are universal.

First checkout the repository:

```
git clone https://github.com/StoneOrbits/VortexEngine.git
```

Then switch to the desired device branch:
```
cd VortexEngine
git switch duo
```
See all branches [here](https://github.com/StoneOrbits/VortexEngine/branches/active)

# Linux Build Steps
The following steps should work on Linux (Ubuntu/debian)

Run the install dependencies make target
```
make install
```

This should install all the required build dependencies to build the device.

Now run the build:
```
make
```

# Windows Build Steps
Honestly this is a bit of a pain and not really worth documenting.

It would be easier for me to just automate this with a similar process to Linux but it's not worth my time.

If you really need to build on Windows just join the [discord](support.html) and ask, I'll be happy to help.