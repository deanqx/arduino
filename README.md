# Creating new sketch

To use the `arduino-language-server` every sketch needs a `sketch.yaml`
containing the FQBN (Fully Qualified Board Name).

```yaml
default_fqbn: arduino:avr:uno
```

# Compile and upload

Adjust the `/dev/ttyUSB0` with your USB port.

```bash
arduino-cli compile . -u -p /dev/ttyUSB0
```
