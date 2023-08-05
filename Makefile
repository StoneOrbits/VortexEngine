.PHONY: all install build upload clean

ARDUINO_CLI = ./bin/arduino-cli
BOARD = adafruit:samd:adafruit_trinket_m0
PORT = /dev/ttyACM0
PROJECT_NAME = VortexEngine/VortexEngine.ino
CONFIG_FILE = $(HOME)/.arduino15/arduino-cli.yaml

# Default target
all: install build

update-index:
	$(ARDUINO_CLI) core update-index

install:
	sudo apt-get update
	sudo apt-get install -y build-essential
	mkdir $(HOME)/.arduino15
	if ! command -v $(ARDUINO_CLI) &> /dev/null ; then \
		curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sudo sh ; \
	fi
	echo 'board_manager: \n  additional_urls: \n    - https://adafruit.github.io/arduino-board-index/package_adafruit_index.json' | sudo tee $(CONFIG_FILE)
	$(ARDUINO_CLI) core update-index --config-file $(CONFIG_FILE)
	if ! $(ARDUINO_CLI) core list --config-file $(CONFIG_FILE) | grep -q '$(BOARD)' ; then \
		$(ARDUINO_CLI) core install adafruit:samd --config-file $(CONFIG_FILE) ; \
	fi

build:
	$(ARDUINO_CLI) compile --fqbn $(BOARD) $(PROJECT_NAME) --config-file $(CONFIG_FILE)

upload:
	$(ARDUINO_CLI) upload -p $(PORT) --fqbn $(BOARD) $(PROJECT_NAME) --config-file $(CONFIG_FILE)

core-list:
	$(ARDUINO_CLI) core list

clean:
	rm -rf $(BUILD_PATH)
