ifeq ($(OS),Windows_NT) # Windows
    BINDIR="C:/Program Files (x86)/Atmel/Studio/7.0/toolchain/avr8/avr8-gnu-toolchain/bin/"
    AVRDUDEDIR="$(shell echo "$$LOCALAPPDATA")/Arduino15/packages/DxCore/tools/avrdude/6.3.0-arduino17or18/bin"
    PYTHON="$(shell echo "$$LOCALAPPDATA")/Arduino15/packages/megaTinyCore/tools/python3/3.7.2-post1/python3"
    PYPROG="$(shell echo "$$LOCALAPPDATA")/Arduino15/packages/megaTinyCore/hardware/megaavr/2.6.5/tools/prog.py"
    DEVICE_DIR="C:/Program Files (x86)/Atmel/Studio/7.0/Packs/atmel/ATtiny_DFP/1.10.348/gcc/dev/attiny3217"
    INCLUDE_DIR="C:/Program Files (x86)/Atmel/Studio/7.0/Packs/atmel/ATtiny_DFP/1.10.348/include/"
else # linux
    BINDIR=~/atmel_setup/avr8-gnu-toolchain-linux_x86_64/bin/
    DEVICE_DIR=~/atmel_setup/gcc/dev/attiny3217
    INCLUDE_DIR=~/atmel_setup/include/
endif

CC = ${BINDIR}/avr-g++
LD = ${BINDIR}/avr-g++
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

AVRDUDE_FLAGS = -C$(AVRDUDE_CONF) \
		-p$(AVRDUDE_CHIP) \
		-c$(AVRDUDE_PROGRAMMER) \
		-P$(AVRDUDE_PORT) \
		-b$(AVRDUDE_BAUDRATE) \
		-v

CPU_SPEED = 10000000L

# the port for serial upload
SERIAL_PORT = COM11

SAVE_EEPROM = 1
FUSE0 = 0b00000000
FUSE2 = 0x02
FUSE5 = 0b1100010$(SAVE_EEPROM)
FUSE6 = 0x04
# fuse7 = APPEND
FUSE7 = 0x00
# fuse8 = BOOTEND
FUSE8 = 0x7e

CFLAGS = -g \
	 -Os \
	 -MMD \
	 -Wall \
	 -flto \
	 -mrelax \
	 -std=gnu++17 \
	 -fshort-enums \
	 -fpack-struct \
	 -fno-exceptions \
	 -fdata-sections \
	 -funsigned-char \
	 -ffunction-sections\
	 -funsigned-bitfields \
	 -fno-threadsafe-statics \
	 -D__AVR_ATtiny3217__ \
	 -mmcu=$(AVRDUDE_CHIP) \
	 -DF_CPU=$(CPU_SPEED) \
	 -B $(DEVICE_DIR)

LDFLAGS = -g \
	  -Wall \
	  -Os \
	  -flto \
	  -fuse-linker-plugin \
	  -Wl,--gc-sections \
	  -mrelax \
	  -lm \
	  -mmcu=$(AVRDUDE_CHIP) \
	  -B $(DEVICE_DIR)

INCLUDES=\
	-I $(INCLUDE_DIR) \
	-I ./VortexEngine/src/

CFLAGS+=$(INCLUDES)

# Source files
ifeq ($(OS),Windows_NT) # Windows
SRCS = \
       $(shell find ./VortexEngine/src/ -type f -name '\*.cpp') \
       ./VortexEngine/appmain.cpp
else # linux
SRCS = \
       $(shell find ./VortexEngine/src/ -type f -name \*.cpp) \
       ./VortexEngine/appmain.cpp
endif

OBJS = $(SRCS:.cpp=.o)

DFILES = $(SRCS:.cpp=.d)

# Target name
TARGET = vortex

all: $(TARGET).hex
	$(OBJDUMP) --disassemble --source --line-numbers --demangle --section=.text $(TARGET).elf > $(TARGET).lst
	$(NM) --numeric-sort --line-numbers --demangle --print-size --format=s $(TARGET).elf > $(TARGET).map
	chmod +x avrsize.sh
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

readstorage:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -Ueeprom:r:eeprom.hex:h -Uuserrow:r:userrow.hex:h
	@if [ "$(OS)" = "Windows_NT" ]; then \
		type eeprom.hex & type userrow.hex; \
	else \
		cat eeprom.hex & cat userrow.hex; \
	fi

readflash:
	$(PYTHON) -u $(PYPROG) -t uart -u $(SERIAL_PORT) -b 921600 -d $(AVRDUDE_CHIP) --action read -f flash_dump.hex
	@if [ "$(OS)" = "Windows_NT" ]; then \
		type flash_dump.hex; \
	else \
		cat flash_dump.hex; \
	fi

ifneq ($(OS),Windows_NT) # Linux
build: all
INSTALL_DIR=~/atmel_setup
# Name of the toolchain tarball
TOOLCHAIN_TAR=avr8-gnu-toolchain-3.7.0.1796-linux.any.x86_64.tar.gz
# Name of the ATtiny DFP zip
ATTINY_ZIP=Atmel.ATtiny_DFP.2.0.368.atpack
install:
	@echo "Setting up in directory $(INSTALL_DIR)"
	@mkdir -p $(INSTALL_DIR)
	@cd $(INSTALL_DIR) && \
	echo "Downloading and installing AVR 8-bit Toolchain..." && \
	wget -q https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools/$(TOOLCHAIN_TAR) && \
	tar -xf $(TOOLCHAIN_TAR) && \
	echo "Downloading and installing ATtiny DFP..." && \
	wget -q http://packs.download.atmel.com/$(ATTINY_ZIP) && \
	unzip $(ATTINY_ZIP)
	@echo "Download and extraction complete. You'll find the toolchain and pack files in $(INSTALL_DIR)"
endif

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex $(DFILES)

# include dependency files to ensure partial rebuilds work correctly
-include $(DFILES)
