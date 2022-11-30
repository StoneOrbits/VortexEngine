# Serial

This folder contains all classes and code related to Serial communications, that means any communications which take place over a Serial stream.

This includes: saving data to storage, Infrared communications, and even just runtime storage of modes that aren't actively running.

## [Bit Stream Class](BitStream.h)

This is a lightweight stream object that allows streaming a single bit into or out of the buffer.

If you need to read or write one bit at a time then you must use a BitStream, if you are reading/writing bytes at a time then use a ByteStream.

This is like a smaller lightweight version of the ByteStream class, it is utilized by the ByteStream class for operations like compression.

## [Byte Stream Class](ByteStream.h)

This is a more sophisticated stream class that specializing in streaming bytes, words, and dwords in/out of a buffer.

This is mainly used for serializing and unserializing data for storage and IR transfer.

This class also includes a pointer to walk through the stream (for sending the stream) and accessors for reading/writing data in chunks.

Both Storage and Infrared systems are designed to be given ByteStream objects which they walk and write to IR/Serial respectively.

## [Compression](Compression.h)

This is a heavily trimmed down version of lz4 that has been configured to use as small of a footprint as possible.

## [Serial Class](Serial.h)

This is the global Serial communications manager, this class is used to interface over USB with serial operations.
