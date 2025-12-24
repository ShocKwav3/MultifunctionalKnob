# Starter Template Evaluation

## Primary Technology Domain

IoT/Embedded (ESP32-C3 Arduino) - Brownfield project extending existing codebase with established patterns.

## Engineering Standards (Non-Negotiable)

All implementation must adhere to:
- **Proper Architecture** - Clean separation of concerns, single responsibility
- **Appropriate Design Patterns** - Interface abstraction, handler pattern, state machines where applicable
- **Good Coding Style** - Consistent naming conventions, clear intent, self-documenting code
- **SOLID Principles** - Even in embedded C++, these apply
- **Testability by Design** - Code structured to enable unit testing without hardware

These standards are not optional optimizations - they are baseline requirements for all new code.

## Existing Foundation (No Changes Required)

| Component | Current Implementation | Status |
|-----------|----------------------|--------|
| Platform/Build | PlatformIO + Arduino framework | ✅ Keep |
| BLE Stack | NimBLE 2.2.3 | ✅ Keep |
| HID Library | ESP32-BLE-Keyboard (ShocKwav3 fork) | ✅ Keep |
| Display Driver | Adafruit SSD1306 2.5.15 | ✅ Keep |
| Encoder Driver | Custom wrapper over ai-esp32-rotary-encoder 1.7 | ✅ Keep |
| Event Architecture | FreeRTOS queues + dispatcher/handler pattern | ✅ Keep |

## New Components Required

**1. Configuration Persistence: Preferences Library (built-in)**

- Native Arduino-ESP32 library wrapping ESP32 NVS
- No additional dependency required
- Namespace-based key-value storage
- Suitable for infrequent config changes (wheel mode, button behaviors)

**2. Menu System: Custom Implementation**

- Extend existing handler pattern with MenuModeHandler
- Integrate with established event dispatcher architecture
- **Why not ArduinoMenu?** It brings its own input polling model that conflicts with existing FreeRTOS queue architecture. Integration would require either bypassing the event queue (breaking architecture) or complex adapter code.
- Menu state machine manages navigation hierarchy with explicit state documentation

**3. Display Abstraction: Custom Interface Pattern**

- Abstract DisplayInterface base class
- SerialDisplay implementation for MVP (build first)
- OLEDDisplay implementation added only when hardware is ready (YAGNI)
- Menu logic calls interface methods, unaware of concrete display type

## Testability Design

**Strategic Pivot (2025-12-18):** Automated test implementation is **deferred** for the MVP phase to prioritize speed. However, the architecture **must** maintain testability to allow adding tests later.

| Component | Test Strategy (Deferred) |
|-----------|--------------------------|
| Menu Logic | MockDisplay captures UI calls, inject events via queue |
| Display Abstraction | Test concrete implementations independently |
| Persistence | MockPreferences or ESP32 Preferences test partition |
| State Machine | Direct state transition testing with mock inputs |

## Architectural Decisions Established

| Decision Area | Choice |
|---------------|--------|
| Build Tooling | PlatformIO (existing) |
| Language | C++ with Arduino framework (existing) |
| Runtime | FreeRTOS tasks and queues (existing) |
| Persistence | ESP32 Preferences/NVS (new - built-in) |
| Menu Pattern | Custom handler extending existing architecture (new) |
| Display Pattern | Interface abstraction with pluggable implementations (new) |

## Documentation Commitment

The Architecture Decision Document will include:
- Menu System Design section with state machine diagram/table
- Event-to-action mapping for menu navigation
- Integration points with existing handler registration

**Note:** No external starter template required. New components integrate with established patterns while maintaining engineering standards.
