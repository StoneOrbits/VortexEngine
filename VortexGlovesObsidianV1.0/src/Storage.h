#ifndef STORAGE_H
#define STORAGE_H

#define STORAGE_SIZE 8192

class SerialBuffer;

class Storage
{
public:
  Storage();
  ~Storage();

  // store a serial buffer to storage
  static bool write(SerialBuffer &buffer);
  // read a serial buffer from storage
  static bool read(SerialBuffer &buffer);
};

#endif
