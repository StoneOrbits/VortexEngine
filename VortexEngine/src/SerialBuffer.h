#ifndef SERIAL_BUFFER_H
#define SERIAL_BUFFER_H

#include <inttypes.h>

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

  // used to initialize the full RawBuffer with a chunk of data,
  // this includes the size, flags, crc, and data
  bool rawInit(const uint8_t *rawdata, uint32_t size);

  // reset the buffer to a specific capcity
  bool init(uint32_t capacity = 0, const uint8_t *buf = nullptr);

  // clear the buffer
  void clear();

  // shrink capacity down to size, to free unused space
  bool shrink();

  // append another buffer
  bool append(const SerialBuffer &other);

  // extend the storage without changing the size of the data
  bool extend(uint32_t size);

  // badly written in-place compression and decompression
  // which just uses a table of values then compresses the
  // data into indexes in the table packed into a buffer.
  // Unfortunately sometimes compression does not yield a
  // smaller buffer size, incases like this compress returns
  // true but the data isn't compressed. Similarly, if that
  // data is passed to decompress it will return true.
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
  bool is_compressed() const;

  // this should be fine
  uint8_t *frontUnserializer() const { return m_pData ? m_pData->buf + m_position : nullptr; }

private:
  // don't expose this one it's dangerous
  uint8_t *frontSerializer() const { return m_pData ? m_pData->buf + m_pData->size : nullptr; }
  bool largeEnough(uint32_t amount) const;
  uint32_t getWidth(uint32_t value);

  // inner data buffer
#ifdef TEST_FRAMEWORK
#ifndef LINUX_FRAMEWORK
  // disable warning about using flexible array at end of structure,
  // stfu compiler I know what im doing
#pragma warning(disable : 4200)
#endif
#endif

  // the structure of raw data that's written to storage
  struct RawBuffer
  {
    RawBuffer() : size(0), flags(0), crc32(5381) {}
    // hash the raw buffer into crc
    uint32_t hash()
    {
      uint32_t hash = 5381;
      for (uint32_t i = 0; i < size; ++i) {
        hash = ((hash << 5) + hash) + buf[i];
      }
      return hash;
    }
    // accumulate the data into the hash
    void accumulate(uint32_t val)
    {
      crc32 = ((crc32 << 5) + crc32) + val;
    }
    // veryify the crc
    bool verify()
    {
      if (!crc32) {
        return true;
      }
      return crc32 == hash();
    }
    // re-calculate the crc
    void recalc_crc()
    {
      crc32 = hash();
    }
    // the size of the buf of data
    uint32_t size;
    // any special flags about the data (compression etc)
    uint32_t flags;
    // the crc32 for the data
    uint32_t crc32;
    // the start of the buf of data
    uint8_t buf[];
  };

  // The raw buffer of data along with size and flags
  RawBuffer *m_pData;
  // the index in the raw buffer for unserialization
  uint32_t m_position;
  // the actual size of the buffer raw buffer
  uint32_t m_capacity;
};

#endif
