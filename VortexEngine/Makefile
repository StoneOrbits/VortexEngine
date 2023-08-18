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
    vortex.a \
	vortex_test \

# Default target for 'make' command
all: $(TARGETS)

# unit test target
tests: $(TESTS)

vortex.a: 
	$(MAKE) -C VortexLIB
	cp -f VortexLIB/vortex.a .

vortex: 
	$(MAKE) -C VortexCLI
	cp -f VortexCLI/vortex .

# generic clean target
clean:
	@$(RM) $(TARGETS) $(TESTS)
	$(MAKE) -C VortexLIB clean
	$(MAKE) -C VortexCLI clean
