# Microcontroller settings
MCU = attiny3217
F_CPU = 8000000UL
BAUD_RATE = 9600

# Compiler settings
CC = avr-g++
OBJCOPY = avr-objcopy
CFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -DBAUD_RATE=$(BAUD_RATE) -mmcu=$(MCU) \
  -std=gnu++17 \
  -fpermissive \
  -Wno-sized-deallocation \
  -fno-exceptions \
  -ffunction-sections \
  -fdata-sections \
  -fno-threadsafe-statics \
  -Wno-error=narrowing \
  -flto \
  -mrelax \
  -w \
  -x c++ \
  -E \
  -CC \
  -mmcu=attiny3217 \
  -DF_CPU=20000000L \
  -DCLOCK_SOURCE=0 \
  -DTWI_MORS \
  -DMILLIS_USE_TIMERD0 \
  -DCORE_ATTACH_ALL \
  -DUSE_TIMERD0_PWM \
  -DARDUINO=10819 \
  -DARDUINO_AVR_ATtiny3217 \
  -DARDUINO_ARCH_MEGAAVR \
  -DMEGATINYCORE=\"2.6.7.1\" \
  -DMEGATINYCORE_MAJOR=2UL \
  -DMEGATINYCORE_MINOR=6UL \
  -DMEGATINYCORE_PATCH=7UL \
  -DMEGATINYCORE_RELEASED=1 \
  -DARDUINO_attinyxy7  \
  -I C:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\megaTinyCore\\hardware\\megaavr\\2.6.7\\cores\\megatinycore/api/deprecated \
  -I C:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\megaTinyCore\\hardware\\megaavr\\2.6.7\\cores\\megatinycore \
  -I C:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\megaTinyCore\\hardware\\megaavr\\2.6.7\\variants\\txy7

LDFLAGS = -mmcu=$(MCU)

INCLUDES=\
	-I ./VortexEngine/src/ \
	-I ./VortexEngine/VortexLib/EngineDependencies 

ifneq ($(INCLUDES),)
    CFLAGS+=$(INCLUDES)
endif

# Source files
SRCS = main.cpp \
       $(shell find ./VortexEngine/src/ -type f -name '*.cpp')

OBJS = $(SRCS:.cpp=.o)

# Target name
TARGET = main

all: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

flash: $(TARGET).hex
	avrdude -p $(MCU) -c jtag2updi -P COM14 -U flash:w:$<

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex

