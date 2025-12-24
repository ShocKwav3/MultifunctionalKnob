# Project Structure & Boundaries

## Complete Project Directory Structure

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
│   │   ├── button_config.h              # (implemented) Button GPIO array, labels, debounce config
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
│   │   │   ├── EncoderModeHandlerAbstract.h
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

## Design Clarifications

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

## Architectural Boundaries

**Event System Boundaries:**
```
┌─────────────────────────────────────────────────────────────────┐
│                        Input Layer                               │
│  EncoderDriver ──→ EncoderEventDispatcher ──→ EncoderInputQueue │
│  ButtonManager ──→ ButtonEventDispatcher ──→ buttonEventQueue   │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                     Logic/Domain Layer                           │
│  EncoderEventHandler                                             │
│    ├── MenuController.isActive() ? Emit MenuEvent               │
│    └── else → EncoderModeManager → Emit Mode Action             │
│                                                                 │
│  ButtonEventHandler (logs button events for MVP)                │
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

## Build Order Dependencies

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

## Requirements to Structure Mapping

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

## File Modification Summary

**Existing Files to Modify:**

| File | Changes |
|------|---------|
| `src/main.cpp` | Add ConfigManager, ButtonManager, DisplayHandler init; factory reset check |
| `src/Event/Handler/EncoderEventHandler.cpp` | Add MenuController interception check |
| `src/Event/Handler/AppEventHandler.cpp` | Add DisplayHandler routing, BLE disconnect handling |
| `include/Enum/EventEnum.h` | Add MENU_*, CONFIG_CHANGED, BLE_DISCONNECTED |
| `include/Type/AppEvent.h` | Add union-based data payload |

**New Files to Create:** 23 files across 6 new directories
