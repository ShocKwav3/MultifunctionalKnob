# Project Context Analysis

## Requirements Overview

**Functional Requirements:**
42 requirements across 9 functional areas defining a hierarchical menu system for runtime configuration of wheel behaviors and button actions. Core capabilities include:
- Menu navigation via two-input model (wheel rotation + button press)
- 3 wheel modes (Scroll, Volume, Zoom) selectable at runtime
- Dynamic button configuration with predefined behaviors (Mute, Play, Pause, Next, Previous)
- Device status and about screens for troubleshooting
- Configuration persistence across power cycles

**Non-Functional Requirements:**
12 requirements emphasizing:
- **Performance**: Responsive menu navigation with no perceptible lag
- **Reliability**: Zero data loss on persistence, graceful fallback to defaults
- **Maintainability**: Display abstraction for swappable implementations, extensible handler interfaces, documented build process
- **Compatibility**: ESP32-C3 Super Mini as reference hardware

**Scale & Complexity:**
- Primary domain: IoT/Embedded (ESP32-C3 BLE HID device)
- Complexity level: Low
- Estimated architectural components: 6-8 new components integrating with existing event system

## Technical Constraints & Dependencies

**Hardware Constraints:**
- ESP32-C3 RISC-V single-core @ 160 MHz
- 400 KB SRAM, 4 MB Flash
- GPIO limitations (I2C on 6/7, encoder on 0/1/2, remaining pins for buttons)
- USB powered for MVP

**Existing Codebase:**
- Event-driven architecture with FreeRTOS queues
- Handler pattern for encoder modes
- Working BLE HID connectivity via NimBLE
- Mode management system (EncoderModeManager)

**Integration Requirements:**
- Must integrate with existing event dispatchers
- Must extend handler pattern for menu system
- Must not require refactoring of working mode handlers

## Cross-Cutting Concerns Identified

1. **Display Abstraction** - All UI output (menu, status, confirmations) must go through abstract interface that supports serial now, OLED later
2. **Configuration Persistence** - Wheel mode and button behaviors must persist to NVS, with defaults on first boot or corruption
3. **Input Routing** - Short press, long press, and rotation events must route correctly based on application state (menu active vs normal operation)
4. **State Management** - Track current menu level, selected item, and application mode for proper event handling
