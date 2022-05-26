#include "SerialBuffer.h"

#include "Memory.h"
#include "Log.h"

#include <FlashStorage.h>
#include <string.h>

SerialBuffer::SerialBuffer(uint32_t size, const uint8_t *buf) :
  m_pBuffer(nullptr),
  m_position(0),
  m_size(0),
  m_capacity(0)
{
  init(size, buf);
}

SerialBuffer::~SerialBuffer()
{
  if (m_pBuffer) {
    vfree(m_pBuffer);
  }
}

// reset the buffer
bool SerialBuffer::init(uint32_t capacity, const uint8_t *buf)
{
  if (m_pBuffer) {
    vfree(m_pBuffer);
    m_pBuffer = nullptr;
  }
  if (capacity) {
    // round up to nearest 4
    m_capacity = (capacity + 4) - (capacity % 4);
    m_pBuffer = (uint8_t *)vcalloc(1, m_capacity);
    if (!m_pBuffer){
      m_capacity = 0;
      ERROR_OUT_OF_MEMORY();
      return false;
    }
  }
  if (buf && m_pBuffer) {
    memcpy(m_pBuffer, buf, m_capacity);
    m_size = m_capacity;
  }
  return true;
}

// append another buffer
bool SerialBuffer::append(const SerialBuffer &other)
{
  if (other.m_size > (m_capacity - m_size)) {
    if (!extend(other.m_size)) {
      return false;
    }
  }
  memcpy(m_pBuffer + m_size, other.m_pBuffer, other.m_size);
  m_size += other.m_size;
  return true;
}

// extend the storage without changing the size of the data
bool SerialBuffer::extend(uint32_t size)
{
  // round size up to nearest 4
  uint32_t new_size = m_capacity + size;
  new_size = (new_size + 4) - (new_size % 4);
  //DEBUGF("Extending %u bytes from %u to %u", size, m_capacity, new_size);
  void *temp = vrealloc(m_pBuffer, new_size);
  if (!temp) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  m_pBuffer = (uint8_t *)temp;
  m_capacity = new_size;
  return true;
}

bool SerialBuffer::largeEnough(uint32_t amount) const
{
  return ((m_size + amount) <= m_capacity);
}

bool SerialBuffer::serialize(uint8_t byte)
{
  //DEBUGF("Serialize8(): %u", byte);
  if ((m_size + sizeof(uint32_t)) > m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
  }
  uint8_t *ptr = m_pBuffer + m_size;
  *ptr = byte;
  m_size += sizeof(uint32_t);
  return true;
}

bool SerialBuffer::serialize(uint16_t bytes)
{
  //DEBUGF("Serialize16(): %u", bytes);
  if ((m_size + sizeof(uint32_t)) > m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
  }
  uint16_t *ptr = (uint16_t *)(m_pBuffer + m_size);
  *ptr = bytes;
  m_size += sizeof(uint32_t);
  return true;
}

bool SerialBuffer::serialize(uint32_t bytes)
{
  //DEBUGF("Serialize32(): %u", bytes);
  if ((m_size + sizeof(uint32_t)) > m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
  }
  uint32_t *ptr = (uint32_t *)(m_pBuffer + m_size);
  *ptr = bytes;
  m_size += sizeof(uint32_t);
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
  if (idx >= m_size) {
    idx = 0;
  }
  m_position = idx;
}

// unserialize data and walk the buffer that many bytes
bool SerialBuffer::unserialize(uint8_t *byte)
{
  if (m_position >= m_size || (m_size - m_position) < sizeof(uint32_t)) {
    return false;
  }
  *byte = m_pBuffer[m_position];
  //DEBUGF("Unserialize8(): %u", *byte);
  m_position += sizeof(uint32_t);
  return true;
}

bool SerialBuffer::unserialize(uint16_t *bytes)
{
  if (m_position >= m_size || (m_size - m_position) < sizeof(uint32_t)) {
    return false;
  }
  *bytes = *(uint16_t *)(m_pBuffer + m_position);
  //DEBUGF("Unserialize16(): %u", *bytes);
  m_position += sizeof(uint32_t);
  return true;
}

bool SerialBuffer::unserialize(uint32_t *bytes)
{
  if (m_position >= m_size || (m_size - m_position) < sizeof(uint32_t)) {
    return false;
  }
  *bytes = *(uint32_t *)(m_pBuffer + m_position);
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
  return *(uint8_t *)(m_pBuffer + m_position);
}

uint16_t SerialBuffer::peek16() const
{
  return *(uint16_t *)(m_pBuffer + m_position);
}

uint32_t SerialBuffer::peek32() const
{
  return *(uint32_t *)(m_pBuffer + m_position);
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
