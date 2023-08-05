.PHONY: install build upload all

# Default target
all: install build

install:
	# Update package list
	sudo apt-get update

	# Install necessary packages
	sudo apt-get install -y build-essential

	# Install Arduino CLI if it doesn't exist
	if ! command -v arduino-cli &> /dev/null ; then \
		curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sudo sh ; \
	fi

	# Create a configuration file for Arduino CLI
	echo 'board_manager: \n  additional_urls: \n    - https://adafruit.github.io/arduino-board-index/package_adafruit_index.json' | sudo tee ~/.arduino15/arduino-cli.yaml

	# Update core libraries
	sudo arduino-cli core update-index --config-file ~/.arduino15/arduino-cli.yaml

	# Install Adafruit's SAMD core if not already installed
	if ! sudo arduino-cli core list --config-file ~/.arduino15/arduino-cli.yaml | grep -q 'adafruit:samd' ; then \
		sudo arduino-cli core install adafruit:samd --config-file ~/.arduino15/arduino-cli.yaml ; \
	fi

build:
	# Compile the project
	sudo arduino-cli compile \
		--fqbn adafruit:samd:adafruit_trinket_m0 VortexEngine/VortexEngine.ino \
		--config-file ~/.arduino15/arduino-cli.yaml

upload:
	# Upload the compiled code to the Arduino board
	# Replace /dev/ttyACM0 with your actual Arduino port
	sudo arduino-cli upload \
		-p /dev/ttyACM0 \
		--fqbn adafruit:samd:adafruit_trinket_m0 VortexEngine/VortexEngine.ino \
		--config-file ~/.arduino15/arduino-cli.yaml

