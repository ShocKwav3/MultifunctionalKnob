---
stepsCompleted: [1, 2, 3, 4, 5, 6, 7, 8]
inputDocuments:
  - docs/prd.md
  - docs/index.md
  - docs/project-overview.md
  - docs/hardware-documentation.md
  - docs/development-guide.md
  - docs/source-tree-analysis.md
workflowType: 'architecture'
lastStep: 8
status: 'complete'
completedAt: '2025-12-16'
project_name: 'UtilityButtonsWithKnobUSB'
user_name: 'Feroj'
date: '2025-12-15'
hasProjectContext: false
---

# Architecture Decision Document

_This document builds collaboratively through step-by-step discovery. Sections are appended as we work through each architectural decision together._

## Project Context Analysis

### Requirements Overview

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

### Technical Constraints & Dependencies

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

### Cross-Cutting Concerns Identified

1. **Display Abstraction** - All UI output (menu, status, confirmations) must go through abstract interface that supports serial now, OLED later
2. **Configuration Persistence** - Wheel mode and button behaviors must persist to NVS, with defaults on first boot or corruption
3. **Input Routing** - Short press, long press, and rotation events must route correctly based on application state (menu active vs normal operation)
4. **State Management** - Track current menu level, selected item, and application mode for proper event handling

## Starter Template Evaluation

### Primary Technology Domain

IoT/Embedded (ESP32-C3 Arduino) - Brownfield project extending existing codebase with established patterns.

### Engineering Standards (Non-Negotiable)

All implementation must adhere to:
- **Proper Architecture** - Clean separation of concerns, single responsibility
- **Appropriate Design Patterns** - Interface abstraction, handler pattern, state machines where applicable
- **Good Coding Style** - Consistent naming conventions, clear intent, self-documenting code
- **SOLID Principles** - Even in embedded C++, these apply
- **Testability by Design** - Code structured to enable unit testing without hardware

These standards are not optional optimizations - they are baseline requirements for all new code.

### Existing Foundation (No Changes Required)

| Component | Current Implementation | Status |
|-----------|----------------------|--------|
| Platform/Build | PlatformIO + Arduino framework | ✅ Keep |
| BLE Stack | NimBLE 2.2.3 | ✅ Keep |
| HID Library | ESP32-BLE-Keyboard (ShocKwav3 fork) | ✅ Keep |
| Display Driver | Adafruit SSD1306 2.5.15 | ✅ Keep |
| Encoder Driver | Custom wrapper over ai-esp32-rotary-encoder 1.7 | ✅ Keep |
| Event Architecture | FreeRTOS queues + dispatcher/handler pattern | ✅ Keep |

### New Components Required

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

### Testability Design

**Strategic Pivot (2025-12-18):** Automated test implementation is **deferred** for the MVP phase to prioritize speed. However, the architecture **must** maintain testability to allow adding tests later.

| Component | Test Strategy (Deferred) |
|-----------|--------------------------|
| Menu Logic | MockDisplay captures UI calls, inject events via queue |
| Display Abstraction | Test concrete implementations independently |
| Persistence | MockPreferences or ESP32 Preferences test partition |
| State Machine | Direct state transition testing with mock inputs |

### Architectural Decisions Established

| Decision Area | Choice |
|---------------|--------|
| Build Tooling | PlatformIO (existing) |
| Language | C++ with Arduino framework (existing) |
| Runtime | FreeRTOS tasks and queues (existing) |
| Persistence | ESP32 Preferences/NVS (new - built-in) |
| Menu Pattern | Custom handler extending existing architecture (new) |
| Display Pattern | Interface abstraction with pluggable implementations (new) |

### Documentation Commitment

The Architecture Decision Document will include:
- Menu System Design section with state machine diagram/table
- Event-to-action mapping for menu navigation
- Integration points with existing handler registration

**Note:** No external starter template required. New components integrate with established patterns while maintaining engineering standards.

## Core Architectural Decisions

### Decision Priority Analysis

**Critical Decisions (Block Implementation):**
- NVS persistence structure
- Menu state machine pattern
- Event routing for menu activation
- Display update mechanism

**Important Decisions (Shape Architecture):**
- Action execution pattern
- Button configuration structure
- Default values strategy

**Deferred Decisions (Post-MVP):**
- OTA update mechanism
- Battery power management
- Multi-profile support

### Data Architecture

| Decision | Choice | Rationale |
|----------|--------|-----------|
| NVS Namespace | Single namespace with prefixed keys | Organized without namespace-switching overhead |
| Default Values | Defaults struct with factory reset | Minimize NVS writes, explicit reset capability |
| Enum Storage | uint8_t with validation wrapper | Compact storage with graceful fallback on corruption |

**NVS Key Structure:**
```
Namespace: "knobkoky"
Keys: "wheel.mode", "btn0.action", "btn1.action", "btn2.action", "btn3.action"
```

**Validation Pattern:**
```cpp
template<typename EnumT>
EnumT readEnumConfig(const char* key, EnumT defaultVal, EnumT maxValid) {
    uint8_t stored = preferences.getUChar(key, static_cast<uint8_t>(defaultVal));
    return (stored <= static_cast<uint8_t>(maxValid))
           ? static_cast<EnumT>(stored)
           : defaultVal;
}
```

### Component Architecture (Menu System)

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Menu State Machine | Static tree structure with parent pointers | No dynamic allocation, compile-time menu definition |
| Mode Transition | Event interceptor pattern | Menu intercepts events at higher level, clean separation from mode system |
| Action Execution | Command pattern with action objects | Flexible, extensible, clean separation of concerns |

**Menu Tree Structure:**
```cpp
struct MenuItem {
    const char* label;
    MenuItem* parent;
    MenuItem* const* children;
    uint8_t childCount;
    MenuAction* action;
};
```

**Event Interceptor Pattern:**
```cpp
void handleEncoderEvent(EncoderInputEvent& event) {
    if (menuController.isActive()) {
        menuController.handleEvent(event);
        return;  // Menu consumes event
    }
    currentModeHandler->handleEvent(event);
}
```

**Command Pattern for Actions:**
```cpp
class MenuAction {
public:
    virtual ~MenuAction() = default;
    virtual void execute() = 0;
    virtual const char* getConfirmationMessage() const { return nullptr; }
};

class SelectWheelModeAction : public MenuAction {
    WheelMode mode;
public:
    SelectWheelModeAction(WheelMode m) : mode(m) {}
    void execute() override { /* save to NVS, update mode manager */ }
    const char* getConfirmationMessage() const override { return "Mode saved"; }
};
```

### Communication Patterns

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Event Architecture | Distributed Pipelines (Option B) | Decouples domains (Menu, System), adheres to SRP |
| Display Access | Arbitration via DisplayTask | Prevents race conditions on shared display resource |
| Menu Events | Dedicated MenuEventQueue | Separates UI logic from system state |
| Button Config | Array of pin definitions with metadata | Easy to iterate, adding button = one line change |

**Event Pipelines:**
1. **Menu Pipeline:** `MenuController` → `MenuEventDispatcher` → `MenuEventQueue` → `MenuEventHandler`
2. **System Pipeline:** `SystemComponents` → `SystemEventDispatcher` → `SystemEventQueue` → `SystemEventHandler`
3. **Display Pipeline:** `*Handler` → `DisplayRequestQueue` → `DisplayTask` (Arbitrator)

**Display Arbitration Pattern:**
```cpp
// All handlers must use this struct to request drawing
struct DisplayRequest {
    enum Type { DRAW_MENU, SHOW_STATUS, CLEAR } type;
    union {
        struct { const char* title; const char** items; uint8_t count; uint8_t selected; } menu;
        struct { const char* key; const char* value; } status;
    } data;
};
// Only DisplayTask consumes this queue and touches hardware
```

**Button Configuration:**
```cpp
struct ButtonConfig {
    uint8_t pin;
    const char* label;
    bool activeLow;
};

constexpr ButtonConfig BUTTONS[] = {
    {3, "Button 1", true},
    {4, "Button 2", true},
    {5, "Button 3", true},
    {8, "Button 4", true}
};
constexpr size_t BUTTON_COUNT = sizeof(BUTTONS) / sizeof(BUTTONS[0]);
```

### Decision Impact Analysis

**Implementation Sequence:**
1. Display abstraction (DisplayInterface + SerialDisplay)
2. Display Arbitration (DisplayRequestQueue + DisplayTask)
3. NVS persistence wrapper (ConfigManager)
4. Button configuration and GPIO setup
5. Menu data structures (MenuItem tree)
6. MenuAction classes
7. MenuController with event interception
8. MenuEventHandler and SystemEventHandler implementation
9. Integration with existing EncoderEventHandler

**Cross-Component Dependencies:**
- MenuController depends on MenuEventDispatcher
- MenuEventHandler depends on DisplayRequestQueue
- DisplayTask depends on DisplayInterface

## Implementation Patterns & Consistency Rules

### Critical Conflict Points Addressed

8 potential conflict areas identified and standardized for AI agent consistency.

### Naming Patterns

| Element | Convention | Example |
|---------|------------|---------|
| Files | PascalCase matching class | `MenuController.cpp` |
| Classes/Structs | PascalCase | `MenuController`, `MenuItem` |
| Functions | camelCase | `handleEvent()`, `saveConfig()` |
| Variables | camelCase | `currentItem`, `menuActive` |
| Constants | UPPER_SNAKE_CASE | `MAX_MENU_DEPTH`, `BUTTON_COUNT` |
| Enums | PascalCase enum, UPPER_SNAKE values | `enum class Error { OK, NVS_WRITE_FAIL }` |
| Interfaces | Suffix with Interface | `DisplayInterface` |
| Macros | UPPER_SNAKE_CASE | `LOG_DEBUG`, `LOG_ERROR` |

### Structure Patterns

**Directory Organization:**
```
src/
├── Menu/
│   ├── Controller/
│   │   └── MenuController.cpp/.h
│   ├── Model/
│   │   └── MenuItem.h
│   └── Action/
│       ├── MenuAction.h
│       ├── SelectWheelModeAction.cpp/.h
│       └── SetButtonActionAction.cpp/.h
├── Display/
│   ├── Interface/
│   │   └── DisplayInterface.h
│   ├── Task/
│   │   └── DisplayTask.cpp/.h           # NEW: Arbitration Task
│   ├── Model/
│   │   └── DisplayRequest.h             # NEW: Request struct
│   └── Impl/
│       ├── SerialDisplay.cpp/.h
│       └── OLEDDisplay.cpp/.h
├── Config/
│   └── ConfigManager.cpp/.h
├── Button/
│   └── ButtonManager.cpp/.h
├── System/                              # NEW: System domain
│   └── Handler/
│       └── SystemEventHandler.cpp/.h
└── (existing directories unchanged)
```

**Header Guards:**
All headers use `#pragma once`

### Format Patterns

**Error Return Pattern:**
```cpp
enum class Error : uint8_t {
    OK = 0,
    INVALID_PARAM,
    NVS_READ_FAIL,
    NVS_WRITE_FAIL,
    INVALID_STATE
};

// All fallible operations return Error
Error ConfigManager::saveWheelMode(WheelMode mode);
```

### Communication Patterns

**Logging Pattern:**
```cpp
LOG_ERROR("Tag", "Format %d", value);  // [ERR][Tag] Format 123
LOG_INFO("Tag", "Format %s", str);     // [Tag] Format string
LOG_DEBUG("Tag", "Format %d", val);    // [DBG][Tag] Format 123
```

**Log levels controlled via build flag:**
- `LOG_LEVEL_NONE` (0) - No logging
- `LOG_LEVEL_ERROR` (1) - Errors only
- `LOG_LEVEL_INFO` (2) - Info and errors
- `LOG_LEVEL_DEBUG` (3) - All messages

### Enforcement Guidelines

**All AI Agents MUST:**
1. Follow naming conventions exactly as specified in this document
2. Place new files in the correct directory per structure patterns
3. Use `#pragma once` for all headers
4. Return `Error` enum from fallible operations
5. Use `LOG_*` macros instead of direct `Serial.print`
6. Add new constants to appropriate `include/Config/*.h` file

**Pattern Verification:**
- Code review checks naming and structure compliance
- Build warnings for direct Serial.print in new code (future lint rule)
- New components must follow established interface patterns

### Pattern Examples

**Good Example - New Action Class:**
```cpp
// src/Menu/Action/SelectWheelModeAction.h
#pragma once
#include "MenuAction.h"
#include "Config/ConfigManager.h"

class SelectWheelModeAction : public MenuAction {
    WheelMode targetMode;
public:
    explicit SelectWheelModeAction(WheelMode mode);
    void execute() override;
    const char* getConfirmationMessage() const override;
};
```

**Anti-Patterns to Avoid:**
```cpp
// ❌ Wrong file location
src/set_wheel_mode.cpp  // Should be src/Menu/Action/

// ❌ Wrong naming
class set_wheel_mode_action  // Should be PascalCase

// ❌ Direct serial output
Serial.println("Mode saved");  // Should use LOG_INFO

// ❌ Missing error handling
void saveMode(WheelMode m) { prefs.putUChar(...); }  // Should return Error
```

## Project Structure & Boundaries

### Complete Project Directory Structure

```
UtilityButtonsWithKnobUSB/
├── platformio.ini                      # Build configuration (existing)
├── README.md                           # Project readme
│
├── boards/                             # Custom board definitions (existing)
│   └── nologo_esp32c3_super_mini.json
│
├── include/                            # Header files
│   ├── AppState.h                      # (existing) Global state, queues
│   ├── version.h                       # (existing) Version info
│   ├── Config/
│   │   ├── device_config.h             # (existing) BLE name, manufacturer
│   │   ├── encoder_config.h            # (existing) Encoder GPIO pins
│   │   ├── button_config.h             # (new) Button GPIO array, labels
│   │   ├── menu_config.h               # (new) Menu depth, timeout, defaults
│   │   ├── display_config.h            # (new) Display dimensions, addresses
│   │   └── log_config.h                # (new) Logging macros and levels
│   ├── Enum/
│   │   ├── EventEnum.h                 # (existing + extend) Event types
│   │   ├── WheelModeEnum.h             # (new) Wheel mode values
│   │   ├── ButtonActionEnum.h          # (new) Button action values
│   │   └── ErrorEnum.h                 # (new) Error return codes
│   └── Type/
│       ├── MenuEvent.h                 # (new) Menu-specific events
│       ├── SystemEvent.h               # (new) System-specific events
│       └── EncoderInputEvent.h         # (existing) Encoder input struct
│
├── lib/                                # Custom libraries (existing)
│   ├── EncoderDriver/
│   │   ├── EncoderDriver.h
│   │   └── EncoderDriver.cpp
│   └── StatsMonitor/
│       └── StatsMonitor.h
│
├── src/                                # Main application source
│   ├── main.cpp                        # (existing + modify) Entry point, initialization
│   │
│   ├── Component/                      # (existing) Interface definitions
│   │   └── Interface/
│   │       └── EncoderInputHandlerInterface.h
│   │
│   ├── EncoderMode/                    # (existing) Mode handlers
│   │   ├── Handler/
│   │   │   ├── EncoderModeHandlerAbstract.cpp/.h
│   │   │   ├── EncoderModeHandlerInterface.h
│   │   │   ├── EncoderModeHandlerScroll.cpp/.h
│   │   │   ├── EncoderModeHandlerVolume.cpp/.h
│   │   │   └── EncoderModeHandlerZoom.cpp/.h    # (new) Zoom mode
│   │   ├── Interface/
│   │   │   └── EncoderModeBaseInterface.h
│   │   ├── Manager/
│   │   │   └── EncoderModeManager.cpp/.h
│   │   └── Selector/
│   │       └── EncoderModeSelector.cpp/.h
│   │
│   ├── Event/                          # (existing) Event system
│   │   ├── Dispatcher/
│   │   │   ├── MenuEventDispatcher.cpp/.h       # (new) Menu events
│   │   │   ├── SystemEventDispatcher.cpp/.h     # (new) System events
│   │   │   └── EncoderEventDispatcher.cpp/.h
│   │   └── Handler/
│   │       ├── MenuEventHandler.cpp/.h          # (new) Menu logic formatting
│   │       ├── SystemEventHandler.cpp/.h        # (new) System state formatting
│   │       └── EncoderEventHandler.cpp/.h       # (modify) Add menu interception
│   │
│   ├── Helper/                         # (existing) Utilities
│   │   └── EncoderModeHelper.cpp/.h
│   │
│   ├── Menu/                           # (new) Menu system
│   │   ├── Controller/
│   │   │   └── MenuController.cpp/.h            # Menu state machine, event interception
│   │   ├── Model/
│   │   │   ├── MenuItem.h                       # Menu item struct with tree pointers
│   │   │   └── MenuTree.h                       # Static menu tree (HEADER-ONLY, constexpr)
│   │   └── Action/
│   │       ├── MenuAction.h                     # Abstract action base class
│   │       ├── SelectWheelModeAction.cpp/.h     # Select wheel mode
│   │       ├── SetButtonActionAction.cpp/.h     # Change button behavior
│   │       ├── ShowStatusAction.cpp/.h          # Display device status
│   │       └── ShowAboutAction.cpp/.h           # Display about info
│   │
│   ├── Display/                        # (new) Display abstraction
│   │   ├── Interface/
│   │   │   └── DisplayInterface.h               # Abstract display interface
│   │   ├── Task/
│   │   │   └── DisplayTask.cpp/.h               # (new) Arbitration Task
│   │   └── Impl/
│   │       ├── SerialDisplay.cpp/.h             # Serial output implementation
│   │       └── OLEDDisplay.cpp/.h               # OLED implementation (future)
│   │
│   ├── Config/                         # (new) Configuration management
│   │   ├── ConfigManager.cpp/.h                 # NVS read/write, defaults (DI-enabled)
│   │   └── FactoryReset.cpp/.h                  # Factory reset detection and execution
│   │
│   ├── System/                         # (new) System components
│   │   └── State/
│   │       └── SystemState.h                    # Global system state
│   │
│   └── Button/                         # (new) Button management
│       └── ButtonManager.cpp/.h                 # GPIO setup, button event dispatch
│
├── test/                               # (Deferred) Test files
│   ├── unit/
│   │   ├── test_ConfigManager.cpp
│   │   ├── test_MenuController.cpp
│   │   └── test_MenuItem.cpp
│   └── mocks/
│       ├── MockDisplay.h
│       └── MockPreferences.h
│
└── docs/                               # Documentation
    ├── index.md
    ├── architecture.md                 # This document
    ├── prd.md
    └── ...
```

### Design Clarifications

**MenuTree as Header-Only (constexpr):**
```cpp
// src/Menu/Model/MenuTree.h
#pragma once
#include "MenuItem.h"

// All menu items defined as constexpr - no runtime initialization
constexpr MenuItem WHEEL_MODE_ITEMS[] = {
    {"Scroll", nullptr, nullptr, 0, &setScrollModeAction},
    {"Volume", nullptr, nullptr, 0, &setVolumeModeAction},
    {"Zoom", nullptr, nullptr, 0, &setZoomModeAction}
};

constexpr MenuItem MAIN_MENU_ITEMS[] = {
    {"Wheel Behavior", nullptr, WHEEL_MODE_ITEMS, 3, nullptr},
    {"Button Config", nullptr, BUTTON_CONFIG_ITEMS, BUTTON_COUNT, nullptr},
    {"Device Status", nullptr, nullptr, 0, &showStatusAction},
    {"About", nullptr, nullptr, 0, &showAboutAction}
};
```

**ConfigManager Dependency Injection:**
```cpp
// src/Config/ConfigManager.h
class ConfigManager {
    Preferences* prefs;  // Injected, not global
public:
    // Production: pass &Preferences instance
    // Testing: pass MockPreferences*
    explicit ConfigManager(Preferences* preferences);

    Error saveWheelMode(WheelMode mode);
    WheelMode loadWheelMode();
};
```

**Factory Reset Trigger:**
```cpp
// src/Config/FactoryReset.cpp/.h
// Checked in main.cpp setup() BEFORE normal initialization

class FactoryReset {
public:
    // Check if encoder button held during boot (5+ seconds)
    static bool isResetRequested();

    // Clear all NVS config, restore defaults
    static void execute(ConfigManager& config, DisplayInterface& display);
};
```

### Architectural Boundaries

**Event System Boundaries:**
```
┌─────────────────────────────────────────────────────────────────┐
│                        Input Layer                               │
│  EncoderDriver ──→ EncoderEventDispatcher ──→ EncoderInputQueue │
│  ButtonManager ──→ EncoderEventDispatcher ──→ EncoderInputQueue │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                     Logic/Domain Layer                           │
│  EncoderEventHandler                                             │
│    ├── MenuController.isActive() ? Emit MenuEvent               │
│    └── else → EncoderModeManager → Emit Mode Action             │
│                                                                 │
│  System Components ──→ SystemEventDispatcher ──→ SystemEventQueue│
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                     Output Processing Layer                      │
│  MenuEventQueue ──→ MenuEventHandler                             │
│                      ↓ (Emits Draw Command)                     │
│  SystemEventQueue ──→ SystemEventHandler                         │
│                      ↓ (Emits Draw Command)                     │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                     Hardware Layer                               │
│  DisplayRequestQueue ──→ DisplayTask ──→ DisplayInterface       │
└─────────────────────────────────────────────────────────────────┘
```

**Component Boundaries:**

| Boundary | Owner | Consumers | Communication |
|----------|-------|-----------|---------------|
| Display Hardware | DisplayTask | Handlers (Menu/System) | Via DisplayRequestQueue |
| Menu Logic | MenuController | EncoderEventHandler | Direct call (interception) |
| Menu Output | MenuEventHandler | MenuController | Via MenuEventQueue |
| System State | SystemEventHandler | Various components | Via SystemEventQueue |
| Config Persistence | ConfigManager | MenuAction classes | Direct method calls (DI) |

### Build Order Dependencies

Components must be built/implemented in this order due to compile-time dependencies:

```
Phase 1: Foundation
├── include/Config/log_config.h
├── include/Enum/ErrorEnum.h
├── include/Enum/WheelModeEnum.h
├── include/Enum/ButtonActionEnum.h
└── include/Config/button_config.h

Phase 2: Interfaces
├── src/Display/Interface/DisplayInterface.h
└── src/Menu/Model/MenuItem.h

Phase 3: Core Implementations
├── src/Display/Impl/SerialDisplay.cpp/.h
├── src/Config/ConfigManager.cpp/.h
├── src/Button/ButtonManager.cpp/.h
├── src/Menu/Action/MenuAction.h

Phase 4: Display Arbitration (NEW)
├── src/Display/Model/DisplayRequest.h
└── src/Display/Task/DisplayTask.cpp/.h

Phase 5: Menu System
├── src/Menu/Action/*Action.cpp/.h
├── src/Menu/Model/MenuTree.h
├── src/Menu/Controller/MenuController.cpp/.h

Phase 6: Integration (Distributed Events)
├── src/Event/Dispatcher/MenuEventDispatcher.cpp/.h
├── src/Event/Handler/MenuEventHandler.cpp/.h
├── src/Event/Dispatcher/SystemEventDispatcher.cpp/.h
├── src/Event/Handler/SystemEventHandler.cpp/.h
└── src/main.cpp (wire everything together)
```

### Requirements to Structure Mapping

| FR Category | Component Location | Key Files |
|-------------|-------------------|-----------|
| Menu System (FR1-FR8) | src/Menu/ | MenuController, MenuItem, MenuTree |
| Wheel Config (FR9-FR14) | src/Menu/Action/ | SelectWheelModeAction |
| Button Config (FR15-FR19) | src/Menu/Action/, src/Button/ | SetButtonActionAction, ButtonManager |
| Device Status (FR20-FR23) | src/Menu/Action/ | ShowStatusAction |
| About Screen (FR24-FR25) | src/Menu/Action/ | ShowAboutAction |
| Config Persistence (FR26-FR29) | src/Config/ | ConfigManager, FactoryReset |
| Display Interface (FR35-FR37) | src/Display/ | DisplayInterface, SerialDisplay |
| Input Handling (FR38-FR42) | src/Button/ | ButtonManager |
| BLE Handling (FR30-FR34) | src/Event/Handler/ | AppEventHandler (BLE_DISCONNECTED) |

### File Modification Summary

**Existing Files to Modify:**

| File | Changes |
|------|---------|
| `src/main.cpp` | Add ConfigManager, ButtonManager, DisplayHandler init; factory reset check |
| `src/Event/Handler/EncoderEventHandler.cpp` | Add MenuController interception check |
| `src/Event/Handler/AppEventHandler.cpp` | Add DisplayHandler routing, BLE disconnect handling |
| `include/Enum/EventEnum.h` | Add MENU_*, CONFIG_CHANGED, BLE_DISCONNECTED |
| `include/Type/AppEvent.h` | Add union-based data payload |

**New Files to Create:** 23 files across 6 new directories

## Architecture Validation Results

### Coherence Validation ✅

**Decision Compatibility:**
All technology choices verified compatible. PlatformIO + Arduino framework works seamlessly with NimBLE 2.2.3, Adafruit SSD1306, and custom event-driven architecture. No version conflicts detected.

**Pattern Consistency:**
Implementation patterns align with existing codebase conventions. Naming patterns (PascalCase files, camelCase functions, UPPER_SNAKE constants) match established code. Error handling and logging patterns standardized.

**Structure Alignment:**
New directories (Menu/, Display/, Config/, Button/) follow established subdirectory patterns (Handler/, Interface/, Impl/). All new components integrate with existing event system without architectural changes.

### Requirements Coverage Validation ✅

**Functional Requirements:**
All 42 functional requirements mapped to specific architectural components:
- FR1-FR8 (Menu): MenuController, MenuItem, MenuTree
- FR9-FR14 (Wheel): SelectWheelModeAction, ConfigManager
- FR15-FR19 (Buttons): SetButtonActionAction, ButtonManager
- FR20-FR29 (Status/Config): ShowStatusAction, ShowAboutAction, ConfigManager, FactoryReset
- FR30-FR42 (BLE/Display/Input): Existing components + BLE_DISCONNECTED event

**Non-Functional Requirements:**
All 12 NFRs addressed through architectural decisions:
- Performance: Event-driven, non-blocking, static compile-time menu
- Reliability: NVS persistence, graceful defaults, factory reset
- Maintainability: Interface abstraction, DI, standardized patterns
- Compatibility: ESP32-C3 Super Mini reference hardware

### Implementation Readiness Validation ✅

**Decision Completeness:**
9 major architectural decisions documented with:
- Code examples for each pattern
- Rationale explaining choices
- Verified technology versions

**Structure Completeness:**
- 23 new files defined across 6 directories
- 5 existing files with specific modification requirements
- 5-phase build order with dependency chain
- FR-to-file mapping complete

**Pattern Completeness:**
- 8 naming patterns with conventions and examples
- Good patterns and anti-patterns documented
- Enforcement guidelines for AI agents

### Gap Analysis Results

**Critical Gaps:** None identified

**Important Gaps:**
1. EncoderModeHandlerZoom implementation - follows existing Scroll/Volume handler pattern

**Nice-to-Have (Future):**
1. Sequence diagrams for complex flows
2. Memory usage analysis
3. Unit test implementation examples

### Architecture Completeness Checklist

**✅ Requirements Analysis**
- [x] Project context thoroughly analyzed
- [x] Scale and complexity assessed (Low complexity, IoT/Embedded)
- [x] Technical constraints identified (ESP32-C3, 400KB SRAM, GPIO limits)
- [x] Cross-cutting concerns mapped (Display, Persistence, Events, State)

**✅ Architectural Decisions**
- [x] Critical decisions documented with versions
- [x] Technology stack fully specified (PlatformIO, Arduino, NimBLE)
- [x] Integration patterns defined (Event queues, handler pattern)
- [x] Performance considerations addressed (static menu, no dynamic alloc)

**✅ Implementation Patterns**
- [x] Naming conventions established (8 patterns)
- [x] Structure patterns defined (directory organization)
- [x] Communication patterns specified (AppEvent, DisplayHandler)
- [x] Process patterns documented (Error enum, LOG_* macros)

**✅ Project Structure**
- [x] Complete directory structure defined
- [x] Component boundaries established
- [x] Integration points mapped
- [x] Requirements to structure mapping complete

### Architecture Readiness Assessment

**Overall Status:** READY FOR IMPLEMENTATION

**Confidence Level:** HIGH

**Key Strengths:**
1. Clean integration with existing event-driven architecture
2. Display abstraction enables serial-first, OLED-later approach
3. Static menu tree avoids dynamic allocation on constrained device
4. DI-enabled ConfigManager supports unit testing
5. Comprehensive pattern documentation prevents AI agent conflicts

**Areas for Future Enhancement:**
1. OLEDDisplay implementation when hardware ready
2. Multi-profile support (deferred per PRD)
3. OTA update mechanism (post-MVP)

### Implementation Handoff

**AI Agent Guidelines:**
- Follow all architectural decisions exactly as documented
- Use implementation patterns consistently across all components
- Respect project structure and component boundaries
- Use LOG_* macros, not direct Serial.print
- Return Error enum from fallible operations
- Use DI for ConfigManager testability

**First Implementation Priority (Build Phase 1):**
1. `include/Config/log_config.h` - Logging macros
2. `include/Enum/ErrorEnum.h` - Error codes
3. `include/Enum/WheelModeEnum.h` - Wheel modes
4. `include/Enum/ButtonActionEnum.h` - Button actions
5. `include/Config/button_config.h` - Button GPIO array

## Architecture Completion Summary

### Workflow Completion

**Architecture Decision Workflow:** COMPLETED ✅
**Total Steps Completed:** 8
**Date Completed:** 2025-12-16
**Document Location:** docs/architecture.md

### Final Architecture Deliverables

**Complete Architecture Document**
- All architectural decisions documented with specific versions
- Implementation patterns ensuring AI agent consistency
- Complete project structure with all files and directories
- Requirements to architecture mapping
- Validation confirming coherence and completeness

**Implementation Ready Foundation**
- 9 major architectural decisions made
- 8 implementation patterns defined
- 6 new component directories specified
- 42 functional requirements + 12 NFRs fully supported

**AI Agent Implementation Guide**
- Technology stack with verified versions
- Consistency rules that prevent implementation conflicts
- Project structure with clear boundaries
- Integration patterns and communication standards

### Development Sequence

1. **Phase 1: Foundation** - Create enums, config headers, log macros
2. **Phase 2: Interfaces** - Define DisplayInterface, MenuItem structures
3. **Phase 3: Implementations** - Build SerialDisplay, ConfigManager, ButtonManager
4. **Phase 4: Menu System** - Implement MenuActions, MenuTree, MenuController
5. **Phase 5: Integration** - Wire DisplayHandler, modify event handlers, update main.cpp

### Quality Assurance Checklist

**✅ Architecture Coherence**
- [x] All decisions work together without conflicts
- [x] Technology choices are compatible
- [x] Patterns support the architectural decisions
- [x] Structure aligns with all choices

**✅ Requirements Coverage**
- [x] All 42 functional requirements are supported
- [x] All 12 non-functional requirements are addressed
- [x] Cross-cutting concerns are handled
- [x] Integration points are defined

**✅ Implementation Readiness**
- [x] Decisions are specific and actionable
- [x] Patterns prevent agent conflicts
- [x] Structure is complete and unambiguous
- [x] Examples are provided for clarity

---

**Architecture Status:** READY FOR IMPLEMENTATION ✅

**Next Phase:** Begin implementation using the architectural decisions and patterns documented herein.

**Document Maintenance:** Update this architecture when major technical decisions are made during implementation.

