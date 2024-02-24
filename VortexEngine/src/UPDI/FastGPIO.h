#ifndef FAST_GPIO_H
#define FAST_GPIO_H

#include "driver/gpio.h"
#include <vector>

class FastGPIO {
public:
    static void init(gpio_num_t pin);
    static void setOutput();
    static void setInput();
    static void writeHigh();
    static void writeLow();
    static void write(bool high);
    static int read();

private:
    static gpio_num_t pinNumber;
};

#endif