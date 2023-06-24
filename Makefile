# need to install megatinycore and it should work
BINDIR="$(shell echo "$$LOCALAPPDATA")/Arduino15/packages/DxCore/tools/avr-gcc/7.3.0-atmel3.6.1-azduino6/bin"
AVRDUDEDIR="$(shell echo "$$LOCALAPPDATA")/Arduino15/packages/DxCore/tools/avrdude/6.3.0-arduino17or18/bin"

CC = ${BINDIR}/avr-gcc
LD = ${BINDIR}/avr-gcc
OBJCOPY = ${BINDIR}/avr-objcopy -v
AR = ${BINDIR}/avr-gcc-ar
SIZE = ${BINDIR}/avr-size
OBJDUMP = ${BINDIR}/avr-objdump
NM = ${BINDIR}/avr-nm
AVRDUDE = ${AVRDUDEDIR}/avrdude

AVRDUDE_CONF = avrdude.conf
AVRDUDE_PORT = usb
AVRDUDE_PROGRAMMER = atmelice_updi
AVRDUDE_BAUDRATE = 115200
AVRDUDE_CHIP = attiny3217
AVRDUDE_FLAGS = -C$(AVRDUDE_CONF) -v -p$(AVRDUDE_CHIP) -c$(AVRDUDE_PROGRAMMER) -P$(AVRDUDE_PORT) -b$(AVRDUDE_BAUDRATE)

CPU_SPEED=10000000L

CFLAGS = -g \
	 -Os \
	 -MMD \
	 -Wall \
	 -flto \
	 -mrelax \
	 -std=gnu++17 \
	 -fno-threadsafe-statics \
	 -fno-exceptions \
	 -mmcu=$(AVRDUDE_CHIP) \
	 -DF_CPU=$(CPU_SPEED)

LDFLAGS = -g -Wall -Os -flto -fuse-linker-plugin -Wl,--gc-sections -mrelax -mmcu=$(AVRDUDE_CHIP) -lm

INCLUDES=\
	-I ./VortexEngine/src/

CFLAGS+=$(INCLUDES)

# Source files
SRCS = \
       $(shell find ./VortexEngine/src/ -type f -name '\*.cpp') \
       ./VortexEngine/appmain.cpp

OBJS = $(SRCS:.cpp=.o)

DFILES = $(SRCS:.cpp=.d)

# Target name
TARGET = vortex

all: $(TARGET).hex
	$(OBJDUMP) --disassemble --source --line-numbers --demangle --section=.text $(TARGET).elf > $(TARGET).lst
	$(NM) --numeric-sort --line-numbers --demangle --print-size --format=s $(TARGET).elf > $(TARGET).map
	./avrsize.sh $(TARGET).elf

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O binary -R .eeprom $(TARGET).elf $(TARGET).bin
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(TARGET).elf $(TARGET).eep
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.S
	$(CC) $(ASMFLAGS) -c $< -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

#  0x7e = 0x7e00 flash and 0x100 appcode for storage
# fuse6 = 
# fuse7 = APPEND
# fuse8 = BOOTEND
SAVE_EEPROM = 1
FUSE0 = 0b00000000
FUSE2 = 0x02
FUSE5 = 0b1100010$(SAVE_EEPROM)
FUSE6 = 0x04
FUSE7 = 0x00
FUSE8 = 0x7f
upload: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -Ufuse0:w:$(FUSE0):m -Ufuse2:w:$(FUSE2):m -Ufuse5:w:$(FUSE5):m -Ufuse6:w:$(FUSE6):m -Ufuse7:w:$(FUSE7):m -Ufuse8:w:$(FUSE8):m -Uflash:w:$(TARGET).hex:i

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex $(DFILES)

# include dependency files to ensure partial rebuilds work correctly
-include $(DFILES)
