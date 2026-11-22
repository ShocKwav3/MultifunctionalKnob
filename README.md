# MultifunctionalKnob

A configurable Bluetooth knob for a computer, built on the ESP32-C3. It pairs as a BLE keyboard and mouse with the name `KnobKoKy`. A rotary encoder and five buttons control scroll, volume, zoom, media keys and key combinations. You change every setting on the device, through its menu and OLED, and each setting stays after a power cycle.

## Features

- **Wheel:** Scroll, Volume or Zoom mode, with normal or reversed direction.
- **Buttons:** four buttons (Top Left, Top Right, Bottom Left, Bottom Right), each with a media key that you select in the menu.
- **Macro mode:** a long press on the macro button turns macro mode on or off. In macro mode, inputs with a stored key combination send it. There is no menu to edit macros yet.
- **Menu:** long-press the wheel button to open it. Rotate to move, press to select, long-press to go back.
- **Bluetooth:** pair and disconnect from the menu. The device reconnects to the last host.
- **Display:** a 128x32 OLED shows the Bluetooth state, the wheel mode and the direction.
- **Power:** deep sleep after 5 min of inactivity, with a warning at 4 min. The wheel button wakes the device.
- **Factory reset:** hold the wheel button for 5 s or more during power-on.

## Usage

1. Power the device through USB-C.
2. On the host, open the Bluetooth settings and pair with `KnobKoKy`.
3. Rotate the wheel. The default mode is Scroll.
4. To change a setting, long-press the wheel button, select the item, and press the wheel button.

## Hardware

ESP32-C3 Super Mini, an EC11 rotary encoder with push button, an SSD1306 128x32 I2C OLED, and five push buttons. The pin map and the wiring are in [docs/hardware.md](docs/hardware.md).

## Development

```bash
pio run -e use_nimble
pio run -e use_nimble -t upload
pio device monitor
```

The serial monitor uses 460800 baud. The build environments, debugging and common tasks are in [docs/development.md](docs/development.md). The firmware design is in [docs/architecture.md](docs/architecture.md).

## Stack

- PlatformIO, Arduino framework on ESP32-C3, FreeRTOS
- NimBLE-Arduino 2, ESP32-BLE-Keyboard (ShocKwav3 fork)
- Adafruit SSD1306 2.5
- ai-esp32-rotary-encoder 1.7

## Layout

```text
src/       Application code: BLE, Config, Display, EncoderMode, Event, Macro, Menu, System
include/   Configuration constants, enums, event types, shared state
lib/       Encoder and button drivers
boards/    Board definition for the ESP32-C3 Super Mini
docs/      Hardware, development and architecture documentation
```

**License:** not decided yet.
