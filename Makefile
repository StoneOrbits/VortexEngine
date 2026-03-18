.PHONY: all install build upload clean compute_version update-index core-list

ARDUINO_CLI = ./bin/arduino-cli --verbose

# Match your local Arduino build options exactly
BOARD = esp32:esp32:XIAO_ESP32C3:CDCOnBoot=default,PartitionScheme=default,CPUFreq=160,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none,EraseFlash=none

# Pin esp32 core to match local
ESP32_CORE_VERSION = 3.0.4

PORT = COMx # Replace 'x' with the appropriate COM port number
PROJECT_NAME = VortexEngine/VortexEngine.ino
BUILD_PATH = build
CONFIG_FILE = $(HOME)/.arduino15/arduino-cli.yaml

# The branch/tag suffix for this device
BRANCH_SUFFIX=a

DEFINES=\
	-D VORTEX_VERSION_MAJOR=$(VORTEX_VERSION_MAJOR) \
	-D VORTEX_VERSION_MINOR=$(VORTEX_VERSION_MINOR) \
	-D VORTEX_BUILD_NUMBER=$(VORTEX_BUILD_NUMBER) \
	-D VORTEX_VERSION_NUMBER=$(VORTEX_VERSION_NUMBER) \
	-MMD -c

all: build

update-index:
	$(ARDUINO_CLI) core update-index

install:
	sudo apt-get update
	sudo apt-get install -y build-essential
	pip install pyserial
	mkdir -p $(HOME)/.arduino15

	# Install arduino-cli if missing
	if ! command -v $(ARDUINO_CLI) &> /dev/null ; then \
		curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sudo sh ; \
	fi

	# Configure esp32 board manager URL
	echo 'board_manager: \n  additional_urls: \n    - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json' | sudo tee $(CONFIG_FILE)

	$(ARDUINO_CLI) lib update-index
	$(ARDUINO_CLI) core update-index --config-file $(CONFIG_FILE)

	# Pin the esp32 core version to match local Arduino
	$(ARDUINO_CLI) core install esp32:esp32@$(ESP32_CORE_VERSION) --config-file $(CONFIG_FILE)

	# Pin FastLED as you already do
	$(ARDUINO_CLI) lib install FastLED@3.7.6

build: compute_version
	$(ARDUINO_CLI) compile --fqbn "$(BOARD)" "$(PROJECT_NAME)" \
		--config-file "$(CONFIG_FILE)" \
		--build-path "$(BUILD_PATH)" \
		--build-property compiler.cpp.extra_flags="$(DEFINES)" \
		--build-property compiler.c.extra_flags="$(DEFINES)"
	@echo "== Success building Axon v$(VORTEX_VERSION_NUMBER) =="

upload:
	$(ARDUINO_CLI) upload -p "$(PORT)" --fqbn "$(BOARD)" "$(PROJECT_NAME)" --config-file "$(CONFIG_FILE)"

core-list:
	$(ARDUINO_CLI) core list

clean:
	rm -rf "$(BUILD_PATH)"

compute_version:
	$(eval LATEST_TAG ?= $(shell git fetch --depth=1 origin +refs/tags/*:refs/tags/* &> /dev/null && git tag --list "*$(BRANCH_SUFFIX)" | sort -V | tail -n1))
	$(eval VORTEX_VERSION_MAJOR ?= $(shell echo $(LATEST_TAG) | cut -d. -f1))
	$(eval VORTEX_VERSION_MINOR ?= $(shell echo $(LATEST_TAG) | sed 's/$(BRANCH_SUFFIX)$$//' | cut -d. -f2))
	$(eval VORTEX_BUILD_NUMBER ?= $(shell git rev-list --count $(LATEST_TAG)..HEAD))
	$(eval VORTEX_VERSION_MAJOR := $(if $(VORTEX_VERSION_MAJOR),$(VORTEX_VERSION_MAJOR),0))
	$(eval VORTEX_VERSION_MINOR := $(if $(VORTEX_VERSION_MINOR),$(VORTEX_VERSION_MINOR),1))
	$(eval VORTEX_BUILD_NUMBER := $(if $(VORTEX_BUILD_NUMBER),$(VORTEX_BUILD_NUMBER),0))
	$(eval VORTEX_VERSION_NUMBER := $(VORTEX_VERSION_MAJOR).$(VORTEX_VERSION_MINOR).$(VORTEX_BUILD_NUMBER))

