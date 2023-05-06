CC = C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin/avr-gcc
LD = C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin/avr-gcc
OBJCOPY = C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin/avr-objcopy -v
AR = C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin/avr-gcc-ar
SIZE = C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin/avr-size
OBJDUMP = C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin/avr-objdump
NM = C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin/avr-nm
AVRDUDE =  C:/Users/danie/AppData/Local/Arduino15/packages/DxCore/tools/avrdude/6.3.0-arduino17or18/bin/avrdude

AVRDUDE_CONF = C:/Users/danie/source/repos/VortexAVR/VortexEngine/avrdude.conf
AVRDUDE_PORT = COM12
AVRDUDE_BAUDRATE = 115200
AVRDUDE_FLAGS = -C$(AVRDUDE_CONF) -v -pattiny3217 -cjtag2updi -P$(AVRDUDE_PORT) -b$(AVRDUDE_BAUDRATE)

ASMFLAGS = \
  -x assembler-with-cpp \
  -flto \
  -MMD \
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
  -DARDUINO_attinyxy7 \
  -I C:/Users/danie/AppData/Local/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.7/cores/megatinycore/api/deprecated \
  -I C:/Users/danie/AppData/Local/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.7/cores/megatinycore \
  -I C:/Users/danie/AppData/Local/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.7/variants/txy7 

CFLAGS = \
  -Wall \
  -Os \
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
  -I C:/Users/danie/AppData/Local/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.7/cores/megatinycore/api/deprecated \
  -I C:/Users/danie/AppData/Local/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.7/cores/megatinycore \
  -I C:/Users/danie/AppData/Local/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.7/libraries/EEPROM/src/ \
  -I C:/Users/danie/AppData/Local/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.7/variants/txy7

#LDFLAGS = -mmcu=attiny3217 -nostartfiles -flto -fuse-linker-plugin -Wl,--gc-sections -Wl,--section-start=.text=0x0 -lm

LDFLAGS = -Wall -Os -flto -fuse-linker-plugin -Wl,--gc-sections -Wl,--section-start=.text=0x0 -mrelax -mmcu=attiny3217 -lm -Wl,-T,custom.xn

INCLUDES=\
	-I ./VortexEngine/src/ \
	-I ./VortexEngine/VortexLib/EngineDependencies/

ifneq ($(INCLUDES),)
    CFLAGS+=$(INCLUDES)
endif

# Source files
SRCS = \
	./VortexEngine/src/Colors/ColorTypes.cpp \
	./VortexEngine/src/Leds/Leds.cpp \
	./VortexEngine/src/Log/ErrorBlinker.cpp \
	./VortexEngine/src/Log/Log.cpp \
	./VortexEngine/src/Memory/Memory.cpp \
	./VortexEngine/src/Modes/Mode.cpp \
	./VortexEngine/src/Modes/Modes.cpp \
	./VortexEngine/src/Serial/BitStream.cpp \
	./VortexEngine/src/Serial/ByteStream.cpp \
	./VortexEngine/src/Serial/Compression.cpp \
	./VortexEngine/src/Serial/Serial.cpp \
	./VortexEngine/src/Storage/Storage.cpp \
	./VortexEngine/src/VortexEngine.cpp \

OBJS = $(SRCS:.cpp=.o)

COREASM = \
	./libraries/megatinycore/wiring_pulse.S

CORESRC = \
	./libraries/megatinycore/abi.cpp \
	./libraries/megatinycore/api/Common.cpp \
	./libraries/megatinycore/api/IPAddress.cpp \
	./libraries/megatinycore/api/PluggableUSB.cpp \
	./libraries/megatinycore/api/Print.cpp \
	./libraries/megatinycore/api/RingBuffer.cpp \
	./libraries/megatinycore/api/Stream.cpp \
	./libraries/megatinycore/api/String.cpp \
	./libraries/megatinycore/ExtraWiring.cpp \
	./libraries/megatinycore/main.cpp \
	./libraries/megatinycore/new.cpp \
	./libraries/megatinycore/Tone.cpp \
	./libraries/megatinycore/UART.cpp \
	./libraries/megatinycore/UART0.cpp \
	./libraries/megatinycore/UART1.cpp \
	./libraries/megatinycore/wiring_extra.cpp \
	./libraries/megatinycore/WMath.cpp \
	./appmain.cpp

CORESRCC = \
	./libraries/megatinycore/hooks.c \
	./libraries/megatinycore/WInterrupts.c \
	./libraries/megatinycore/WInterrupts_PA.c \
	./libraries/megatinycore/WInterrupts_PB.c \
	./libraries/megatinycore/WInterrupts_PC.c \
	./libraries/megatinycore/wiring.c \
	./libraries/megatinycore/wiring_analog.c \
	./libraries/megatinycore/wiring_digital.c \
	./libraries/megatinycore/wiring_pulse.c \
	./libraries/megatinycore/wiring_shift.c 

COREOBJS = $(COREASM:.S=.o) $(CORESRC:.cpp=.o) $(CORESRCC:.c=.o)

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
		-Ufuse8:w:0x00:m \
		-Uflash:w:$(TARGET).hex:i

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex core.a $(COREOBJS)

