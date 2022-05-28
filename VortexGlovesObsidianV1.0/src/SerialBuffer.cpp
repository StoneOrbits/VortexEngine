#include "SerialBuffer.h"

#include "Memory.h"
#include "Log.h"

#include <FlashStorage.h>
#include <string.h>

SerialBuffer::SerialBuffer(uint32_t size, const uint8_t *buf) :
  m_pData(),
  m_position(0),
  m_capacity(0)
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
  if (!m_pData || (m_pData->size + sizeof(uint32_t)) > m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
  }
  // set the byte at the front of the buf
  *frontSerializer() = byte;
  // walk forward
  m_pData->size += sizeof(uint32_t);
  return true;
}

bool SerialBuffer::serialize(uint16_t bytes)
{
  //DEBUGF("Serialize16(): %u", bytes);
  if (!m_pData || (m_pData->size + sizeof(uint32_t)) > m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
  }
  *(uint16_t *)frontSerializer() = bytes;
  m_pData->size += sizeof(uint32_t);
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
  *(uint32_t *)frontSerializer() = bytes;
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
  if (!m_pData || m_position >= m_pData->size || (m_pData->size - m_position) < sizeof(uint32_t)) {
    return false;
  }
  *byte = *frontUnserializer();
  //DEBUGF("Unserialize8(): %u", *byte);
  m_position += sizeof(uint32_t);
  return true;
}

bool SerialBuffer::unserialize(uint16_t *bytes)
{
  if (!m_pData || m_position >= m_pData->size || (m_pData->size - m_position) < sizeof(uint32_t)) {
    return false;
  }
  *bytes = *(uint16_t *)frontUnserializer();
  //DEBUGF("Unserialize16(): %u", *bytes);
  m_position += sizeof(uint32_t);
  return true;
}

bool SerialBuffer::unserialize(uint32_t *bytes)
{
  if (!m_pData || m_position >= m_pData->size || (m_pData->size - m_position) < sizeof(uint32_t)) {
    return false;
  }
  *bytes = *(uint32_t *)frontUnserializer();
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
