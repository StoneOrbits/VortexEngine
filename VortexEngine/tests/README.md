# Vortex Tests

At the moment this folders only contains a set of integration tests which test the entire Vortex Engine from end to end.

The tests use the command line vortex tool with various inputs and parameters and compare the output against prerecorded
test files. If the output differs then the test fails. These tests are rather rudimentary and not the most robust.

These tests will catch if anything changes in the output of the vortex engine, including if something as simple as a blink
timing has been changed. That can be both a helpful and annoying aspect of these tests.
