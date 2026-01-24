# Project Overview

**Project Name:** UtilityButtonsWithKnobUSB

**Type:** Embedded/IoT - ESP32-C3 Bluetooth HID Device

**Last Updated:** 2026-01-22

## Executive Summary

This project implements a Bluetooth HID rotary encoder input device using an ESP32-C3 microcontroller. The device acts as a wireless USB knob that can function as a mouse scroll wheel, volume control, or zoom control, with an integrated 128x32 OLED display for visual feedback. It features a comprehensive menu system for configuration, power management with deep sleep, and Bluetooth pairing controls. The system leverages FreeRTOS for event-driven architecture and supports multiple encoder modes that can be switched dynamically.

## Purpose

Create a versatile Bluetooth HID input device that provides:

- Rotary encoder input for scroll, volume, or zoom control
- Mode switching between different input behaviors
- Visual feedback via OLED display
- Complete menu system for device configuration
- Wireless Bluetooth connectivity with pairing controls
- Power management with automatic deep sleep
- Configurable button behaviors for media control

## Quick Reference

### Technology Stack

| Category | Technology | Version/Details |
|----------|-----------|----------------|
| **Platform** | ESP32-C3 Super Mini | Espressif32 |
| **Framework** | Arduino | PlatformIO |
| **RTOS** | FreeRTOS | Built-in ESP32 |
| **Bluetooth** | NimBLE | 2.2.3 |
| **Display** | SSD1306 OLED | 128x32, I2C (Adafruit lib 2.5.15) |
| **Encoder** | Rotary Encoder | ai-esp32-rotary-encoder 1.7 |
| **HID** | BLE Keyboard/Mouse | ESP32-BLE-Keyboard |
| **Build System** | PlatformIO | - |

### Architecture Type

**Event-Driven Architecture** with:
- FreeRTOS task-based concurrency
- Queue-based event dispatching (encoder, button, app, menu events)
- Handler pattern for encoder modes
- Static allocation for embedded constraints
- Power management with inactivity monitoring

### Hardware Configuration

**Microcontroller:** ESP32-C3 Super Mini (nologo variant)

**Peripherals:**
- **OLED Display:** SSD1306 128x32 (I2C @ 0x3C)
  - SDA: GPIO 6
  - SCL: GPIO 7
- **Rotary Encoder:**
  - CLK (A): GPIO 1
  - DT (B): GPIO 0
  - Button (SW): GPIO 2
  - Steps: 4 per detent
- **Buttons (4 configurable):**
  - Top Left: GPIO 3
  - Top Right: GPIO 4
  - Bottom Left: GPIO 5
  - Bottom Right: GPIO 9
- **Bluetooth:** Built-in BLE radio
- **Serial:** USB CDC (460800 baud)

### Repository Structure

**Type:** Monolith (single cohesive embedded project)

**Build Environment:** PlatformIO with custom board definition

## Key Features

1. **Multi-Mode Encoder**
   - Scroll Mode: Mouse wheel emulation (vertical scrolling)
   - Volume Mode: Media volume control
   - Zoom Mode: Ctrl+scroll for zoom in/out
   - Mode selection via menu
   - Wheel direction reversal option

2. **Menu System**
   - Wheel Behavior: Mode selection, direction reversal
   - Button Config: Assign actions (None, Mute, Play, Pause, Next, Previous) to each button
   - Bluetooth: Pair new device, disconnect current device
   - Display: Turn display on/off
   - Device Status: Show connection status and settings
   - About: Device information

3. **Bluetooth HID**
   - Device name: "KnobKoKy"
   - Manufacturer: "KoKy"
   - Acts as HID keyboard and mouse
   - Supports media keys and mouse movements
   - Pair and disconnect controls via menu

4. **Power Management**
   - Inactivity detection with configurable timeouts
   - Warning display at 4 minutes idle
   - Automatic deep sleep after 5 minutes
   - Wake via encoder button press
   - Auto-reconnect Bluetooth after wake

5. **Event-Driven Architecture**
   - Four event queues (encoder, button, app, menu)
   - Handler registration system for extensibility
   - Clean separation of concerns

6. **Visual Feedback**
   - 128x32 OLED display
   - Normal mode status screen with icons
   - Bluetooth status icon (solid/flashing/none)
   - Menu navigation display
   - Sleep warning display

## Project Structure

```
UtilityButtonsWithKnobUSB/
├── src/                    # Main application code
│   ├── main.cpp           # Entry point and setup
│   ├── BLE/               # BLE callback handlers
│   ├── Button/            # Button manager
│   ├── Component/         # Interface definitions
│   ├── Config/            # ConfigManager, FactoryReset
│   ├── Display/           # OLED display implementation
│   │   ├── Impl/          # OLEDDisplay, SerialDisplay
│   │   ├── Interface/     # DisplayInterface
│   │   ├── Model/         # DisplayRequest
│   │   └── Task/          # DisplayTask
│   ├── EncoderMode/       # Mode handlers and management
│   │   ├── Handler/       # Scroll, Volume, Zoom handlers
│   │   ├── Interface/     # EncoderModeBaseInterface
│   │   ├── Manager/       # EncoderModeManager
│   │   └── Selector/      # EncoderModeSelector
│   ├── Event/             # Event dispatchers and handlers
│   │   ├── Dispatcher/    # App, Button, Encoder, Menu dispatchers
│   │   └── Handler/       # App, Button, Encoder, Menu handlers
│   ├── Helper/            # Utility functions
│   ├── Menu/              # Menu system
│   │   ├── Action/        # Menu actions (PairAction, DisconnectAction, etc.)
│   │   ├── Controller/    # MenuController
│   │   └── Model/         # MenuItem, MenuTree
│   └── System/            # Power management
├── include/               # Header files
│   ├── Config/           # Device, encoder, button, display, system configs
│   ├── Type/             # Event type definitions
│   ├── Enum/             # Enumerations (WheelMode, ButtonAction, etc.)
│   └── state/            # Hardware state definitions
├── lib/                   # Custom libraries
│   ├── EncoderDriver/    # Rotary encoder driver wrapper
│   └── StatsMonitor/     # Performance monitoring
├── boards/                # Custom board definitions
│   └── nologo_esp32c3_super_mini.json
├── platformio.ini         # PlatformIO configuration
└── _bmad-output/          # Generated documentation
```

## Getting Started

### Prerequisites

- PlatformIO Core or PlatformIO IDE
- ESP32-C3 board with USB-C connection
- Rotary encoder hardware
- SSD1306 OLED display (128x32, I2C)
- 4 tactile buttons (optional)

### Quick Start

1. **Install PlatformIO**
   ```bash
   pip install platformio
   ```

2. **Clone and Build**
   ```bash
   cd UtilityButtonsWithKnobUSB
   ./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble
   ```

3. **Upload to Device**
   ```bash
   ./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble -t upload
   ```

4. **Monitor Serial Output**
   ```bash
   pio device monitor --baud 460800
   ```

### Bluetooth Pairing

1. Power on the device
2. Look for "KnobKoKy" in Bluetooth settings
3. Pair as HID device
4. Rotate encoder to test scroll functionality
5. Long-press encoder button to access menu

### Factory Reset

Hold encoder button for 5+ seconds during boot to reset all settings to defaults.

## Documentation Index

### Generated Documentation

- [Project Overview](./project-overview.md) _(This document)_
- [Project Context](./project-context.md) - AI agent implementation guide
- [Architecture Documentation](./architecture/index.md)
- [Source Tree Analysis](./source-tree-analysis.md)
- [Hardware Documentation](./hardware-documentation/index.md)
- [Development Guide](./development-guide/index.md)

### Existing Documentation

- [PlatformIO Configuration](../platformio.ini) - Build configuration and dependencies
- [Board Definition](../boards/nologo_esp32c3_super_mini.json) - Custom ESP32-C3 board specs

## Project Status

**Current Version:** v1.0 (Feature Complete for Sprint 1)

**Completed Features (Epics 6-10):**
- ✅ Codebase cleanup and consolidation
- ✅ Rotary encoder input (rotation, clicks)
- ✅ Bluetooth HID connectivity
- ✅ OLED display integration (128x32)
- ✅ Event-driven architecture
- ✅ Multiple encoder modes (Scroll, Volume, Zoom)
- ✅ Mode selection system
- ✅ Wheel direction control (normal/reversed)
- ✅ Complete menu system
- ✅ Button configuration (4 buttons with 6 actions each)
- ✅ Bluetooth pairing and disconnect via menu
- ✅ Display power control
- ✅ Power management with deep sleep
- ✅ Inactivity warning display
- ✅ Wake from deep sleep via encoder button
- ✅ Auto-reconnect Bluetooth after wake

**Upcoming (Sprint 2 - Epic 11):**
- 🔨 LED Strip Control (FastLED integration)
- 🔨 LED power toggle
- 🔨 Wheel-controlled brightness
- 🔨 LED mode selection
- 🔨 LED color selection

## Contact & Attribution

**Device Name:** KnobKoKy
**Manufacturer:** KoKy

---

*This documentation was last updated on 2026-01-22*
