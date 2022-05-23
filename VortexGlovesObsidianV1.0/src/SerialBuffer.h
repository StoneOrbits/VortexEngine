#ifndef SERIAL_BUFFER_H
#define SERIAL_BUFFER_H

#include <inttypes.h>

class SerialBuffer
{
public:
  SerialBuffer(uint32_t size = 0, const uint8_t *buf = nullptr);
  ~SerialBuffer();

  // reset the buffer to a specific capcity
  bool init(uint32_t capacity = 0, const uint8_t *buf = nullptr);

  // append another buffer
  bool append(const SerialBuffer &other);

  // extend the storage without changing the size of the data
  bool extend(uint32_t size);

  // serialize a byte into the buffer
  bool serialize(uint8_t byte);
  bool serialize(uint16_t bytes);
  bool serialize(uint32_t bytes);

  // overload += for appending buffer
  SerialBuffer &operator+=(const SerialBuffer &rhs);
  // also overload += for appending bytes
  SerialBuffer &operator+=(const uint8_t &rhs);
  SerialBuffer &operator+=(const uint16_t &rhs);
  SerialBuffer &operator+=(const uint32_t &rhs);

  // overload [] for array access (no bounds check lol)
  uint8_t &operator[](uint32_t index) { return m_pBuffer[index]; }
  // overload (uint8_t *) for cast to buffer
  operator uint8_t *() const { return m_pBuffer; }
  operator const uint8_t *() const { return m_pBuffer; }

  // return the members
  const uint8_t *data() const { return m_pBuffer; }
  uint32_t size() const { return m_size; }
  uint32_t capacity() const { return m_capacity; }

private:
  // the buffer of data
  uint8_t *m_pBuffer;
  // the size of the data
  uint32_t m_size;
  // the actual size of the buffer
  uint32_t m_capacity;
};

#endif
