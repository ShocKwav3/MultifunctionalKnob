# Project Overview

**Project Name:** UtilityButtonsWithKnobUSB

**Type:** Embedded/IoT - ESP32-C3 Bluetooth HID Device

**Last Updated:** 2025-12-14

## Executive Summary

This project implements a Bluetooth HID rotary encoder input device using an ESP32-C3 microcontroller. The device acts as a wireless USB knob that can function as a mouse scroll wheel or volume control, with an integrated OLED display for visual feedback. It leverages FreeRTOS for event-driven architecture and supports multiple encoder modes that can be switched dynamically.

## Purpose

Create a versatile Bluetooth HID input device that provides:

- Rotary encoder input for scroll or volume control
- Mode switching between different input behaviors
- Visual feedback via OLED display
- Wireless Bluetooth connectivity
- Low-power ESP32-C3 implementation

## Quick Reference

### Technology Stack

| Category | Technology | Version/Details |
|----------|-----------|----------------|
| **Platform** | ESP32-C3 Super Mini | Espressif32 |
| **Framework** | Arduino | PlatformIO |
| **RTOS** | FreeRTOS | Built-in ESP32 |
| **Bluetooth** | NimBLE | 2.2.3 |
| **Display** | SSD1306 OLED | 128x64, I2C (Adafruit lib 2.5.15) |
| **Encoder** | Rotary Encoder | ai-esp32-rotary-encoder 1.7 |
| **HID** | BLE Keyboard/Mouse | ESP32-BLE-Keyboard |
| **Build System** | PlatformIO | - |

### Architecture Type

**Event-Driven Architecture** with:
- FreeRTOS task-based concurrency
- Queue-based event dispatching
- Handler pattern for encoder modes
- Singleton pattern for hardware drivers

### Hardware Configuration

**Microcontroller:** ESP32-C3 Super Mini (nologo variant)

**Peripherals:**
- **OLED Display:** SSD1306 128x64 (I2C @ 0x3C)
  - SDA: GPIO 6
  - SCL: GPIO 7
- **Rotary Encoder:**
  - CLK (A): GPIO 1
  - DT (B): GPIO 0
  - Button (SW): GPIO 2
  - Steps: 4 per detent
- **Bluetooth:** Built-in BLE radio
- **Serial:** USB CDC (460800 baud)

### Repository Structure

**Type:** Monolith (single cohesive embedded project)

**Build Environment:** PlatformIO with custom board definition

## Key Features

1. **Multi-Mode Encoder**
   - Scroll Mode: Mouse wheel emulation (horizontal/vertical)
   - Volume Mode: Media volume control
   - Mode selection via long press

2. **Bluetooth HID**
   - Device name: "KnobKoKy"
   - Manufacturer: "KoKy"
   - Acts as HID keyboard and mouse
   - Supports media keys and mouse movements

3. **Event-Driven Architecture**
   - Two separate event queues (encoder input, application events)
   - Handler registration system for extensibility
   - Clean separation of concerns

4. **Visual Feedback**
   - 128x64 OLED display
   - Mode indication
   - Status messages

## Project Structure

```
UtilityButtonsWithKnobUSB/
├── src/                    # Main application code
│   ├── main.cpp           # Entry point and setup
│   ├── Component/         # Interface definitions
│   ├── EncoderMode/       # Mode handlers and management
│   ├── Event/             # Event dispatchers and handlers
│   ├── Helper/            # Utility functions
│   └── Menu/              # (Future) Menu system
├── include/               # Header files
│   ├── Config/           # Device and encoder configuration
│   ├── Type/             # Event type definitions
│   ├── Enum/             # Enumerations
│   ├── AppState.h        # Global application state
│   └── version.h         # Version information
├── lib/                   # Custom libraries
│   ├── EncoderDriver/    # Rotary encoder driver wrapper
│   └── StatsMonitor/     # Performance monitoring
├── boards/                # Custom board definitions
│   └── nologo_esp32c3_super_mini.json
├── platformio.ini         # PlatformIO configuration
└── docs/                  # Generated documentation
```

## Getting Started

### Prerequisites

- PlatformIO Core or PlatformIO IDE
- ESP32-C3 board with USB-C connection
- Rotary encoder hardware
- SSD1306 OLED display (128x64, I2C)

### Quick Start

1. **Install PlatformIO**
   ```bash
   pip install platformio
   ```

2. **Clone and Build**
   ```bash
   cd UtilityButtonsWithKnobUSB
   pio run
   ```

3. **Upload to Device**
   ```bash
   pio run --target upload
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

## Documentation Index

### Generated Documentation

- [Project Overview](./project-overview.md) _(This document)_
- [Architecture Documentation](./architecture.md) _(To be generated)_
- [Source Tree Analysis](./source-tree-analysis.md)
- [Hardware Documentation](./hardware-documentation.md)
- [Development Guide](./development-guide.md)
- [Component Inventory](./component-inventory.md) _(To be generated)_

### Existing Documentation

- [PlatformIO Configuration](../platformio.ini) - Build configuration and dependencies
- [Board Definition](../boards/nologo_esp32c3_super_mini.json) - Custom ESP32-C3 board specs

## Project Status

**Current Version:** Development

**Functional Features:**
- ✅ Rotary encoder input (rotation, clicks)
- ✅ Bluetooth HID connectivity
- ✅ OLED display integration
- ✅ Event-driven architecture
- ✅ Multiple encoder modes (Scroll, Volume)
- ✅ Mode selection system

**In Development:**
- 🔨 Menu system implementation
- 🔨 Stats monitoring integration
- 🔨 Additional encoder modes

## Contact & Attribution

**Device Name:** KnobKoKy
**Manufacturer:** KoKy

---

*This documentation was generated on 2025-12-14*
