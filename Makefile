.PHONY: all install build upload clean

ARDUINO_CLI = ./bin/arduino-cli --verbose
BOARD = adafruit:samd:adafruit_trinket_m0
PORT = /dev/ttyACM0
PROJECT_NAME = VortexEngine/VortexEngine.ino
BUILD_PATH = build
CONFIG_FILE = $(HOME)/.arduino15/arduino-cli.yaml

# Default target
all: build

update-index:
	$(ARDUINO_CLI) core update-index

install:
	sudo apt-get update
	sudo apt-get install -y build-essential
	mkdir -p $(HOME)/.arduino15
	if ! command -v $(ARDUINO_CLI) &> /dev/null ; then \
		curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sudo sh ; \
	fi
	echo 'board_manager: \n  additional_urls: \n    - https://adafruit.github.io/arduino-board-index/package_adafruit_index.json' | sudo tee $(CONFIG_FILE)
	$(ARDUINO_CLI) core update-index --config-file $(CONFIG_FILE)
	if ! $(ARDUINO_CLI) core list --config-file $(CONFIG_FILE) | grep -q '$(BOARD)' ; then \
		$(ARDUINO_CLI) core install adafruit:samd --config-file $(CONFIG_FILE) ; \
	fi
	wget https://raw.githubusercontent.com/microsoft/uf2/master/utils/uf2conv.py
	wget https://raw.githubusercontent.com/microsoft/uf2/master/utils/uf2families.json
	chmod +x uf2conv.py uf2families.json
	chmod +x rewrite_trinket_source.sh
	./rewrite_trinket_source.sh

build: mod_trinket_source
	$(ARDUINO_CLI) compile --fqbn $(BOARD) $(PROJECT_NAME) --config-file $(CONFIG_FILE) --build-path $(BUILD_PATH)
	python3 uf2conv.py -c -b 0x2000 build/VortexEngine.ino.bin -o build/VortexEngine.ino.uf2

upload:
	$(ARDUINO_CLI) upload -p $(PORT) --fqbn $(BOARD) $(PROJECT_NAME) --config-file $(CONFIG_FILE)

core-list:
	$(ARDUINO_CLI) core list

clean:
	rm -rf $(BUILD_PATH)
