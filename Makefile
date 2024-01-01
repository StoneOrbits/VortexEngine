.PHONY: all install build upload clean

ARDUINO_CLI = ./bin/arduino-cli --verbose
BOARD = esp32:esp32:esp32s3
PORT = COMx # Replace 'x' with the appropriate COM port number
PROJECT_NAME = VortexEngine/VortexEngine.ino
BUILD_PATH = build
CONFIG_FILE = $(HOME)/.arduino15/arduino-cli.yaml

# Default target
all: install build

update-index:
	$(ARDUINO_CLI) core update-index

install:
	sudo apt-get update
	sudo apt-get install -y build-essential
	pip install pyserial
	mkdir -p $(HOME)/.arduino15
	if ! command -v $(ARDUINO_CLI) &> /dev/null ; then \
		curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sudo sh ; \
	fi
	echo 'board_manager: \n  additional_urls: \n    - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json' | sudo tee $(CONFIG_FILE)
	$(ARDUINO_CLI) core update-index --config-file $(CONFIG_FILE)
	if ! $(ARDUINO_CLI) core list --config-file $(CONFIG_FILE) | grep -q '$(BOARD)' ; then \
		$(ARDUINO_CLI) core install esp32:esp32 --config-file $(CONFIG_FILE) ; \
	fi
	if ! $(ARDUINO_CLI) lib list | grep -q 'FastLED' ; then \
		$(ARDUINO_CLI) lib install "FastLED" ; \
	fi

build:
	$(ARDUINO_CLI) compile --fqbn $(BOARD) $(PROJECT_NAME) --config-file $(CONFIG_FILE) --build-path $(BUILD_PATH)

upload:
	$(ARDUINO_CLI) upload -p $(PORT) --fqbn $(BOARD) $(PROJECT_NAME) --config-file $(CONFIG_FILE)

core-list:
	$(ARDUINO_CLI) core list

clean:
	rm -rf $(BUILD_PATH)

