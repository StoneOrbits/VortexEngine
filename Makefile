# Compiler settings
CC = C:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\DxCore\\tools\\avr-gcc\\7.3.0-atmel3.6.1-azduino6/bin/avr-g++.exe
LD = C:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\DxCore\\tools\\avr-gcc\\7.3.0-atmel3.6.1-azduino6/bin/avr-gcc.exe
OBJCOPY = C:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\DxCore\\tools\\avr-gcc\\7.3.0-atmel3.6.1-azduino6/bin/avr-objcopy.exe
CFLAGS = -Wall -Os \
  -std=gnu++17 \
  -fpermissive \
  -Wno-sized-deallocation \
  -fno-exceptions \
  -ffunction-sections \
  -fdata-sections \
  -fno-threadsafe-statics \
  -Wno-error=narrowing \
  -MMD \
  -flto \
  -mrelax \
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

#LDFLAGS = -mmcu=attiny3217 -nostartfiles -flto -fuse-linker-plugin -Wl,--gc-sections -Wl,--section-start=.text=0x0 -lm

LDFLAGS = -Wall -Os -flto -fuse-linker-plugin -Wl,--gc-sections -Wl,--section-start=.text=0x0 -mrelax -mmcu=attiny3217 -lm

INCLUDES=\
	-I ./VortexEngine/src/ \
	-I ./VortexEngine/VortexLib/EngineDependencies/

ifneq ($(INCLUDES),)
    CFLAGS+=$(INCLUDES)
endif

# Source files
SRCS = \
	./VortexEngine/src/Buttons/Button.cpp \
	./VortexEngine/src/Buttons/Buttons.cpp \
	./VortexEngine/src/Colors/Colorset.cpp \
	./VortexEngine/src/Colors/ColorTypes.cpp \
	./VortexEngine/src/Infrared/IRReceiver.cpp \
	./VortexEngine/src/Infrared/IRSender.cpp \
	./VortexEngine/src/Leds/Leds.cpp \
	./VortexEngine/src/Leds/LedStash.cpp \
	./VortexEngine/src/Log/ErrorBlinker.cpp \
	./VortexEngine/src/Log/Log.cpp \
	./VortexEngine/src/Memory/Memory.cpp \
	./VortexEngine/src/Menus/Menu.cpp \
	./VortexEngine/src/Menus/MenuList/ColorSelect.cpp \
	./VortexEngine/src/Menus/MenuList/EditorConnection.cpp \
	./VortexEngine/src/Menus/MenuList/FactoryReset.cpp \
	./VortexEngine/src/Menus/MenuList/GlobalBrightness.cpp \
	./VortexEngine/src/Menus/MenuList/ModeSharing.cpp \
	./VortexEngine/src/Menus/MenuList/PatternSelect.cpp \
	./VortexEngine/src/Menus/MenuList/Randomizer.cpp \
	./VortexEngine/src/Menus/Menus.cpp \
	./VortexEngine/src/Modes/DefaultModes.cpp \
	./VortexEngine/src/Modes/Mode.cpp \
	./VortexEngine/src/Modes/Modes.cpp \
	./VortexEngine/src/Patterns/Multi/BackStrobePattern.cpp \
	./VortexEngine/src/Patterns/Multi/BlinkStepPattern.cpp \
	./VortexEngine/src/Patterns/Multi/BouncePattern.cpp \
	./VortexEngine/src/Patterns/Multi/CompoundPattern.cpp \
	./VortexEngine/src/Patterns/Multi/CrossDopsPattern.cpp \
	./VortexEngine/src/Patterns/Multi/DoubleStrobePattern.cpp \
	./VortexEngine/src/Patterns/Multi/DripMorphPattern.cpp \
	./VortexEngine/src/Patterns/Multi/DripPattern.cpp \
	./VortexEngine/src/Patterns/Multi/FillPattern.cpp \
	./VortexEngine/src/Patterns/Multi/HueShiftPattern.cpp \
	./VortexEngine/src/Patterns/Multi/LighthousePattern.cpp \
	./VortexEngine/src/Patterns/Multi/MateriaPattern.cpp \
	./VortexEngine/src/Patterns/Multi/MeteorPattern.cpp \
	./VortexEngine/src/Patterns/Multi/MultiLedPattern.cpp \
	./VortexEngine/src/Patterns/Multi/PulsishPattern.cpp \
	./VortexEngine/src/Patterns/Multi/Sequencer/ChaserPattern.cpp \
	./VortexEngine/src/Patterns/Multi/Sequencer/Sequence.cpp \
	./VortexEngine/src/Patterns/Multi/Sequencer/SequencedPattern.cpp \
	./VortexEngine/src/Patterns/Multi/SnowballPattern.cpp \
	./VortexEngine/src/Patterns/Multi/SparkleTracePattern.cpp \
	./VortexEngine/src/Patterns/Multi/TheaterChasePattern.cpp \
	./VortexEngine/src/Patterns/Multi/VortexWipePattern.cpp \
	./VortexEngine/src/Patterns/Multi/WarpPattern.cpp \
	./VortexEngine/src/Patterns/Multi/WarpWormPattern.cpp \
	./VortexEngine/src/Patterns/Multi/ZigzagPattern.cpp \
	./VortexEngine/src/Patterns/Pattern.cpp \
	./VortexEngine/src/Patterns/PatternArgs.cpp \
	./VortexEngine/src/Patterns/PatternBuilder.cpp \
	./VortexEngine/src/Patterns/Single/BasicPattern.cpp \
	./VortexEngine/src/Patterns/Single/BlendPattern.cpp \
	./VortexEngine/src/Patterns/Single/DashDopsPattern.cpp \
	./VortexEngine/src/Patterns/Single/SingleLedPattern.cpp \
	./VortexEngine/src/Patterns/Single/SolidPattern.cpp \
	./VortexEngine/src/Patterns/Single/TracerPattern.cpp \
	./VortexEngine/src/Random/Random.cpp \
	./VortexEngine/src/Serial/BitStream.cpp \
	./VortexEngine/src/Serial/ByteStream.cpp \
	./VortexEngine/src/Serial/Compression.cpp \
	./VortexEngine/src/Serial/Serial.cpp \
	./VortexEngine/src/Storage/Storage.cpp \
	./VortexEngine/src/Time/TimeControl.cpp \
	./VortexEngine/src/Time/Timer.cpp \
	./VortexEngine/src/VortexEngine.cpp \
        ./main.cpp

OBJS = $(SRCS:.cpp=.o)

# Target name
TARGET = main

all: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

flash: $(TARGET).hex
	avrdude -p $(MCU) -c jtag2updi -P COM14 -U flash:w:$<

upload: $(TARGET).hex
	avrdude -p $(MCU) -c jtag2updi -P COM14 -U flash:w:$< -U lfuse:w:0xFF:m -U hfuse:w:

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex

