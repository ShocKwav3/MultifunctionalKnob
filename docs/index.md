# Project Documentation Index

**Project:** UtilityButtonsWithKnobUSB
**Type:** Embedded/IoT - ESP32-C3 Bluetooth HID Device
**Last Generated:** 2025-12-14
**Documentation Version:** 1.0.0

---

## Quick Reference

### Project Summary

| Attribute | Value |
|-----------|-------|
| **Platform** | ESP32-C3 Super Mini |
| **Framework** | Arduino (PlatformIO) |
| **Primary Language** | C++ |
| **Architecture** | Event-Driven (FreeRTOS) |
| **Purpose** | Bluetooth HID Rotary Encoder |
| **Repository Type** | Monolith |

### Key Technologies

- **MCU:** ESP32-C3 RISC-V @ 160 MHz
- **Bluetooth:** BLE 5.0 (NimBLE stack)
- **Display:** SSD1306 OLED 128x64 (I2C)
- **Input:** Rotary encoder with button
- **RTOS:** FreeRTOS (built-in)

### Hardware Configuration

| Component | GPIO | Interface |
|-----------|------|-----------|
| Encoder CLK | 1 | Input (Pull-up) |
| Encoder DT | 0 | Input (Pull-up) |
| Encoder Button | 2 | Input (Pull-up) |
| OLED SDA | 6 | I2C |
| OLED SCL | 7 | I2C |

---

## Documentation Structure

### 📖 Core Documentation

#### 1. [Project Overview](./project-overview.md)
**Purpose:** High-level project introduction and quick start
**Contains:**
- Executive summary
- Technology stack
- Key features
- Project structure overview
- Getting started guide

**When to use:** First-time project orientation, onboarding new developers

---

#### 2. [Source Tree Analysis](./source-tree-analysis.md)
**Purpose:** Complete directory structure with annotations
**Contains:**
- Annotated file tree
- Critical directories explained
- Entry points and initialization flow
- Integration points
- FreeRTOS task organization
- Build outputs

**When to use:** Understanding project organization, locating specific components, navigation reference

---

#### 3. [Architecture Documentation](./architecture/index.md)
**Purpose:** Software architecture, design patterns, and system design
**Contains:** _(To be generated)_
- System architecture overview
- Design patterns used
- Event-driven architecture details
- Component dependencies
- Data flow diagrams
- State management
- Concurrency model (FreeRTOS tasks/queues)

**When to use:** Understanding system design, making architectural decisions, refactoring

---

#### 4. [Hardware Documentation](./hardware-documentation/index.md)
**Purpose:** Hardware specifications, wiring, and physical setup
**Contains:**
- ESP32-C3 specifications
- Pin configuration and GPIO assignments
- Peripheral hardware (encoder, OLED, BLE)
- Power requirements
- Wiring diagrams
- Bill of Materials (BOM)
- Hardware testing procedures
- Troubleshooting hardware issues

**When to use:** Hardware setup, debugging hardware issues, modifying hardware configuration

---

#### 5. [Development Guide](./development-guide/index.md)
**Purpose:** Developer setup, build instructions, and workflow
**Contains:**
- Environment setup (PlatformIO installation)
- Build and upload procedures
- Testing and debugging
- Development workflow
- Code organization best practices
- Common development tasks
- Adding new features
- Performance optimization
- Troubleshooting development issues

**When to use:** Setting up development environment, daily development tasks, debugging

---

### 📦 Component Documentation

#### 6. [Component Inventory](./component-inventory.md) _(To be generated)_
**Purpose:** Catalog of all software components and their responsibilities
**Contains:**
- Event system components
- Encoder mode handlers
- Dispatchers and event handlers
- Helper utilities
- Custom libraries
- Component relationships

**When to use:** Understanding component responsibilities, finding specific functionality

---

### 🔧 Operational Documentation

#### 7. [Build and Deployment Guide](./build-deployment-guide.md) _(To be generated)_
**Purpose:** CI/CD, release process, and deployment instructions
**Contains:**
- Build environments
- Release versioning
- Firmware update process
- Build optimization
- Distribution methods

**When to use:** Creating releases, deploying firmware updates

---

## Getting Started Paths

### For New Developers

**Recommended Reading Order:**

1. **[Project Overview](./project-overview.md)** - Start here for project introduction
2. **[Hardware Documentation](./hardware-documentation/index.md)** - Understand the hardware
3. **[Development Guide](./development-guide/index.md)** - Set up your environment
4. **[Source Tree Analysis](./source-tree-analysis.md)** - Navigate the codebase
5. **[Architecture Documentation](./architecture/index.md)** - Understand the design _(To be generated)_

### For Quick Hardware Setup

1. **[Hardware Documentation](./hardware-documentation/build-and-assembly.md)** → Wiring Instructions
2. **[Development Guide](./development-guide/uploading-firmware.md)** → Upload Firmware
3. **[Hardware Documentation](./hardware-documentation/troubleshooting.md)** → Testing Hardware

### For Feature Development

1. **[Architecture Documentation](./architecture/index.md)** → Understand current design _(To be generated)_
2. **[Component Inventory](./component-inventory.md)** → Find relevant components _(To be generated)_
3. **[Development Guide](./development-guide/common-development-tasks.md)** → Adding New Features
4. **[Source Tree Analysis](./source-tree-analysis.md)** → Locate files

### For Troubleshooting

1. **[Development Guide](./development-guide/troubleshooting-development-issues.md)** → Troubleshooting sections
2. **[Hardware Documentation](./hardware-documentation/troubleshooting.md)** → Hardware issues
3. **[Architecture Documentation](./architecture/index.md)** → System behavior _(To be generated)_

---

## System Architecture Quick Reference

### Event Flow

```
Hardware Input
    ↓
EncoderDriver (ISR/Task)
    ↓
EncoderEventDispatcher
    ↓
EncoderInputEvent Queue
    ↓
EncoderEventHandler (FreeRTOS Task)
    ↓
Current Mode Handler
    ↓
BleKeyboard HID Output
```

### Key Components

| Component | Location | Purpose |
|-----------|----------|---------|
| **main.cpp** | `src/main.cpp` | Entry point, initialization |
| **EncoderDriver** | `lib/EncoderDriver/` | Hardware abstraction for encoder |
| **Event System** | `src/Event/` | Dispatchers and handlers |
| **Mode System** | `src/EncoderMode/` | Mode handlers and management |
| **Config** | `include/Config/` | Hardware and device configuration |
| **Types** | `include/Type/` | Event data structures |

### Current Encoder Modes

1. **Scroll Mode** - Mouse wheel emulation (horizontal/vertical)
2. **Volume Mode** - Media volume control
3. **Selection Mode** - Interactive mode picker (long press to activate)

---

## Build Quick Reference

### Essential Commands

```bash
# Build firmware
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run

# Upload to device
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run --target upload

# Open serial monitor
pio device monitor --baud 460800

# All-in-one (build + upload + monitor)
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t upload && pio device monitor

# Clean build
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t clean && ./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run
```

### Configuration Files

| File | Purpose |
|------|---------|
| [`platformio.ini`](../platformio.ini) | Build configuration, dependencies, environments |
| [`include/Config/device_config.h`](../include/Config/device_config.h) | Bluetooth settings, button timings |
| [`include/Config/encoder_config.h`](../include/Config/encoder_config.h) | GPIO pins, encoder parameters |
| [`boards/nologo_esp32c3_super_mini.json`](../boards/nologo_esp32c3_super_mini.json) | Custom board definition |

---

## External Resources

### Datasheets

- [ESP32-C3 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf)
- [ESP32-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [SSD1306 OLED Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

### Library Documentation

- [PlatformIO Docs](https://docs.platformio.org/)
- [ESP32 Arduino Core](https://docs.espressif.com/projects/arduino-esp32/)
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino)
- [ESP32-BLE-Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard)
- [ai-esp32-rotary-encoder](https://github.com/igorantolic/ai-esp32-rotary-encoder)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)

### Community Resources

- [PlatformIO Community](https://community.platformio.org/)
- [ESP32 Forum](https://esp32.com/)
- [Arduino Forum](https://forum.arduino.cc/)

---

## Project Files Reference

### Source Code Files

```
src/
├── main.cpp                                    # Entry point
├── Component/Interface/
│   └── EncoderInputHandlerInterface.h        # Handler interface
├── EncoderMode/
│   ├── Handler/
│   │   ├── EncoderModeHandlerAbstract.*      # Base handler
│   │   ├── EncoderModeHandlerInterface.h     # Handler contract
│   │   ├── EncoderModeHandlerScroll.*        # Scroll mode
│   │   └── EncoderModeHandlerVolume.*        # Volume mode
│   ├── Interface/
│   │   └── EncoderModeBaseInterface.h        # Mode interface
│   ├── Manager/
│   │   └── EncoderModeManager.*              # Mode management
│   └── Selector/
│       └── EncoderModeSelector.*             # Mode selection
├── Event/
│   ├── Dispatcher/
│   │   ├── AppEventDispatcher.*              # App events
│   │   └── EncoderEventDispatcher.*          # Input events
│   └── Handler/
│       ├── AppEventHandler.*                 # App event processing
│       └── EncoderEventHandler.*             # Input event processing
└── Helper/
    └── EncoderModeHelper.*                   # Utilities
```

### Header Files

```
include/
├── AppState.h                                # Global state
├── version.h                                 # Version info
├── Config/
│   ├── device_config.h                      # BLE config
│   └── encoder_config.h                     # Pin config
├── Enum/
│   └── EventEnum.h                          # Event types
└── Type/
    ├── AppEvent.h                           # App event struct
    └── EncoderInputEvent.h                  # Input event struct
```

### Custom Libraries

```
lib/
├── EncoderDriver/
│   ├── EncoderDriver.h                      # Driver interface
│   └── EncoderDriver.cpp                    # Implementation
└── StatsMonitor/
    └── StatsMonitor.h                       # Performance monitoring
```

---

## Documentation Maintenance

### Updating Documentation

**When to Update:**
- After adding new features
- When changing hardware configuration
- After architectural refactoring
- When updating dependencies

**How to Update:**
1. Modify relevant markdown files in `docs/`
2. Update version and date in file headers
3. Update this index if new documents added
4. Regenerate with `*document-project` if major changes

### Documentation Standards

This documentation follows [BMAD Documentation Standards](../.bmad/bmm/data/documentation-standards.md):
- CommonMark compliant
- Task-oriented writing
- Active voice, present tense
- Clear headings and structure
- No time estimates (per BMAD standards)

---

## Quick Links

### Most Frequently Accessed

- [Hardware Setup](./hardware-documentation/build-and-assembly.md)
- [Upload Firmware](./development-guide/uploading-firmware.md)
- [Troubleshooting](./development-guide/troubleshooting-development-issues.md)
- [Add New Mode](./development-guide/common-development-tasks.md)
- [GPIO Configuration](./hardware-documentation/pin-configuration.md)

### Configuration References

- [Bluetooth Settings](./hardware-documentation/peripheral-hardware.md)
- [Button Timings](./hardware-documentation/pin-configuration.md)
- [Display Settings](./hardware-documentation/peripheral-hardware.md)
- [Build Environments](./development-guide/building-the-project.md)

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0.0 | 2025-12-14 | Initial documentation generation (deep scan) |

---

**Note:** This documentation was automatically generated by the BMAD Document Project workflow. For questions or issues with the documentation, refer to the development guide or project maintainers.

---

*Documentation index generated on 2025-12-14. This is the primary entry point for AI-assisted development and project navigation.*