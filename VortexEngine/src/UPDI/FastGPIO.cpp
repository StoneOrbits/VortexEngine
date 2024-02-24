#include "FastGPIO.h"

// Definition for the static member
gpio_num_t FastGPIO::pinNumber;

void FastGPIO::init(gpio_num_t pin)
{
  gpio_reset_pin(pin);
  FastGPIO::pinNumber = pin;
}

void FastGPIO::setOutput()
{
  gpio_set_direction(pinNumber, GPIO_MODE_OUTPUT);
}

void FastGPIO::setInput()
{
  gpio_set_direction(pinNumber, GPIO_MODE_INPUT);
}

void FastGPIO::writeHigh()
{
  gpio_set_level(pinNumber, 1);
}

void FastGPIO::writeLow()
{
  gpio_set_level(pinNumber, 0);
}

void FastGPIO::write(bool high)
{
  gpio_set_level(pinNumber, (int)high);
}

int FastGPIO::read()
{
  return gpio_get_level(pinNumber);
}
