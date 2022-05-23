#include "SerialBuffer.h"

#include "Log.h"

#include <stdlib.h>
#include <string.h>

SerialBuffer::SerialBuffer(uint32_t size, const uint8_t *buf) :
  m_pBuffer(nullptr),
  m_size(0),
  m_capacity(0)
{
  init(size, buf);
}

SerialBuffer::~SerialBuffer()
{
  if (m_pBuffer) {
    free(m_pBuffer);
  }
}

// reset the buffer
bool SerialBuffer::init(uint32_t capacity, const uint8_t *buf)
{
  if (m_pBuffer) {
    free(m_pBuffer);
  }
  if (capacity) {
    // round up to nearest 4
    m_capacity = (capacity + 4) - (capacity % 4);
    m_pBuffer = (uint8_t *)calloc(1, m_capacity);
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
}

// extend the storage without changing the size of the data
bool SerialBuffer::extend(uint32_t size)
{
  // round size up to nearest 4
  uint32_t new_size = m_capacity + ((size + 4) - (size % 4));
  void *temp = realloc(m_pBuffer, size);
  if (!temp) {
    ERROR_OUT_OF_MEMORY();
    return false;
  }
  m_pBuffer = (uint8_t *)temp;
  m_capacity = new_size;
}

bool SerialBuffer::serialize(uint8_t byte)
{
  if ((m_size + sizeof(uint8_t)) >= m_capacity) {
    if (!extend(sizeof(uint8_t))) {
      return false;
    }
    m_pBuffer[m_size++] = byte;
  }
  return true;
}

bool SerialBuffer::serialize(uint16_t bytes)
{
  if ((m_size + sizeof(uint16_t)) >= m_capacity) {
    if (!extend(sizeof(uint16_t))) {
      return false;
    }
    m_pBuffer[m_size++] = bytes & 0xFF;
    m_pBuffer[m_size++] = (bytes >> 8) & 0xFF;
  }
  return true;
}

bool SerialBuffer::serialize(uint32_t bytes)
{
  if ((m_size + sizeof(uint32_t)) >= m_capacity) {
    if (!extend(sizeof(uint32_t))) {
      return false;
    }
    m_pBuffer[m_size++] = bytes & 0xFF;
    m_pBuffer[m_size++] = (bytes >> 8) & 0xFF;
    m_pBuffer[m_size++] = (bytes >> 16) & 0xFF;
    m_pBuffer[m_size++] = (bytes >> 24) & 0xFF;
  }
  return true;
}

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
