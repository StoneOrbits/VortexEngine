# Sequencer

The sequencer is a special tool which can be used to create very sophisticated patterns that follow a sequence of steps.

The sequencer can be thought of as "sheet music" for patterns, allowing you to define exactly which pattern plays on exactly 
which finger with exactly which colorset for an exact duration. This can be defined in each step of a sequence which can have 
any number of steps.

## [Sequence Class](Sequence.h)

This is a basic sequence object, the Sequenced Pattern simply runs a Sequence.

## [Sequenced Pattern](SequencedPattern.h)

This is a type of Multi-Led pattern which will run a Sequence on all leds.
