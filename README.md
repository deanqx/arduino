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

# AVR

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
screen /dev/ttyUSB0 115200
```
