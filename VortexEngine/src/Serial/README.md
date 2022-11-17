# Serial

This folder contains all classes and code related to Serial communications, that means any communications which take place over a Serial stream.

This includes: saving data to storage, Infrared communications, and even just runtime storage of modes that aren't actively running.

## [Bit Stream Class](BitStream.h)

This is a class to manage a buffer that can be written-to or read-from one bit at a time.

This is like a smaller lightweight version of the SerialBuffer class, it is utilized by the SerialBuffer class for operations like compression.

## [Serial Class](Serial.h)

This is the global Serial communications manager, this class is used to interface over USB with serial operations.

## [Serial Buffer Class](SerialBuffer.h)

This class is the container for any data being serialized, this is like a more sophisticated BitStream buffer.

This class also includes a pointer to walk through the stream (for sending the stream) and accessors for reading/writing data in chunks.

Both Storage and Infrared systems are designed to be given SerialBuffer objects which they walk and write to IR/Serial respectively.
