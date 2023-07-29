# need to install megatinycore and it should work
BINDIR="C:/Users/danie/AppData/Local/Arduino15/packages/arduino/tools/arm-none-eabi-gcc/7-2017q4/bin/"
AVRDUDEDIR="$(shell echo "$$LOCALAPPDATA")/Arduino15/packages/DxCore/tools/avrdude/6.3.0-arduino17or18/bin"

# tools for serial upload
PYTHON="$(shell echo "$$LOCALAPPDATA")/Arduino15/packages/megaTinyCore/tools/python3/3.7.2-post1/python3"
PYPROG="$(shell echo "$$LOCALAPPDATA")/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.5/tools/prog.py"

DEVICE_DIR="C:/Program Files (x86)/Atmel/Studio/7.0/Packs/atmel/ATtiny_DFP/1.10.348/gcc/dev/attiny3217"
INCLUDE_DIR="C:/Program Files (x86)/Atmel/Studio/7.0/Packs/atmel/ATtiny_DFP/1.10.348/include/"

CC = ${BINDIR}/arm-none-eabi-g++
LD = ${BINDIR}/arm-none-eabi-g++
OBJCOPY = ${BINDIR}/arm-none-eabi-objcopy -v
AR = ${BINDIR}/arm-none-eabi-gcc-ar
SIZE = ${BINDIR}/arm-none-eabi-size
OBJDUMP = ${BINDIR}/arm-none-eabi-objdump
NM = ${BINDIR}/arm-none-eabi-nm
AVRDUDE = ${AVRDUDEDIR}/avrdude

AVRDUDE_CONF = avrdude.conf
AVRDUDE_PORT = usb
AVRDUDE_PROGRAMMER = atmelice_updi
AVRDUDE_BAUDRATE = 115200
AVRDUDE_CHIP = attiny3217

AVRDUDE_FLAGS = -C$(AVRDUDE_CONF) \
		-p$(AVRDUDE_CHIP) \
		-c$(AVRDUDE_PROGRAMMER) \
		-P$(AVRDUDE_PORT) \
		-b$(AVRDUDE_BAUDRATE) \
		-v

CPU_SPEED = 48000000L

# the port for serial upload
SERIAL_PORT = COM41

SAVE_EEPROM = 1
FUSE0 = 0b00000000
FUSE2 = 0x02
FUSE5 = 0b1100010$(SAVE_EEPROM)
FUSE6 = 0x04
# fuse7 = APPEND
FUSE7 = 0x00
# fuse8 = BOOTEND
FUSE8 = 0x7e

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
	"-DUSB_PRODUCT=\"Trinket M0\"" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\cores\\arduino/TinyUSB" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\cores\\arduino/TinyUSB/Adafruit_TinyUSB_ArduinoCore" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\cores\\arduino/TinyUSB/Adafruit_TinyUSB_ArduinoCore/tinyusb/src" \
	-DCRYSTALLESS \
	-DADAFRUIT_TRINKET_M0 \
	-D__SAMD21E18A__ \
	-DARM_MATH_CM0PLUS \
	-DUSB_VID=0x239A \
	-DUSB_PID=0x801E \
	-DUSBCON \
	-DUSB_CONFIG_POWER=100 \
	"-DUSB_MANUFACTURER=\"Adafruit\"" \
	"-DUSB_PRODUCT=\"Trinket M0\"" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\cores\\arduino/TinyUSB" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\cores\\arduino/TinyUSB/Adafruit_TinyUSB_ArduinoCore" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\cores\\arduino/TinyUSB/Adafruit_TinyUSB_ArduinoCore/tinyusb/src" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\CMSIS\\4.5.0/CMSIS/Include/" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\CMSIS-Atmel\\1.2.0/CMSIS/Device/ATMEL/" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\cores\\arduino" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\variants\\trinket_m0" \
	"-IC:\\Users\\danie\\OneDrive\\Documents\\Arduino\\libraries\\FastLED-master" \
	"-IC:\\Users\\danie\\OneDrive\\Documents\\Arduino\\libraries\\Adafruit_DotStar" \
	"-IC:\\Users\\danie\\OneDrive\\Documents\\Arduino\\libraries\\FlashStorage-master\\src" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\libraries\\SPI" \
	"-IC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\libraries\\Adafruit_ZeroDMA" 

LDFLAGS = \
	-Os\
	-Wl,--gc-sections\
	-save-temps\
	"-TC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\variants\\trinket_m0/linker_scripts/gcc/flash_with_bootloader.ld"\
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
	"-LC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\CMSIS\\4.5.0/CMSIS/Lib/GCC/"\
	-larm_cortexM0l_math\
	"-LC:\\Users\\danie\\AppData\\Local\\Arduino15\\packages\\adafruit\\hardware\\samd\\1.5.14\\variants\\trinket_m0"

INCLUDES=\
	-I $(INCLUDE_DIR) \
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

upload: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -Ufuse0:w:$(FUSE0):m -Ufuse2:w:$(FUSE2):m -Ufuse5:w:$(FUSE5):m -Ufuse6:w:$(FUSE6):m -Ufuse7:w:$(FUSE7):m -Ufuse8:w:$(FUSE8):m -Uflash:w:$(TARGET).hex:i

# upload via SerialUPDI
serial: $(TARGET).hex
	$(PYTHON) -u $(PYPROG) -t uart -u $(SERIAL_PORT) -b 921600 -d $(AVRDUDE_CHIP) --fuses 0:$(FUSE0) 2:$(FUSE2) 5:$(FUSE5) 6:$(FUSE6) 7:$(FUSE7) 8:$(FUSE8) -f $< -a write -v

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex $(DFILES)

# include dependency files to ensure partial rebuilds work correctly
-include $(DFILES)
