.PHONY: all install build upload clean

ARDUINO_CLI = /usr/local/bin/arduino-cli
BOARD = adafruit:samd:adafruit_trinket_m0
PORT = /dev/ttyACM0
BUILD_PATH = /tmp/arduino-build
PROJECT_NAME = VortexEngine/VortexEngine.ino
CONFIG_FILE = ~/.arduino15/arduino-cli.yaml

# Default target
all: install build

update-index:
	$(ARDUINO_CLI) core update-index

install:
	# Update package list
	sudo apt-get update

	# Install necessary packages
	sudo apt-get install -y build-essential

	# Install Arduino CLI if it doesn't exist
	if ! command -v $(ARDUINO_CLI) &> /dev/null ; then \
		curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sudo sh ; \
	fi

	# Create a configuration file for Arduino CLI
	echo 'board_manager: \n  additional_urls: \n    - https://adafruit.github.io/arduino-board-index/package_adafruit_index.json' | sudo tee $(CONFIG_FILE)

	# Update core libraries
	$(ARDUINO_CLI) core update-index --config-file $(CONFIG_FILE)

	# Install Adafruit's SAMD core if not already installed
	if ! $(ARDUINO_CLI) core list --config-file $(CONFIG_FILE) | grep -q '$(BOARD)' ; then \
		$(ARDUINO_CLI) core install adafruit:samd --config-file $(CONFIG_FILE) ; \
	fi

build:
	$(ARDUINO_CLI) compile --fqbn $(BOARD) $(PROJECT_NAME) --config-file $(CONFIG_FILE) --build-path $(BUILD_PATH)

upload:
	$(ARDUINO_CLI) upload -p $(PORT) --fqbn $(BOARD) $(PROJECT_NAME) --config-file $(CONFIG_FILE)

core-list:
	$(ARDUINO_CLI) core list

clean:
	rm -rf $(BUILD_PATH)
