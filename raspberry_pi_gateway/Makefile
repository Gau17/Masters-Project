# Makefile for Raspberry Pi Gateway

# Variables
RUSTC=rustc
TARGET=armv7-unknown-linux-gnueabihf # Example target, adjust as needed
SRC_DIR=src
BUILD_DIR=target
APP_NAME=gateway_app

# Default target
all: build

build:
	@echo "Building Raspberry Pi Gateway with rustc..."
	@mkdir -p $(BUILD_DIR)
	$(RUSTC) --target=$(TARGET) $(SRC_DIR)/main.rs -o $(BUILD_DIR)/$(APP_NAME)

run: build
	@echo "Running Raspberry Pi Gateway..."
	# Add command to run your application, possibly on the RPi via SSH
	# Example: ssh user@raspberrypi_ip ./$(BUILD_DIR)/$(APP_NAME)

deploy: build
	@echo "Deploying to Raspberry Pi..."
	# Add scp or rsync commands to deploy to RPi
	# Example: scp $(BUILD_DIR)/$(APP_NAME) user@raspberrypi_ip:~/path/to/deploy

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)

test:
	@echo "Running tests... (define your test commands)"
	# Add commands to compile and run your Rust tests using rustc
	# Example: $(RUSTC) --test $(SRC_DIR)/main.rs && ./main # (this will create 'main' executable for tests)

.PHONY: all build run deploy clean test
