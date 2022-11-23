#ifndef STORAGE_H
#define STORAGE_H

#define STORAGE_SIZE 8192

class ByteStream;

class Storage
{
  Storage();
public:

  // init storage
  static bool init();
  static void cleanup();

  // store a serial buffer to storage
  static bool write(ByteStream &buffer);
  // read a serial buffer from storage
  static bool read(ByteStream &buffer);
};

#endif
