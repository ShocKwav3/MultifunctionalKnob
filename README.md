# UtilityButtonsWithKnobUSB

A configurable Bluetooth HID rotary encoder device built on ESP32-C3. Transform a simple knob into a versatile desktop peripheral with runtime-configurable wheel behaviors and button actions.

## Introduction

This project implements a wireless Bluetooth HID input device using an ESP32-C3 microcontroller with a rotary encoder and OLED display. Unlike typical single-purpose knobs, this device features a built-in menu system that lets you configure wheel behavior (scroll/volume/zoom) and button actions without reflashing firmware.

**Device Name:** KnobKoKy
**Target Users:** Makers, developers, and enthusiasts who want customizable desktop peripherals

## Features

- **Multi-Mode Wheel**: Switch between scroll, volume, and zoom modes
- **Runtime Configuration**: Configure behaviors through the device menu system
- **Bluetooth HID**: Acts as wireless keyboard and mouse for broad compatibility
- **Visual Feedback**: 128x64 OLED display shows current mode and menu options
- **Persistent Settings**: Configuration saved to non-volatile storage (NVS)
- **Event-Driven Architecture**: FreeRTOS-based queue system for responsive input handling

See the [Functional Requirements](./_bmad-output/prd/functional-requirements.md) for complete feature details.

## Usage

### First Time Setup

1. Power on the device via USB-C
2. Look for "KnobKoKy" in your device's Bluetooth settings
3. Pair as an HID device (keyboard/mouse)
4. The device is ready to use

### Basic Operation

- **Rotate**: Performs the active wheel behavior (default: scroll)
- **Short Press**: Select menu item or confirm action
- **Long Press**: Enter menu or return to previous menu level

### Changing Wheel Mode

1. Long-press the encoder button to enter the menu
2. Rotate to navigate to "Wheel Behavior"
3. Short-press to enter, then select your desired mode (Scroll/Volume/Zoom)
4. Device automatically exits and applies the new mode

See the [Development Guide](./_bmad-output/development-guide/index.md) for detailed usage instructions.

## Hardware

### Components

- **ESP32-C3 Super Mini** (RISC-V @ 160 MHz, 400 KB SRAM, 4 MB Flash)
- **Rotary Encoder** (quadrature with integrated button)
- **SSD1306 OLED Display** (128x64, I2C @ 0x3C)

### Pin Configuration

| GPIO | Function | Component |
|------|----------|-----------|
| 0 | Encoder DT | Rotary Encoder Channel B |
| 1 | Encoder CLK | Rotary Encoder Channel A |
| 2 | Encoder SW | Encoder Button |
| 6 | I2C SDA | OLED Display |
| 7 | I2C SCL | OLED Display |

See [Hardware Documentation](./_bmad-output/hardware-documentation/hardware-overview.md) for complete specifications and assembly instructions.

## Development

### Prerequisites

- [PlatformIO](https://platformio.org/) Core or IDE
- USB-C cable for programming and power
- Hardware components assembled per pin configuration

### Quick Start

```bash
# Clone the repository
git clone <repository-url>
cd UtilityButtonsWithKnobUSB

# Build the project (using pio-wrapper for cleaner output)
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble

# Upload firmware
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble -t upload

# Monitor serial output
pio device monitor --baud 460800
```

### Project Structure

```
├── src/                    # Application source code
│   ├── main.cpp           # Entry point
│   ├── EncoderMode/       # Wheel behavior handlers
│   ├── Event/             # Event dispatchers and handlers
│   ├── Menu/              # Menu system implementation
│   └── Display/           # Display interface and implementations
├── include/               # Headers and configuration
│   ├── Config/           # Device configuration constants
│   ├── Enum/             # Type-safe enumerations
│   └── Type/             # Event type definitions
├── lib/                   # Custom libraries
└── _bmad-output/          # Generated documentation
```

### Build Environments

- `use_nimble` - Preferred build (uses NimBLE stack for smaller footprint)
- `use_nimble_debug` - Debug build with verbose logging

### Key Technologies

- **Framework**: Arduino on ESP32-C3
- **RTOS**: FreeRTOS (built-in)
- **Bluetooth**: NimBLE 2.2.3
- **Display**: Adafruit SSD1306 2.5.15
- **Encoder**: ai-esp32-rotary-encoder 1.7
- **HID**: ESP32-BLE-Keyboard (ShocKwav3 fork)

### Development Resources

- [Development Environment Setup](./_bmad-output/development-guide/development-environment-setup.md)
- [Building the Project](./_bmad-output/development-guide/building-the-project.md)
- [Testing and Debugging](./_bmad-output/development-guide/testing-and-debugging.md)
- [Architecture Documentation](./_bmad-output/architecture/index.md)
- [Project Context](./_bmad-output/project-context.md) - AI agent development rules

### Contributing

This project is designed to be open-sourced and community-extensible. When contributing:

- Follow the established event-driven architecture patterns
- Read [project-context.md](./_bmad-output/project-context.md) for coding standards
- Test on hardware before submitting changes
- Update documentation for new features

---

**License:** _[To be determined]_
**Version:** Development
**Last Updated:** 2026-01-01
