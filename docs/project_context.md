---
project_name: 'UtilityButtonsWithKnobUSB'
user_name: 'Feroj'
date: '2025-12-16'
sections_completed: ['technology_stack', 'language_rules', 'framework_rules', 'code_quality', 'critical_rules']
status: 'complete'
rule_count: 80
optimized_for_llm: true
---

# Project Context for AI Agents

_This file contains critical rules and patterns that AI agents must follow when implementing code in this project. Focus on unobvious details that agents might otherwise miss._

---

## Technology Stack & Versions

**Platform:** PlatformIO with espressif32 platform, Arduino framework
**Target:** ESP32-C3 Super Mini (RISC-V 160MHz, 400KB SRAM, 4MB Flash)

### Dependencies (Exact Versions)
- NimBLE-Arduino: ^2.2.3 (BLE stack)
- ai-esp32-rotary-encoder: ^1.7 (encoder driver)
- Adafruit SSD1306: ^2.5.15 (OLED driver)
- ESP32-BLE-Keyboard: ShocKwav3 fork (HID library)

### Build Configuration
- Use `env:use_nimble` environment (preferred)
- Monitor speed: 460800 baud
- USB CDC enabled on boot

### Hardware Constraints
- Single-core RISC-V @ 160 MHz - avoid blocking operations
- 400 KB SRAM - minimize dynamic allocation
- GPIO limitations: I2C on 6/7, encoder on 0/1/2

---

## Critical Implementation Rules

### C++/Arduino Language Rules

- **Header Guards:** Always use `#pragma once`, never traditional `#ifndef` guards
- **Error Returns:** All fallible operations MUST return `Error` enum, not bool or void
- **Enum Storage:** Store enums as `uint8_t` in NVS with validation wrapper for corruption handling
- **Static Objects:** Declare long-lived objects as `static` in setup() - prevents stack overflow
- **No Dynamic Allocation:** Avoid `new`/`malloc` in runtime code - use `constexpr` and static arrays
- **Explicit Constructors:** Use `explicit` keyword for single-argument constructors
- **Virtual Destructors:** All base classes with virtual methods MUST have virtual destructor

### Event Architecture Rules

- **Event Flow:** Input → Dispatcher → Queue → Handler (NEVER bypass the queue)
- **Queue Sizes:** Use 10 items for event queues (established pattern)
- **Handler Registration:** Register handlers with Manager before use in setup()
- **Menu Interception:** Menu system intercepts events BEFORE mode handlers when active
- **Non-Blocking:** Never block in event handlers - use queues for async work
- **Event Payloads:** Use union-based struct for AppEvent data - access correct union member based on event type (wrong member = undefined behavior)
- **Zero-Initialize:** Always zero-initialize event structs before populating fields
- **FreeRTOS Queue API:** Use `xQueueSend()` in task context, always check return value (`pdPASS` = success)
- **Ownership Boundary:** Dispatcher owns event emission, Handler owns event processing - handlers emit via injected dispatcher, never directly to queue

### Handler Pattern Rules

- **Interface First:** All handlers implement abstract interface (e.g., `EncoderModeHandlerInterface`)
- **Single Responsibility:** One handler per mode/behavior
- **Dispatcher Injection:** Pass dispatcher pointer to handler constructor - enables mock testing
- **State in Handler:** Handlers may hold state, but must be re-entrant safe

### Testing Event Architecture

**Strategic Pivot (2025-12-18):** Automated test implementation is **deferred** for the MVP phase.

- **Mock Dispatchers:** Inject `MockDispatcher*` for unit tests instead of real dispatcher (Deferred)
- **Direct Handler Testing:** Test handlers by calling methods directly - queues are integration-level concern (Deferred)
- **Event Tracing:** Use `LOG_DEBUG` to trace event flow through dispatcher → queue → handler chain

### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Files | PascalCase matching class | `MenuController.cpp` |
| Classes/Structs | PascalCase | `MenuController`, `MenuItem` |
| Functions/Methods | camelCase | `handleEvent()`, `saveConfig()` |
| Variables | camelCase | `currentItem`, `menuActive` |
| Constants | UPPER_SNAKE_CASE | `MAX_MENU_DEPTH`, `BUTTON_COUNT` |
| Enums | PascalCase enum, UPPER_SNAKE values | `enum class Error { OK, NVS_WRITE_FAIL }` |
| Interfaces | Suffix with `Interface` | `DisplayInterface` |
| Macros | UPPER_SNAKE_CASE | `LOG_DEBUG`, `LOG_ERROR` |

**Anti-Patterns (DO NOT DO):**
```cpp
// ❌ WRONG
class set_wheel_mode_action    // Should be PascalCase
src/set_wheel_mode.cpp         // Should match class name, wrong location
void HandleEvent()             // Should be camelCase
#define maxDepth 5             // Should be UPPER_SNAKE_CASE

// ✅ CORRECT
class SelectWheelModeAction
src/Menu/Action/SelectWheelModeAction.cpp
void handleEvent()
constexpr uint8_t MAX_DEPTH = 5;
```

### Constants Rule

- **Prefer `constexpr` over `#define`** for new constants - provides type safety
- Use `#define` only for conditional compilation (`#ifdef`)

### Include Order

1. Matching header (for `.cpp` files)
2. C standard library (`<stdint.h>`, `<string.h>`)
3. Arduino/ESP32 headers (`"Wire.h"`, `"Arduino.h"`)
4. Third-party libraries (`"BleKeyboard.h"`, `"Adafruit_SSD1306.h"`)
5. Project headers - alphabetically (`"Config/device_config.h"`, `"Event/Dispatcher/..."`)

### Directory Structure - File Placement Decision Tree

**New handler?** → `src/<Domain>/Handler/<HandlerName>.cpp/.h`
**New interface?** → `src/<Domain>/Interface/<InterfaceName>.h`
**New implementation of interface?** → `src/<Domain>/Impl/<ImplName>.cpp/.h`
**New data model/struct?** → `src/<Domain>/Model/<ModelName>.h`
**New enum?** → `include/Enum/<EnumName>Enum.h`
**New config constants?** → `include/Config/<domain>_config.h`
**New event type?** → `include/Type/AppEvent.h` (extend existing)

### Logging Rules

- **NEVER use `Serial.print`** in new code - Always use `LOG_*` macros
- **Log levels:** `LOG_ERROR`, `LOG_INFO`, `LOG_DEBUG`
- **Format:** `LOG_ERROR("Tag", "Format %d", value)` → `[ERR][Tag] Format 123`
- **Build control:** Log level via build flag (`LOG_LEVEL_NONE`/`ERROR`/`INFO`/`DEBUG`)

### Test File Naming (Deferred)

**Strategic Pivot (2025-12-18):** Automated test implementation is **deferred** for the MVP phase.

- **Test files:** `test_<ClassName>.cpp` in `test/unit/`
- **Mock files:** `Mock<InterfaceName>.h` in `test/mocks/`
- **Test functions:** `test_<methodName>_<scenario>()` pattern

```
test/
├── unit/
│   ├── test_ConfigManager.cpp
│   └── test_MenuController.cpp
└── mocks/
    ├── MockDisplay.h
    └── MockPreferences.h
```

---

## Critical Don't-Miss Rules

### Anti-Patterns to AVOID

```cpp
// ❌ NEVER bypass event queue
handler->handleEvent(event);  // Wrong - must go through queue
xQueueSend(queue, &event, portMAX_DELAY);  // Correct

// ❌ NEVER block in handlers
delay(100);  // Wrong - blocks entire event loop

// ❌ NEVER use dynamic allocation at runtime
MenuItem* item = new MenuItem();  // Wrong
static MenuItem item;  // Correct - or constexpr

// ❌ NEVER access wrong union member
AppEvent event;
event.type = MENU_ACTIVATED;
event.data.config.value = 5;  // Wrong - should use event.data.state

// ❌ NEVER send HID without checking connection
bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);  // Wrong
if (bleKeyboard.isConnected()) {  // Correct
    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
}
```

### ESP32-C3 Specific Gotchas

- **Single-core:** No parallel task execution - everything is cooperative
- **Stack size:** Default task stack is limited - avoid deep recursion
- **NVS writes:** Slow (~20ms) - never call in tight loops or event handlers
- **GPIO 8/9:** Used for USB - avoid for buttons if USB CDC enabled
- **BLE advertising:** Blocks briefly - don't call from time-critical code
- **Static initialization:** Never access hardware/peripherals in static constructors - only in `setup()`

### BLE HID Gotchas

- **Always check `bleKeyboard.isConnected()`** before any HID operation
- Sending to disconnected device causes undefined behavior

### Input Handling Gotchas

- **Don't add debounce logic** - EncoderDriver already handles debouncing
- **Queue full scenario:** Use timeout instead of `portMAX_DELAY` for inputs:
  ```cpp
  xQueueSend(queue, &event, pdMS_TO_TICKS(10));  // 10ms timeout
  // Log if send fails - event was dropped
  ```

### Menu System Gotchas

- **Menu intercept order:** Check `menuController.isActive()` BEFORE passing to mode handler
- **Exit on config change:** Menu auto-exits after selection - don't manually exit twice
- **Long press from root:** Exits menu entirely - handle this state transition

### Configuration Persistence Gotchas

- **NVS namespace:** Use single namespace `"knobkoky"` with prefixed keys
- **Enum corruption:** Always validate stored enum values against max valid
- **First boot:** Check for uninitialized NVS and apply defaults
- **Factory reset:** Hold encoder button 5+ seconds on boot triggers reset
- **Check write return values:** `putUChar()` can silently fail if NVS full - always verify

### Memory Gotchas

- **Avoid Arduino `String` class** - causes heap fragmentation
- **Use char arrays** with fixed-size buffers instead

### Testing Gotchas (Deferred)

- **Reset static/global state** in each test - embedded state persists between tests

---

## Usage Guidelines

**For AI Agents:**
- Read this file before implementing any code
- Follow ALL rules exactly as documented
- When in doubt, prefer the more restrictive option
- Reference architecture.md for detailed design decisions

**For Humans:**
- Keep this file lean and focused on agent needs
- Update when technology stack or patterns change
- Review periodically for outdated rules
- Remove rules that become obvious over time

---

_Last Updated: 2025-12-16_

