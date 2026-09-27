# clear out all default make targets
.SUFFIXES:

# List all make targets which are not filenames
.PHONY: all clean wasm vortex vortex.a lib

MAKE=make
RM=rm -rf
RANLIB=ranlib

# ============================================================
# Embedded firmware target (pure C, default)
# ============================================================
# Set these to match your target hardware
MCU=attiny3217
F_CPU=20000000

CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size

CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -DVORTEX_EMBEDDED -DVORTEX_SLIM=1 -Os -Wall -Wextra -std=gnu11
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS=-mmcu=$(MCU) -Wl,-gc-sections
INCLUDES=-I src/

# All .c files in src/ tree, excluding multi-led patterns (VORTEX_SLIM)
SRC=$(shell find src/ -type f -name '*.c' \
  ! -path 'src/Patterns/Multi/*' \
  ! -path 'src/Serial/Compression.c') appmain.c
OBJS=$(SRC:.c=.o)

TARGET=vortex

all: $(TARGET).hex $(TARGET).elf

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	$(SIZE) $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================
# Desktop targets (delegated to sub-makefiles)
# ============================================================

# force sub-build of wasm
wasm: FORCE
	env WASM=1 $(MAKE)

vortex.a:
	$(MAKE) -C VortexLib
	cp -f VortexLib/vortex.a .

vortex:
	$(MAKE) -C VortexCLI
	cp -f VortexCLI/vortex .

lib: vortex.a

# Empty rule forces stuff to re-run every time
FORCE:

# generic clean target
clean:
	@$(RM) $(TARGETS) $(TESTS) vortex.exe vortex.elf vortex.hex $(OBJS)
	$(MAKE) -C VortexLib clean
	$(MAKE) -C VortexCLI clean
