SRC_DIR = src
BUILD_DIR = build
# -fshort-enums: Use as less space as possible for enums
COMPILER_FLAGS = -fshort-enums

O ?= s
port ?= /dev/ttyUSB0
i ?= $(wildcard $(SRC_DIR)/*.c)
# Defines, F_CPU used for util/delay.h
D ?= F_CPU=16000000UL

OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(i))

all: build upload

deps:
	sudo pacman -S avr-libc avrdude avr-binutils avr-gcc

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c force_rebuild
	@mkdir -p build
	avr-gcc -O$(O) $(COMPILER_FLAGS) -D$(D) -mmcu=atmega328p -c $< -o $@

build: $(OBJS)
	avr-gcc -O$(O) $(COMPILER_FLAGS) -D$(D)  -mmcu=atmega328p -o $(BUILD_DIR)/compiled.bin $(OBJS)
	avr-objcopy -O ihex -R .eeprom $(BUILD_DIR)/compiled.bin $(BUILD_DIR)/compiled.hex

upload:
	avrdude -c arduino -p ATMEGA328P -P $(port) -b 115200 -U flash:w:$(BUILD_DIR)/compiled.hex

size: build
	avr-size --format=avr --mcu=atmega328p $(BUILD_DIR)/compiled.bin

asm: build
	@mkdir -p $(BUILD_DIR)
	avr-gcc -S -O$(O) $(COMPILER_FLAGS) -D$(D) -mmcu=atmega328p -o $(BUILD_DIR)/compiled.asm $(SRC_DIR)/main.c

clean:
	rm -fr $(BUILD_DIR)

.PHONY: all force_rebuild clean deps upload size
