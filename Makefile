BINDIR=C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin

AVRDUDEDIR=C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avrdude/6.3.0-arduino17or18/bin

CC = ${BINDIR}/avr-gcc
LD = ${BINDIR}/avr-gcc
OBJCOPY = ${BINDIR}/avr-objcopy -v
AR = ${BINDIR}/avr-gcc-ar
SIZE = ${BINDIR}/avr-size
OBJDUMP = ${BINDIR}/avr-objdump
NM = ${BINDIR}/avr-nm
AVRDUDE = ${AVRDUDEDIR}/avrdude

AVRDUDE_CONF = C:/Users/danie/source/repos/VortexAVR/VortexEngine/avrdude.conf
AVRDUDE_PORT = COM12
AVRDUDE_PROGRAMMER = jtag2updi
AVRDUDE_BAUDRATE = 115200
AVRDUDE_FLAGS = -C$(AVRDUDE_CONF) -v -pattiny3217 -c$(AVRDUDE_PROGRAMMER) -P$(AVRDUDE_PORT) -b$(AVRDUDE_BAUDRATE)

CFLAGS = \
  -Os \
  -MMD \
  -Wall \
  -flto \
  -mrelax \
  -std=gnu++17 \
  -fpermissive \
  -fdata-sections \
  -fno-exceptions \
  -mmcu=attiny3217 \
  -DF_CPU=10000000L \
  -ffunction-sections \
  -Wno-error=narrowing \
  -fno-threadsafe-statics \
  -Wno-sized-deallocation \
  -I C:/Users/danie/AppData/Local/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.7/libraries/EEPROM/src/ \

LDFLAGS = -Wall -Os -flto -fuse-linker-plugin -Wl,--gc-sections -mrelax -mmcu=attiny3217 -lm

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
	./appmain.cpp

OBJS = $(SRCS:.cpp=.o)

CORESRCC = \

COREOBJS = $(CORESRCC:.c=.o)

DFILES = $(SRCS:.cpp=.d) $(CORESRCC:.c:.d)

# Target name
TARGET = main

all: $(TARGET).hex
	$(OBJDUMP) --disassemble --source --line-numbers --demangle --section=.text $(TARGET).elf > $(TARGET).lst
	#$(OBJDUMP) --disassemble --source --line-numbers --demangle --section=.storage $(TARGET).elf > $(TARGET)-storage.lst
	$(NM) --numeric-sort --line-numbers --demangle --print-size --format=s $(TARGET).elf > $(TARGET).map
	./avrsize.sh

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O binary -R .eeprom $(TARGET).elf $(TARGET).bin
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).elf: $(OBJS) core.a
	$(LD) $(LDFLAGS) $^ -o $@

core.a: $(COREOBJS)
	$(AR) rcs $@ $^

%.o: %.S
	$(CC) $(ASMFLAGS) -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# fuse7 = APPEND
# fuse8 = BOOTEND
upload: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) \
		-Ufuse0:w:0b00000000:m \
		-Ufuse2:w:0x02:m \
		-Ufuse5:w:0b11000101:m \
		-Ufuse6:w:0x04:m \
		-Ufuse7:w:0x00:m \
		-Ufuse8:w:0x76:m \
		-Uflash:w:$(TARGET).hex:i

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex core.a $(COREOBJS)

-include $(DFILES)
