# Implementation Patterns & Consistency Rules

## Critical Conflict Points Addressed

8 potential conflict areas identified and standardized for AI agent consistency.

## Naming Patterns

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

## Structure Patterns

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

## Format Patterns

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

## Communication Patterns

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

## Enforcement Guidelines

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

## Pattern Examples

**Good Example - EncoderModeHandler with DI:**
```cpp
// src/EncoderMode/Handler/EncoderModeHandlerZoom.h (Example for all mode handlers)
#pragma once
#include "EncoderModeHandlerAbstract.h"
#include "BleKeyboard.h" // Assuming BleKeyboard is available via #include

class EncoderModeHandlerZoom : public EncoderModeHandlerAbstract {
public:
    // All EncoderModeHandlers MUST accept BleKeyboard* via constructor DI
    EncoderModeHandlerZoom(AppEventDispatcher* dispatcher, BleKeyboard* bleKeyboard);

    void handleRotate(int delta) override;
    void handleShortClick() override;
    const char* getModeName() const override;
};
```

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
