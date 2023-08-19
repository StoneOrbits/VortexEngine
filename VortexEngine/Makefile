# clear out all default make targets
.SUFFIXES:

# List all make targets which are not filenames
.PHONY: all tests clean wasm

MAKE=make
RM=rm -rf
RANLIB=ranlib

# target files
TARGETS=\
    vortex \
    vortex.a

.PHONY: $(TARGETS)

# Default target for 'make' command
all: $(TARGETS)

# unit test target
tests: $(TESTS)

vortex.a: 
	$(MAKE) -C VortexLib
	cp -f VortexLib/vortex.a .

vortex: 
	$(MAKE) -C VortexCLI
	cp -f VortexCLI/vortex .

# generic clean target
clean:
	@$(RM) $(TARGETS) $(TESTS) vortex.exe
	$(MAKE) -C VortexLib clean
	$(MAKE) -C VortexCLI clean
