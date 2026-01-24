# Project Structure & Boundaries

*Last Updated: 2026-01-22*

## Complete Project Directory Structure

```
UtilityButtonsWithKnobUSB/
├── platformio.ini                      # Build configuration
├── README.md                           # Project readme
│
├── boards/                             # Custom board definitions
│   └── nologo_esp32c3_super_mini.json
│
├── include/                            # Header files
│   ├── README                          # Include directory readme
│   ├── version.h                       # Version info
│   │
│   ├── Config/
│   │   ├── device_config.h             # Device identity, BLE name, NVS namespace
│   │   ├── encoder_config.h            # Encoder GPIO pins
│   │   ├── button_config.h             # Button GPIO array, labels, actions
│   │   ├── display_config.h            # Display dimensions, I2C config (128x32)
│   │   ├── system_config.h             # Power management timeouts
│   │   └── log_config.h                # Logging macros and levels
│   │
│   ├── Enum/
│   │   ├── EventEnum.h                 # Event types (encoder, menu, app)
│   │   ├── WheelModeEnum.h             # Wheel mode values (Scroll, Volume, Zoom)
│   │   ├── WheelDirection.h            # Wheel direction (Normal, Reversed)
│   │   ├── ButtonActionEnum.h          # Button actions (None, Mute, Play, etc.)
│   │   ├── PowerStateEnum.h            # Power states (Active, Warning, Sleep)
│   │   └── ErrorEnum.h                 # Error return codes
│   │
│   ├── Type/
│   │   ├── AppEvent.h                  # App event struct with union payload
│   │   ├── ButtonEvent.h               # Button event struct
│   │   └── EncoderInputEvent.h         # Encoder input struct
│   │
│   └── state/
│       └── HardwareState.h             # Global hardware state (BLE, display, mode)
│
├── lib/                                # Custom libraries
│   ├── EncoderDriver/
│   │   ├── EncoderDriver.h
│   │   └── EncoderDriver.cpp
│   └── StatsMonitor/
│       └── StatsMonitor.h
│
├── src/                                # Main application source
│   ├── main.cpp                        # Entry point, initialization, wiring
│   │
│   ├── BLE/                            # Bluetooth callback handling
│   │   ├── BleCallbackHandler.cpp
│   │   └── BleCallbackHandler.h
│   │
│   ├── Button/                         # Button management
│   │   ├── ButtonManager.cpp
│   │   └── ButtonManager.h
│   │
│   ├── Component/                      # Shared interfaces
│   │   └── Interface/
│   │       └── EncoderInputHandlerInterface.h
│   │
│   ├── Config/                         # Configuration management
│   │   ├── ConfigManager.cpp           # NVS read/write, defaults
│   │   ├── ConfigManager.h
│   │   ├── FactoryReset.cpp            # Factory reset detection and execution
│   │   └── FactoryReset.h
│   │
│   ├── Display/                        # Display system
│   │   ├── Bitmaps.h                   # Icon bitmaps (BT, battery, etc.)
│   │   ├── DisplayFactory.cpp          # Display instance factory
│   │   ├── DisplayFactory.h
│   │   │
│   │   ├── Interface/
│   │   │   └── DisplayInterface.h      # Abstract display interface
│   │   │
│   │   ├── Impl/
│   │   │   ├── OLEDDisplay.cpp         # SSD1306 128x32 OLED implementation
│   │   │   ├── OLEDDisplay.h
│   │   │   ├── SerialDisplay.cpp       # Serial output implementation (debug)
│   │   │   └── SerialDisplay.h
│   │   │
│   │   ├── Model/
│   │   │   └── DisplayRequest.h        # Display request types
│   │   │
│   │   └── Task/
│   │       ├── DisplayTask.cpp         # Display arbitration FreeRTOS task
│   │       └── DisplayTask.h
│   │
│   ├── EncoderMode/                    # Encoder mode handlers
│   │   ├── Handler/
│   │   │   ├── EncoderModeHandlerAbstract.cpp
│   │   │   ├── EncoderModeHandlerAbstract.h
│   │   │   ├── EncoderModeHandlerInterface.h
│   │   │   ├── EncoderModeHandlerScroll.cpp
│   │   │   ├── EncoderModeHandlerScroll.h
│   │   │   ├── EncoderModeHandlerVolume.cpp
│   │   │   ├── EncoderModeHandlerVolume.h
│   │   │   ├── EncoderModeHandlerZoom.cpp
│   │   │   └── EncoderModeHandlerZoom.h
│   │   │
│   │   ├── Interface/
│   │   │   └── EncoderModeBaseInterface.h
│   │   │
│   │   ├── Manager/
│   │   │   ├── EncoderModeManager.cpp
│   │   │   └── EncoderModeManager.h
│   │   │
│   │   └── Selector/
│   │       ├── EncoderModeSelector.cpp
│   │       └── EncoderModeSelector.h
│   │
│   ├── Event/                          # Event system
│   │   ├── Dispatcher/
│   │   │   ├── AppEventDispatcher.cpp
│   │   │   ├── AppEventDispatcher.h
│   │   │   ├── ButtonEventDispatcher.cpp
│   │   │   ├── ButtonEventDispatcher.h
│   │   │   ├── EncoderEventDispatcher.cpp
│   │   │   ├── EncoderEventDispatcher.h
│   │   │   ├── MenuEventDispatcher.cpp
│   │   │   └── MenuEventDispatcher.h
│   │   │
│   │   └── Handler/
│   │       ├── Interface/
│   │       │   └── EventHandlerInterface.h
│   │       ├── AppEventHandler.cpp
│   │       ├── AppEventHandler.h
│   │       ├── ButtonEventHandler.cpp
│   │       ├── ButtonEventHandler.h
│   │       ├── EncoderEventHandler.cpp
│   │       ├── EncoderEventHandler.h
│   │       ├── MenuEventHandler.cpp
│   │       └── MenuEventHandler.h
│   │
│   ├── Helper/                         # Utilities
│   │   ├── EncoderModeHelper.cpp
│   │   └── EncoderModeHelper.h
│   │
│   ├── Menu/                           # Menu system
│   │   ├── Action/
│   │   │   ├── MenuAction.h            # Abstract action base class
│   │   │   ├── DisconnectAction.cpp    # BLE disconnect
│   │   │   ├── DisconnectAction.h
│   │   │   ├── DisplayPowerAction.cpp  # Display on/off
│   │   │   ├── DisplayPowerAction.h
│   │   │   ├── PairAction.cpp          # BLE pairing
│   │   │   ├── PairAction.h
│   │   │   ├── SelectWheelDirectionAction.cpp
│   │   │   ├── SelectWheelDirectionAction.h
│   │   │   ├── SelectWheelModeAction.cpp
│   │   │   ├── SelectWheelModeAction.h
│   │   │   ├── SetButtonBehaviorAction.cpp
│   │   │   ├── SetButtonBehaviorAction.h
│   │   │   ├── ShowAboutAction.cpp
│   │   │   ├── ShowAboutAction.h
│   │   │   ├── ShowStatusAction.cpp
│   │   │   └── ShowStatusAction.h
│   │   │
│   │   ├── Controller/
│   │   │   ├── MenuController.cpp      # Menu state machine
│   │   │   └── MenuController.h
│   │   │
│   │   └── Model/
│   │       ├── MenuItem.h              # Menu item struct
│   │       └── MenuTree.h              # Static menu tree definition
│   │
│   └── System/                         # System components
│       ├── PowerManager.cpp            # Inactivity detection, deep sleep
│       └── PowerManager.h
│
└── _bmad-output/                       # Documentation
    ├── index.md
    ├── project-overview.md
    ├── project-context.md
    ├── architecture/
    └── development-guide/
```

## Design Clarifications

**MenuTree Implementation:**
The menu system uses inline arrays (not constexpr) because parent pointers create circular references that cannot be resolved at compile time. Action pointers are set at runtime via dependency injection.

```cpp
// src/Menu/Model/MenuTree.h
inline MenuItem mainMenu[] = {
    { "Wheel Behavior", nullptr, wheelBehaviorSubmenu, WHEEL_BEHAVIOR_COUNT, nullptr },
    { "Button Config",  nullptr, buttonConfigSubmenu, BUTTON_COUNT, nullptr },
    { "Bluetooth",      nullptr, bluetoothSubmenu, BLUETOOTH_SUBMENU_COUNT, nullptr },
    { "Display Off",    nullptr, nullptr, 0, nullptr },
    { "Device Status",  nullptr, nullptr, 0, nullptr },
    { "About",          nullptr, nullptr, 0, nullptr }
};
```

**ConfigManager Dependency Injection:**
```cpp
// src/Config/ConfigManager.h
class ConfigManager {
    Preferences prefs;  // Internal, opened with NVS_NAMESPACE
public:
    ConfigManager();  // Opens NVS namespace

    Error saveWheelMode(WheelMode mode);
    WheelMode loadWheelMode();
    Error saveWheelDirection(WheelDirection dir);
    WheelDirection loadWheelDirection();
    Error saveButtonAction(uint8_t buttonIndex, ButtonAction action);
    ButtonAction loadButtonAction(uint8_t buttonIndex);
    void resetToDefaults();
};
```

**Factory Reset Trigger:**
```cpp
// Checked in main.cpp setup() BEFORE normal initialization
// Hold encoder button for 5+ seconds during boot triggers reset

class FactoryReset {
public:
    static bool isResetRequested(uint8_t buttonPin);
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
│  ButtonEventHandler → Execute configured ButtonAction            │
│                                                                 │
│  MenuEventHandler → Process menu navigation events               │
│                                                                 │
│  PowerManager → Monitor inactivity, trigger sleep warnings       │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                     Output Processing Layer                      │
│  AppEventQueue ──→ AppEventHandler                               │
│                      ↓ (Mode changes, BLE events)               │
│  DisplayRequestQueue ──→ DisplayTask ──→ DisplayInterface       │
└─────────────────────────────────────────────────────────────────┘
```

**Component Boundaries:**

| Boundary | Owner | Consumers | Communication |
|----------|-------|-----------|---------------|
| Display Hardware | DisplayTask | All Handlers | Via DisplayRequestQueue |
| Menu Logic | MenuController | EncoderEventHandler | Direct call (interception) |
| BLE State | BleCallbackHandler | Display, Menu | Via HardwareState updates |
| Config Persistence | ConfigManager | MenuAction classes | Direct method calls |
| Power State | PowerManager | Display, main loop | Via PowerState enum |

## Requirements to Structure Mapping

| FR Category | Component Location | Key Files |
|-------------|-------------------|-----------|
| Menu System | src/Menu/ | MenuController, MenuItem, MenuTree |
| Wheel Config | src/Menu/Action/ | SelectWheelModeAction, SelectWheelDirectionAction |
| Button Config | src/Menu/Action/, src/Button/ | SetButtonBehaviorAction, ButtonManager |
| Bluetooth Control | src/Menu/Action/, src/BLE/ | PairAction, DisconnectAction, BleCallbackHandler |
| Display Control | src/Menu/Action/, src/Display/ | DisplayPowerAction, OLEDDisplay |
| Device Status | src/Menu/Action/ | ShowStatusAction |
| About Screen | src/Menu/Action/ | ShowAboutAction |
| Config Persistence | src/Config/ | ConfigManager, FactoryReset |
| Display Interface | src/Display/ | DisplayInterface, OLEDDisplay, DisplayTask |
| Input Handling | src/Button/, lib/EncoderDriver/ | ButtonManager, EncoderDriver |
| Power Management | src/System/ | PowerManager |

## Implementation Status

**Completed (Epics 6-10):**
- ✅ Complete menu system with hierarchical navigation
- ✅ Wheel mode selection (Scroll, Volume, Zoom)
- ✅ Wheel direction control (Normal, Reversed)
- ✅ Button configuration (4 buttons × 6 actions)
- ✅ Bluetooth pairing and disconnect
- ✅ OLED display (128x32) with status screens
- ✅ Display power control
- ✅ Power management with deep sleep
- ✅ Factory reset functionality

**Upcoming (Sprint 2 - Epic 11):**
- 🔨 LED Strip Control integration
