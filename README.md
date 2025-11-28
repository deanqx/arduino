Experiments and homework with the Arduino R3 (ATmega328) and other AVR Microcontrollers.

# Arduino IDE

## Creating new sketch

To use the `arduino-language-server` every sketch needs a `sketch.yaml`
containing the FQBN (Fully Qualified Board Name).

```yaml
default_fqbn: arduino:avr:uno
```

## Compile and upload

Adjust the `/dev/ttyUSB0` with your USB port.

```bash
arduino-cli compile . -u -p /dev/ttyUSB0
```

# AVR with CMake

## Prepare

```
cmake -B build
```

## Compile and upload

```
cmake --build build
```

# AVR with make

## Install required packages

### Arch Linux

```
make deps
```

## Compile and upload

```
make
```

Warning: You have to run the "clean command" after modifying the `Makefile`:

```
make clean
```

## Optional: Setup Language Server

Generate `compile_commands.json` for clangd

```
make clean
bear -- make build
```

# Read USB serial output

Warning: You can't upload and read UART at the same time.

```
minicom -D /dev/ttyUSB0 -b 19200
```
