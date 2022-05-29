#include "SerialBuffer.h"

#include "Memory.h"
#include "Log.h"

#include <FlashStorage.h>
#include <string.h>

SerialBuffer::SerialBuffer(uint32_t size, const uint8_t *buf) :
  m_pData(),
  m_position(0),
  m_capacity(0),
  m_compressed(false)
{
  init(size, buf);
}

SerialBuffer::~SerialBuffer()
{
  clear();
}

SerialBuffer::SerialBuffer(const SerialBuffer &other)
{
  init(other.capacity(), other.data());
}

void SerialBuffer::operator=(const SerialBuffer &other)
{
  init(other.capacity(), other.data());
}

// reset the buffer
bool SerialBuffer::init(uint32_t capacity, const uint8_t *buf)
{
  clear();
  if (capacity) {
    // round up to nearest 4
    m_capacity = (capacity + 4) - (capacity % 4);
    m_pData = (RawBuffer *)vcalloc(1, m_capacity + sizeof(RawBuffer));
    if (!m_pData) {
      m_capacity = 0;
      ERROR_OUT_OF_MEMORY();
      return false;
    }
  }
  if (buf && m_pData) {
    memcpy(m_pData->buf, buf, m_capacity);
    m_pData->size = m_capacity;
  }
  return true;
}

void SerialBuffer::clear()
{
  if (m_pData) {
    vfree(m_pData);
    m_pData = nullptr;
  }
  m_capacity = 0;
}

bool SerialBuffer::shrink()
{
  if (!m_pData) {
    return false;
  }
  if (m_pData->size == m_capacity) {
    return true;
  }
  //DEBUGF("Extending %u bytes from %u to %u", size, m_capacity, new_size);
  RawBuffer *temp = (RawBuffer *)vrealloc(m_pData, m_pData->size + sizeof(RawBuffer));
  if (!temp) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  m_pData = temp;
  m_capacity = m_pData->size;
  return true;
}

// append another buffer
bool SerialBuffer::append(const SerialBuffer &other)
{
  if (!other.m_pData) {
    // nothing to append
    return true;
  }
  if (!m_pData || other.size() > (m_capacity - size())) {
    if (!extend(other.size())) {
      return false;
    }
  }
  memcpy(frontSerializer(), other.data(), other.size());
  m_pData->size += other.size();
  return true;
}

// extend the storage without changing the size of the data
bool SerialBuffer::extend(uint32_t size)
{
  // round size up to nearest 4
  uint32_t new_size = m_capacity + size;
  new_size += 3;
  new_size -= (new_size % 4);
  //DEBUGF("Extending %u bytes from %u to %u", size, m_capacity, new_size);
  RawBuffer *temp = (RawBuffer *)vrealloc(m_pData, new_size + sizeof(RawBuffer));
  if (!temp) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  m_pData = temp;
  if (!m_capacity) {
    // if new allocation need to zero
    memset(m_pData, 0, new_size + sizeof(RawBuffer));
  }
  m_capacity = new_size;
  return true;
}

// TODO: integrate this somewhere
static uint32_t get_width(uint32_t value)
{
  if (!value) {
    return 0;
  }
  for (uint32_t i = 0; i < 31; ++i) {
    if (value < (uint32_t)(1 << i)) {
      return i;
    }
  }
  return 0;
}

// TODO: find a new home for this
class BitStream
{
public:
  BitStream(uint8_t *buf, uint32_t size) :
    m_buf(buf), m_buf_size(size), m_bit_pos(0), m_buf_eof(false)
  {
  }

  void resetPos()
  {
    m_bit_pos = 0;
    m_buf_eof = false;
  }

  bool eof() const { return m_buf_eof; }
  uint32_t size() const { return m_buf_size; }
  const uint8_t *data() const { return m_buf; }
  uint32_t bytepos() const { return m_bit_pos / 8; }
  uint32_t bitpos() const { return m_bit_pos; }

  uint8_t read1Bit()
  {
    if (m_buf_eof) {
      return 0;
    }
    if (m_bit_pos >= (m_buf_size * 8)) {
      m_buf_eof = true;
      return 0;
    }
    uint32_t rv = (m_buf[m_bit_pos / 8] >> (7 - (m_bit_pos % 8))) & 1;
    m_bit_pos++;
    return rv;
  }

  void write1Bit(uint8_t bit)
  {
    if (m_buf_eof) {
      return;
    }
    if (m_bit_pos >= (m_buf_size * 8)) {
      m_buf_eof = true;
      return;
    }
    m_buf[m_bit_pos / 8] |= (bit & 1) << (7 - (m_bit_pos % 8));
    m_bit_pos++;
    if (m_bit_pos >= (m_buf_size * 8)) {
      m_buf_eof = true;
    }
  }

  uint8_t readBits(uint32_t numBits)
  {
    uint32_t val = 0;
    for (uint32_t i = 0; i < numBits; ++i) {
      if (i > 0) {
        val <<= 1;
      }
      val |= read1Bit();
      if (m_buf_eof) {
        break;
      }
    }
    if (m_bit_pos >= (m_buf_size * 8)) {
      m_buf_eof = true;
    }
    return val;
  }

  void writeBits(uint32_t numBits, uint32_t val)
  {
    for (uint32_t i = 0; i < numBits; ++i) {
      write1Bit(get_bit(val, numBits - (i + 1)));
    }
  }

private:
  uint8_t get_bit(uint32_t val, uint32_t bit) const
  {
    return (val >> bit) & 1;
  }

  uint8_t *m_buf;
  uint32_t m_buf_size;
  uint32_t m_bit_pos;
  bool m_buf_eof;
};

bool SerialBuffer::compress()
{
  if (m_compressed) {
    return true;
  }
  uint8_t bytes[256] = {0};
  uint8_t unique_bytes = 0;
  // count the unique bytes in the data buffer
  for (uint32_t i = 0; i < m_pData->size; ++i) {
    if (bytes[m_pData->buf[i] & 0xFF]) {
      continue;
    }
    bytes[m_pData->buf[i] & 0xFF] = 1;
    unique_bytes++;
  }
  // table = num_entries + entries[]
  uint32_t table_size = sizeof(uint8_t) + (sizeof(uint8_t) * unique_bytes);
  uint32_t wid = get_width(unique_bytes);
  // data = width(num_entries) * size
  uint32_t data_size = ((wid * m_pData->size) + 7) / 8;
  // total = table + data
  uint32_t total_size = table_size + data_size;
  uint32_t old_size = m_pData->size;
  if (total_size >= old_size) {
    DEBUGF("new size larger: %u old: %u", total_size, m_pData->size);
    return false;
  }
  // extend enough for two tables
  extend(table_size * 2);
  // put table at end of buffer, in newly extended region
  uint8_t *table = (m_pData->buf + m_capacity) - table_size;
  // the number of unique bytes (table size)
  table[0] = unique_bytes;
  // start table data at 1
  uint32_t b = 1;
  // walk all 256 bytes and mirror them in the table
  for (uint32_t i = 0; i < 256; ++i) {
    if (!bytes[i]) {
      continue;
    }
    if (b > unique_bytes) {
      // error!!!
      DEBUG("ERROR IN COMPRESSION");
      return false;
    }
    bytes[i] = b;
    table[b++] = i;
  }

  BitStream bits(m_pData->buf, data_size);

  DEBUGF("Bitstream size %u", bits.size());

  for (uint32_t i = 0; i < old_size; ++i) {
    uint32_t b = m_pData->buf[i];
    // zero out the current byte so the bits can be written
    m_pData->buf[i] = 0;
    // compressed version of the current byte
    uint32_t compressed = bytes[b];
    // write the compressed b with width
    bits.writeBits(wid, compressed);
    //DEBUGF("Compressed %u -> %u", b, compressed);
  }

  // move the data forward
  memmove(m_pData->buf + table_size, m_pData->buf, data_size);
  // move the table back
  memmove(m_pData->buf, (m_pData->buf + m_capacity) - table_size, table_size);
  // update the size of the buffer
  m_pData->size = table_size + bits.bytepos();
  // shrink to the size of the buffer
  shrink();

  DEBUGF("Compressed %u to %u bytes", old_size, m_pData->size);

  m_compressed = true;

  return true;
}

bool SerialBuffer::decompress()
{
  if (!m_compressed) {
    return true;
  }
  // WARNING: need to extend buffer more maybe?
  resetUnserializer();
  uint8_t unique_bytes = unserialize8();
  uint8_t *table = m_pData->buf;
  uint8_t *data = table + unique_bytes + 1;
  uint8_t *data_end = m_pData->buf + m_pData->size;
  uint32_t wid = get_width(unique_bytes);
  uint32_t data_len = data_end - data;

  uint32_t expected_inflated_len = ((data_len / wid) + 1) * 8;

  DEBUGF("Expected Inflated length: %u", expected_inflated_len);

  // TODO: do this without allocating a new buffer, just extend the data
  uint8_t *out_data = new uint8_t[expected_inflated_len];
  memset(out_data, 0, expected_inflated_len);

  BitStream bits(data, data_len);

  uint32_t outPos = 0;
  while (!bits.eof()) {
    uint32_t val = bits.readBits(wid);
    out_data[outPos++] = table[val];
    //DEBUGF("Decompressed %u -> %u", val, table[val]);
  }

  m_pData->size = outPos;
  memmove(m_pData->buf, out_data, outPos);
  delete[] out_data;

  DEBUGF("Decompressed %u to %u bytes", (uint32_t)(data_end - m_pData->buf), m_pData->size);

  shrink();

  m_compressed = false;

  return true;
}

bool SerialBuffer::largeEnough(uint32_t amount) const
{
  if (!m_pData) {
    return false;
  }
  return ((m_pData->size + amount) <= m_capacity);
}

bool SerialBuffer::serialize(uint8_t byte)
{
  //DEBUGF("Serialize8(): %u", byte);
  if (!m_pData || (m_pData->size + sizeof(uint8_t)) > m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(m_pData->buf + m_pData->size, &byte, sizeof(uint8_t));
  // walk forward
  m_pData->size += sizeof(uint8_t);
  return true;
}

bool SerialBuffer::serialize(uint16_t bytes)
{
  //DEBUGF("Serialize16(): %u", bytes);
  if (!m_pData || (m_pData->size + sizeof(uint16_t)) > m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(m_pData->buf + m_pData->size, &bytes, sizeof(uint16_t));
  m_pData->size += sizeof(uint16_t);
  return true;
}

bool SerialBuffer::serialize(uint32_t bytes)
{
  //DEBUGF("Serialize32(): %u", bytes);
  if (!m_pData || (m_pData->size + sizeof(uint32_t)) > m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
  }
  memcpy(m_pData->buf + m_pData->size, &bytes, sizeof(uint32_t));
  m_pData->size += sizeof(uint32_t);
  return true;
}

// reset the unserializer index 
void SerialBuffer::resetUnserializer()
{
  moveUnserializer(0);
}

// move the unserializer index manually
void SerialBuffer::moveUnserializer(uint32_t idx)
{
  if (!m_pData) {
    return;
  }
  if (idx >= m_pData->size) {
    idx = 0;
  }
  m_position = idx;
}

// unserialize data and walk the buffer that many bytes
bool SerialBuffer::unserialize(uint8_t *byte)
{
  if (!m_pData || m_position >= m_pData->size || (m_pData->size - m_position) < sizeof(uint8_t)) {
    return false;
  }
  memcpy(byte, m_pData->buf + m_position, sizeof(uint8_t));
  //DEBUGF("Unserialize8(): %u", *byte);
  m_position += sizeof(uint8_t);
  return true;
}

bool SerialBuffer::unserialize(uint16_t *bytes)
{
  if (!m_pData || m_position >= m_pData->size || (m_pData->size - m_position) < sizeof(uint32_t)) {
    return false;
  }
  memcpy(bytes, m_pData->buf + m_position, sizeof(uint16_t));
  //DEBUGF("Unserialize16(): %u", *bytes);
  m_position += sizeof(uint16_t);
  return true;
}

bool SerialBuffer::unserialize(uint32_t *bytes)
{
  if (!m_pData || m_position >= m_pData->size || (m_pData->size - m_position) < sizeof(uint32_t)) {
    return false;
  }
  memcpy(bytes, m_pData->buf + m_position, sizeof(uint32_t));
  //DEBUGF("Unserialize32(): %u", *bytes);
  m_position += sizeof(uint32_t);
  return true;
}

uint8_t SerialBuffer::unserialize8()
{
  uint8_t byte = 0;
  unserialize(&byte);
  return byte;
}

uint16_t SerialBuffer::unserialize16()
{
  uint16_t bytes = 0;
  unserialize(&bytes);
  return bytes;
}

uint32_t SerialBuffer::unserialize32()
{
  uint32_t bytes = 0;
  unserialize(&bytes);
  return bytes;
}

uint8_t SerialBuffer::peek8() const
{
  if (!m_pData) {
    return 0;
  }
  return *(uint8_t *)frontUnserializer();
}

uint16_t SerialBuffer::peek16() const
{
  if (!m_pData) {
    return 0;
  }
  return *(uint16_t *)frontUnserializer();
}

uint32_t SerialBuffer::peek32() const
{
  if (!m_pData) {
    return 0;
  }
  return *(uint32_t *)frontUnserializer();
}

// read the data from a flash storage
// overload += for appending buffer
SerialBuffer &SerialBuffer::operator+=(const SerialBuffer &rhs)
{
  append(rhs);
  return *this;
}

// also overload += for appending bytes
SerialBuffer &SerialBuffer::operator+=(const uint8_t &rhs)
{
  serialize(rhs);
  return *this;
}

SerialBuffer &SerialBuffer::operator+=(const uint16_t &rhs)
{
  serialize(rhs);
  return *this;
}

SerialBuffer &SerialBuffer::operator+=(const uint32_t &rhs)
{
  serialize(rhs);
  return *this;
}
