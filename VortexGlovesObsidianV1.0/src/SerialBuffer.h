#ifndef SERIAL_BUFFER_H
#define SERIAL_BUFFER_H

#include <inttypes.h>

#include "Log.h"
class FlashClass;

class SerialBuffer
{
  friend class Storage;

public:
  SerialBuffer(uint32_t size = 0, const uint8_t *buf = nullptr);
  ~SerialBuffer();

  // copy and assignment operators
  SerialBuffer(const SerialBuffer &other);
  void operator=(const SerialBuffer &other);

  // reset the buffer to a specific capcity
  bool init(uint32_t capacity = 0, const uint8_t *buf = nullptr);

  // clear the buffer
  void clear();

  // shrink capacity down to size, to 
  bool shrink();

  // append another buffer
  bool append(const SerialBuffer &other);

  // extend the storage without changing the size of the data
  bool extend(uint32_t size);

  // shitty in-place compression and decompression
  bool compress();
  bool decompress();

  // serialize a byte into the buffer
  bool serialize(uint8_t byte);
  bool serialize(uint16_t bytes);
  bool serialize(uint32_t bytes);

  // reset the unserializer index 
  void resetUnserializer();
  // move the unserializer index manually
  void moveUnserializer(uint32_t idx);

  // serialize a byte into the buffer
  bool unserialize(uint8_t *byte);
  bool unserialize(uint16_t *bytes);
  bool unserialize(uint32_t *bytes);

  // same thing but via return value
  uint8_t unserialize8();
  uint16_t unserialize16();
  uint32_t unserialize32();

  uint8_t peek8() const;
  uint16_t peek16() const;
  uint32_t peek32() const;

  // overload += for appending buffer
  SerialBuffer &operator+=(const SerialBuffer &rhs);
  // also overload += for appending bytes
  SerialBuffer &operator+=(const uint8_t &rhs);
  SerialBuffer &operator+=(const uint16_t &rhs);
  SerialBuffer &operator+=(const uint32_t &rhs);

  // overload [] for array access (no bounds check lol)
  uint8_t &operator[](uint32_t index) { return m_pData->buf[index]; }
  // overload (uint8_t *) for cast to buffer
  operator uint8_t *() const { return m_pData ? m_pData->buf : nullptr; }
  operator const uint8_t *() const { return m_pData ? m_pData->buf : nullptr; }

  // return the members
  const uint8_t *data() const { return m_pData ? m_pData->buf : nullptr; }
  void *rawData() const { return m_pData; }
  uint32_t rawSize() const { return m_pData ? m_pData->size + sizeof(RawBuffer) : 0; }
  uint32_t size() const { return m_pData ? m_pData->size : 0; }
  uint32_t capacity() const { return m_capacity; }

private:
  uint8_t *frontSerializer() const { return m_pData ? m_pData->buf + m_pData->size : nullptr; }
  uint8_t *frontUnserializer() const { return m_pData ? m_pData->buf + m_position : nullptr; }
  bool largeEnough(uint32_t amount) const;

  // inner data buffer
#ifdef TEST_FRAMEWORK
#ifndef LINUX_FRAMEWORK
  #pragma warning(disable : 4200)
#endif
#endif
  struct RawBuffer {
    RawBuffer() : size(0), buf() {}
    uint32_t size;
    uint8_t buf[];
  };

  // the buffer of data
  RawBuffer *m_pData;
  // the index for unserialization
  uint32_t m_position;
  // the actual size of the buffer
  uint32_t m_capacity;
};

#endif
