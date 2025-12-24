# Building the Project

## Basic Build Commands

### Compile
```bash
pio run
```

**Output:**
- Firmware binary: `.pio/build/<env>/firmware.bin`
- ELF file: `.pio/build/<env>/firmware.elf`
- MAP file: `.pio/build/<env>/firmware.map`

### Clean Build
```bash
pio run --target clean
pio run
```

### Verbose Build
```bash
pio run --verbose
```

## Build Flags

**Current Flags** (from [`platformio.ini`](../platformio.ini:15)):

```ini
build_flags =
    -D ARDUINO_USB_MODE=1          # Enable USB
    -D ARDUINO_USB_CDC_ON_BOOT=1   # USB CDC at boot
```

**Additional Flags** (examples):

```ini
build_flags =
    -D DEBUG_MODE=1                # Enable debug output
    -D LOG_LEVEL=4                 # Set log verbosity
    -D CORE_DEBUG_LEVEL=3          # ESP32 core debug level
```

## Dependency Management

**Library Dependencies** (from [`platformio.ini`](../platformio.ini:19)):

```ini
lib_deps =
    h2zero/NimBLE-Arduino@^2.2.3
    https://github.com/igorantolic/ai-esp32-rotary-encoder@^1.7
    adafruit/Adafruit SSD1306@^2.5.15
    BleKeyboard = https://github.com/ShocKwav3/ESP32-BLE-Keyboard.git
```

**Update Dependencies:**
```bash
pio pkg update
```

**Install Specific Library:**
```bash
pio pkg install --library "library-name@version"
```
