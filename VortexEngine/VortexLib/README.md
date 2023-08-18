# Vortex Lib

This folder contains the compatibility layer that allows vortex engine to run in a non arduino environment.

For example, this contains drop-in files to replace some of the arduino headers with dummy implementations.

This also contains the main 'Vortex Lib' header which can be used as an interface point for controlling and running
the vortex engine contained within.

If you want to use Vortex you should include the VortexLib.h header and use the functions provided to control the engine.
The goal of VortexLib is to provide a simple interface to the engine that can be used in any environment.

However, you're free to include any of the headers from any folder and use any of the classes and functions provided.