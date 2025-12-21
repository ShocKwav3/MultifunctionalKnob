# Source Tree Analysis

**Project:** UtilityButtonsWithKnobUSB
**Last Updated:** 2025-12-14

## Complete Directory Structure

```
UtilityButtonsWithKnobUSB/
├── .bmad/                          # BMAD workflow system (development tools)
├── .claude/                        # Claude AI configuration
├── .roo/                          # Roo configuration
├── .vscode/                       # VSCode workspace settings
│
├── boards/                        # Custom board definitions
│   └── nologo_esp32c3_super_mini.json  # ESP32-C3 Super Mini board config
│
├── bmadocs/                       # BMAD documentation artifacts
│   └── sprint-artifacts/          # Sprint planning documents
│
├── include/                       # 🔑 Header files and configuration
│   ├── AppState.h                # Global application state structure
│   ├── version.h                 # Version information
│   ├── Config/                   # Configuration headers
│   │   ├── device_config.h      # Bluetooth device settings, button timings
│   │   └── encoder_config.h     # GPIO pin assignments, encoder steps
│   ├── Enum/                     # Enumeration definitions
│   │   └── EventEnum.h          # Event type enumerations (input, mode)
│   └── Type/                     # Type definitions
│       ├── AppEvent.h           # Application event structure
│       └── EncoderInputEvent.h  # Encoder input event structure
│
├── lib/                          # 🔑 Custom libraries
│   ├── EncoderDriver/           # Rotary encoder driver wrapper
│   │   ├── EncoderDriver.cpp   # Implementation
│   │   └── EncoderDriver.h     # Singleton driver, ISR handling, callbacks
│   └── StatsMonitor/            # Performance monitoring (future)
│       └── StatsMonitor.h      # Header only
│
├── src/                          # 🔑 Main application code
│   ├── main.cpp                 # ⚡ ENTRY POINT - Setup and main loop
│   │
│   ├── Component/               # Component interfaces
│   │   └── Interface/
│   │       └── EncoderInputHandlerInterface.h  # Input handler contract
│   │
│   ├── EncoderMode/            # 🎯 Encoder mode system
│   │   ├── Handler/            # Mode implementation handlers
│   │   │   ├── EncoderModeHandlerAbstract.cpp    # Base handler
│   │   │   ├── EncoderModeHandlerAbstract.h      # Common mode logic
│   │   │   ├── EncoderModeHandlerInterface.h     # Handler contract
│   │   │   ├── EncoderModeHandlerScroll.cpp      # Scroll mode impl
│   │   │   ├── EncoderModeHandlerScroll.h        # Scroll mode (mouse wheel)
│   │   │   ├── EncoderModeHandlerVolume.cpp      # Volume mode impl
│   │   │   └── EncoderModeHandlerVolume.h        # Volume mode (media keys)
│   │   │
│   │   ├── Interface/          # Base interfaces
│   │   │   └── EncoderModeBaseInterface.h        # Common mode interface
│   │   │
│   │   ├── Manager/            # Mode lifecycle management
│   │   │   ├── EncoderModeManager.cpp            # Mode switching logic
│   │   │   └── EncoderModeManager.h              # Handler registration, mode state
│   │   │
│   │   └── Selector/           # Mode selection UI
│   │       ├── EncoderModeSelector.cpp           # Selection logic
│   │       └── EncoderModeSelector.h             # Interactive mode picker
│   │
│   ├── Event/                  # 🔄 Event handling system
│   │   ├── Dispatcher/         # Event dispatchers
│   │   │   ├── AppEventDispatcher.cpp            # App event distribution
│   │   │   ├── AppEventDispatcher.h              # Queue-based dispatcher
│   │   │   ├── EncoderEventDispatcher.cpp        # Encoder event distribution
│   │   │   └── EncoderEventDispatcher.h          # Input event creator
│   │   │
│   │   └── Handler/            # Event handlers (FreeRTOS tasks)
│   │       ├── AppEventHandler.cpp               # App event processor
│   │       ├── AppEventHandler.h                 # Mode management integration
│   │       ├── EncoderEventHandler.cpp           # Input event processor
│   │       └── EncoderEventHandler.h             # Delegates to mode handlers
│   │
│   ├── Helper/                 # Utility functions
│   │   ├── EncoderModeHelper.cpp                 # Mode helper functions
│   │   └── EncoderModeHelper.h                   # Mode utilities
│   │
│   ├── Menu/                   # Menu system (future implementation)
│   │
│   └── minimal_*.txt           # Reference examples
│       ├── minimal_bluethooth_hid_scroll.txt     # BLE HID example
│       ├── minimal_encoder.txt                   # Encoder example
│       └── minimal_i2c_oled.txt                  # OLED example
│
├── test/                       # Test files (placeholder)
│
├── .gitignore                  # Git ignore patterns
├── platformio.ini              # ⚙️ PlatformIO build configuration
├── README.md                   # Project readme (empty)
└── copier.py                   # Utility script
```

## Critical Directories Explained

### `/src/` - Main Application Code

**Purpose:** Contains all application logic, entry point, and core systems

**Key Files:**
- [`main.cpp`](../src/main.cpp:1) - Application entry point
  - `setup()`: Initializes hardware, creates queues, registers handlers
  - `loop()`: Currently implements test scroll code (BLE HID mouse movements)

**Subsystems:**
- **EncoderMode/**: Mode system (Scroll, Volume, Selection)
- **Event/**: Event-driven architecture (dispatchers + handlers)
- **Component/**: Interface definitions
- **Helper/**: Shared utilities

### `/include/` - Headers and Configuration

**Purpose:** Project-wide header files, configuration constants, type definitions

**Critical Files:**
- [`Config/device_config.h`](../include/Config/device_config.h:1) - Bluetooth settings, button timings
- [`Config/encoder_config.h`](../include/Config/encoder_config.h:1) - GPIO pins, encoder parameters
- [`Enum/EventEnum.h`](../include/Enum/EventEnum.h:1) - Event type enums
- [`Type/AppEvent.h`](../include/Type/AppEvent.h:1) - Application event structure
- [`Type/EncoderInputEvent.h`](../include/Type/EncoderInputEvent.h:1) - Input event structure
- [`AppState.h`](../include/AppState.h:1) - Global state (FreeRTOS queues)

### `/lib/` - Custom Libraries

**Purpose:** Reusable library code, hardware abstraction layers

**Libraries:**
- **EncoderDriver**: Wraps ai-esp32-rotary-encoder library
  - Singleton pattern
  - ISR-safe encoder handling
  - Callback-based event notification
  - FreeRTOS task for button handling

- **StatsMonitor**: Performance monitoring (future implementation)

### `/boards/` - Board Definitions

**Purpose:** PlatformIO custom board configurations

**File:** [`nologo_esp32c3_super_mini.json`](../boards/nologo_esp32c3_super_mini.json:1)
- Defines ESP32-C3 Super Mini variant
- Specifies USB CDC configuration
- Referenced in `platformio.ini`

## Entry Points

### Main Entry Point

**File:** [`src/main.cpp`](../src/main.cpp:1)

**Function:** `void setup()`

**Initialization Sequence:**
1. Serial communication (460800 baud, USB CDC)
2. I2C bus initialization (SDA: GPIO 6, SCL: GPIO 7)
3. OLED display setup (SSD1306, 128x64, I2C address 0x3C)
4. Bluetooth HID keyboard initialization
5. FreeRTOS queue creation (encoder input, app events)
6. Event dispatcher instantiation
7. Mode handler registration (Scroll, Volume)
8. Event handler task startup (FreeRTOS)
9. Encoder driver initialization with callbacks
10. Display welcome message

**Function:** `void loop()`

**Current Implementation:**
- Test code for BLE HID scroll functionality
- Sends horizontal/vertical scroll commands when BLE connected
- 5-second delay between test sequences

### Hardware Interrupts

**ISR Handler:** `EncoderDriver::readEncoderISR()`
- Attached to encoder CLK and DT pins
- Handles rotary encoder quadrature signals
- Updates encoder value atomically

## Integration Points

### Hardware → Software

| Hardware | GPIO | Driver | Event Path |
|----------|------|--------|------------|
| Encoder CLK | 1 | EncoderDriver | ISR → EncoderEventDispatcher → Queue |
| Encoder DT | 0 | EncoderDriver | ISR → EncoderEventDispatcher → Queue |
| Encoder Button | 2 | EncoderDriver | Task → EncoderEventDispatcher → Queue |
| OLED SDA | 6 | Adafruit_SSD1306 | Direct I2C |
| OLED SCL | 7 | Adafruit_SSD1306 | Direct I2C |
| BLE Radio | - | BleKeyboard | Direct API calls |

### Event Flow

```
User Input (Hardware)
    ↓
EncoderDriver (ISR/Task)
    ↓
EncoderEventDispatcher
    ↓
EncoderInputEvent Queue (FreeRTOS)
    ↓
EncoderEventHandler (Task)
    ↓
Current Mode Handler (Scroll/Volume)
    ↓
BLE HID Output / AppEvent Queue
    ↓
AppEventHandler (Task)
    ↓
EncoderModeManager (mode switching)
```

### FreeRTOS Tasks

| Task | Priority | Purpose | Queue |
|------|----------|---------|-------|
| EncoderEventHandler | Default | Process encoder inputs | encoderInputEventQueue |
| AppEventHandler | Default | Process app events | appEventQueue |
| EncoderDriver Button Task | Default | Handle button press timing | - |

## Build Outputs

**Build Directory:** `.pio/build/`

**Firmware Output:** `.pio/build/<env>/firmware.bin`

**Upload Method:** USB Serial/JTAG (ESP32-C3 built-in)

## Dependencies and External Libraries

### PlatformIO Dependencies

Defined in [`platformio.ini`](../platformio.ini:19):

1. **NimBLE-Arduino** (v2.2.3)
   - Bluetooth Low Energy stack
   - Used by: BleKeyboard library

2. **ai-esp32-rotary-encoder** (v1.7)
   - Rotary encoder handling with debouncing
   - Used by: EncoderDriver wrapper

3. **Adafruit SSD1306** (v2.5.15)
   - OLED display driver
   - Dependency: Adafruit_GFX

4. **BleKeyboard** (ESP32-BLE-Keyboard fork from ShocKwav3)
   - BLE HID keyboard/mouse implementation
   - GitHub: ShocKwav3/ESP32-BLE-Keyboard

### Framework Dependencies

- **Arduino Framework**: Core HAL for ESP32
- **FreeRTOS**: Built into ESP-IDF/Arduino
- **ESP32 Hardware Libraries**: Wire (I2C), Serial

## Excluded Directories

- `.pio/`: PlatformIO build artifacts
- `.git/`: Version control
- `.bmad/`, `.claude/`, `.roo/`: Development tools
- `.vscode/`: IDE configuration
- `bmadocs/`: Documentation artifacts (separate from code)

---

*This analysis was generated on 2025-12-14 during deep scan documentation workflow.*
