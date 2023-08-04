BINDIR=$(HOME)/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/7-2017q4/bin/
BOSSACDIR=$(HOME)/.arduino15/packages/arduino/tools/bossac/1.7.0-arduino3/

CC = $(BINDIR)/arm-none-eabi-g++
LD = $(BINDIR)/arm-none-eabi-g++
OBJCOPY = $(BINDIR)/arm-none-eabi-objcopy -v
AR = $(BINDIR)/arm-none-eabi-gcc-ar
SIZE = $(BINDIR)/arm-none-eabi-size
OBJDUMP = $(BINDIR)/arm-none-eabi-objdump
NM = $(BINDIR)/arm-none-eabi-nm
BOSSAC = $(BOSSACDIR)/bossac

PYTHON=/usr/bin/python3
SERIAL_PORT=/dev/ttyACM0

# Other directories
COREDIR=${ARDUINODIR}/hardware/arduino/samd/1.8.9/cores/arduino
VARIANTDIR=${ARDUINODIR}/hardware/arduino/samd/1.8.9/variants/arduino_zero

INCLUDES = \
    -I ${COREDIR} \
    -I ${VARIANTDIR} \
    # Add your other include directories here

# Flags
CFLAGS = \
    -mcpu=cortex-m0plus \
    -mthumb \
    -c \
    -g \
    -Os \
    -Wall \
    -Wextra \
    -Wno-expansion-to-defined \
    -std=gnu++11 \
    -ffunction-sections \
    -fdata-sections \
    -fno-threadsafe-statics \
    -nostdlib \
    --param max-inline-insns-single=500 \
    -fno-rtti \
    -fno-exceptions \
    -MMD \
    "-D__SKETCH_NAME__=\"\"\"VortexEngine.ino\"\"\"" \
    -DF_CPU=48000000L \
    -DARDUINO=10819 \
    -DARDUINO_TRINKET_M0 \
    -DARDUINO_ARCH_SAMD \
    -DCRYSTALLESS \
    -DADAFRUIT_TRINKET_M0 \
    -D__SAMD21E18A__ \
    -DARM_MATH_CM0PLUS \
    -DUSB_VID=0x239A \
    -DUSB_PID=0x801E \
    -DUSBCON \
    -DUSB_CONFIG_POWER=100 \
    "-DUSB_MANUFACTURER=\"Adafruit\"" \
    "-DUSB_PRODUCT=\"Trinket M0\""

LDFLAGS = \
    -Os\
    -Wl,--gc-sections\
    -save-temps\
    "-T${VARIANTDIR}/linker_scripts/gcc/flash_with_bootloader.ld"\
    "-Wl,-Map,VortexEngine.ino.map"\
    --specs=nano.specs\
    --specs=nosys.specs\
    -mcpu=cortex-m0plus\
    -mthumb\
    -Wl,--cref\
    -Wl,--check-sections\
    -Wl,--gc-sections\
    -Wl,--unresolved-symbols=report-all\
    -Wl,--warn-common\
    -Wl,--warn-section-align\
    -Wl,--start-group\
    "-L${ARDUINODIR}/tools/CMSIS/4.5.0/CMSIS/Lib/GCC/"\
    -larm_cortexM0l_math\
    "-L${ARDUINODIR}/hardware/arduino/samd/1.8.9/variants/arduino_zero"

INCLUDES=\
	-I $(INCLUDE_DIR) \
	-I ./VortexEngine/src/

CFLAGS+=$(INCLUDES)

# Source files
SRCS = \
       $(shell find ./VortexEngine/src/ -type f -name \*.cpp) \
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

upload: $(TARGET).bin
	$(BOSSAC) --port=ttyACM0 -U -i -e -w -v $(TARGET).bin -R

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex $(DFILES)

# include dependency files to ensure partial rebuilds work correctly
-include $(DFILES)
