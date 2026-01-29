# Story 11.4: System Integration (Initialization + Factory Reset)

Status: backlog

## Story

As a **user of the complete macro system**,
I want **macros to initialize on boot and clear on factory reset**,
So that **the feature works reliably and resets cleanly**.

## Acceptance Criteria

1. **Given** the device boots
   **When** initialization completes
   **Then** MacroManager is created with BleKeyboard dependency
   **And** MacroManager is injected into EncoderEventHandler
   **And** MacroManager is injected into ButtonEventHandler
   **And** all macros are loaded from NVS

2. **Given** the system is initialized
   **When** checking dependencies
   **Then** MacroManager references valid BleKeyboard instance
   **And** Event handlers have valid MacroManager references
   **And** No null pointers in signal chain

3. **Given** the user performs a factory reset (hold encoder button 5+ sec at boot)
   **When** the reset completes
   **Then** all macro NVS keys ("macro.0" through "macro.6") are cleared
   **And** macros revert to empty (no macro assigned)
   **And** Any non-macro configuration is NOT affected

4. **Given** macro button activity occurs
   **When** the inactivity timer is running
   **Then** the timer resets (consistent with other inputs)

5. **Given** the device reboots after configuration
   **When** MacroManager initializes
   **Then** all saved macros load from NVS
   **And** system is ready for use

6. **Given** the build is run
   **When** compilation completes
   **Then** build succeeds with no errors or warnings
   **And** no new warnings introduced

## Tasks

- [ ] **Task 1: Initialize MacroManager in main.cpp** (AC: 1, 2, 6)
  - [ ] Update `src/main.cpp`:
    - [ ] Add include: `#include "Macro/Manager/MacroManager.h"`
    - [ ] Add include: `#include "Config/macro_config.h"`
    - [ ] In setup() or global scope, instantiate:
      - [ ] `MacroManager macroManager(&bleKeyboard);`
    - [ ] After instantiation, load macros:
      - [ ] `macroManager.loadFromNVS(configManager);`
    - [ ] Log initialization: `LOG_INFO("System", "MacroManager initialized")`

- [ ] **Task 2: Inject HardwareState and MacroManager into EncoderEventHandler** (AC: 1, 2)
  - [ ] Update `src/Event/Handler/EncoderEventHandler.h`:
    - [ ] Add constructor parameter: `HardwareState* hw`
    - [ ] Add constructor parameter: `MacroManager* macroManager`
    - [ ] Add member: `HardwareState* hardwareState;`
    - [ ] Add member: `MacroManager* macroManager;`
  - [ ] Update `src/Event/Handler/EncoderEventHandler.cpp`:
    - [ ] Constructor: store both pointers
      - [ ] `this->hardwareState = hw;`
      - [ ] `this->macroManager = macroManager;`
  - [ ] In `src/main.cpp`:
    - [ ] When creating EncoderEventHandler:
      - [ ] Pass both dependencies: `encoderEventHandler = new EncoderEventHandler(dispatcher, &hardwareState, &macroManager);`

- [ ] **Task 3: Inject HardwareState and MacroManager into ButtonEventHandler** (AC: 1, 2)
  - [ ] Update `src/Event/Handler/ButtonEventHandler.h`:
    - [ ] Add constructor parameter: `HardwareState* hw`
    - [ ] Add constructor parameter: `MacroManager* macroManager`
    - [ ] Add member: `HardwareState* hardwareState;`
    - [ ] Add member: `MacroManager* macroManager;`
  - [ ] Update `src/Event/Handler/ButtonEventHandler.cpp`:
    - [ ] Constructor: store both pointers
      - [ ] `this->hardwareState = hw;`
      - [ ] `this->macroManager = macroManager;`
  - [ ] In `src/main.cpp`:
    - [ ] When creating ButtonEventHandler:
      - [ ] Pass both dependencies: `buttonEventHandler = new ButtonEventHandler(dispatcher, config, bleService, powerManager, &hardwareState, &macroManager);`

- [ ] **Task 4: Update FactoryReset to Clear Macros** (AC: 3, 6)
  - [ ] Update `src/Config/FactoryReset.cpp`:
    - [ ] Find factory reset function (likely `performFactoryReset()` or similar)
    - [ ] Add macro clearing after existing resets:
      - [ ] Loop through indices 0-6:
        - [ ] Call `configManager.remove("macro.0")`
        - [ ] Call `configManager.remove("macro.1")`
        - [ ] ... through "macro.6"
      - [ ] Or: `Preferences.remove("macro.0")` etc. directly
    - [ ] Log: `LOG_INFO("FactoryReset", "Cleared macro configurations")`
  - [ ] Verify other configs NOT affected:
    - [ ] BLE pairing info preserved
    - [ ] Menu settings preserved
    - [ ] Wheel mode preserved
    - [ ] Only macro keys removed

- [ ] **Task 5: Integrate Macro Button with Inactivity Timer** (AC: 4)
  - [ ] Find inactivity timer integration (PowerManager or similar)
  - [ ] Check ButtonEventHandler handling of regular buttons:
    - [ ] Verify they call `powerManager->resetActivity()` (or equivalent)
  - [ ] Ensure macro button ALSO resets timer:
    - [ ] In ButtonEventHandler:
      - [ ] Even though macro button doesn't trigger normal action
      - [ ] Still call activity reset: `powerManager->resetActivity()`
    - [ ] Example flow:
      - [ ] `if (event.pin == MACRO_BUTTON_PIN) { macroManager->setMacroButtonState(...); powerManager->resetActivity(); return; }`

- [ ] **Task 6: Verify Dependency Injection Chain** (AC: 2, 6)
  - [ ] Create mental map of dependency flow:
    - [ ] main.cpp instantiates MacroManager
    - [ ] main.cpp instantiates handlers, passes MacroManager reference
    - [ ] Handlers store and use MacroManager
    - [ ] Verify no circular dependencies
    - [ ] Verify all pointers are valid at initialization
  - [ ] Add validation logging in main.cpp:
    - [ ] After all initialization: `if (!macroManager || !handler) { LOG_ERROR(...); return; }`

- [ ] **Task 7: Test Factory Reset** (AC: 3)
  - [ ] Manual test procedure:
    - [ ] Configure a macro: "Wheel Button: Ctrl+C"
    - [ ] Reboot to verify persistence
    - [ ] Perform factory reset (hold wheel button 5+ sec at boot)
    - [ ] Check Settings > Macros
    - [ ] Verify all slots show "None"
    - [ ] Verify other settings intact (wheel mode, BLE pairing, etc.)

- [ ] **Task 8: Build and Verify Initialization** (AC: 1, 2, 5, 6)
  - [ ] Compile with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Verify no new warnings
  - [ ] Check serial logs at boot:
    - [ ] "MacroManager initialized"
    - [ ] All macro keys loaded (or empty)
  - [ ] Manual test: Configure macro, reboot, verify loaded
  - [ ] Test factory reset sequence complete

## Dev Notes

### Initialization Order

```
setup():
  1. BleKeyboard init
  2. Event system init (dispatchers, queues)
  3. MenuController init
  4. ConfigManager init (NVS preferences)
  5. PowerManager init (inactivity timer)
  ──────────────────────────────────
  6. MacroManager init
  7. Load all macros from NVS
  8. Inject into EncoderEventHandler
  9. Inject into ButtonEventHandler
  ──────────────────────────────────
  10. Start all tasks (handlers, display, etc.)
```

### Macro Clearing in Factory Reset

```cpp
void performFactoryReset() {
    // Clear existing configs
    configManager.clearWheelMode();
    configManager.clearBleConfig();
    // ... other clears ...

    // Clear macros (NEW)
    for (uint8_t i = 0; i < MACRO_INPUT_COUNT; i++) {
        String key = "macro." + String(i);
        preferences.remove(key);
    }

    LOG_INFO("FactoryReset", "Cleared macro configurations");
}
```

### Dependency Injection Pattern

```cpp
// src/main.cpp
// HardwareState is global and initialized early (already in place)
extern HardwareState hardwareState;

// Initialize macro system
MacroManager macroManager(&bleKeyboard);
macroManager.loadFromNVS(configManager);

// Initialize hardwareState macro flag
hardwareState.macroModeActive = false;  // Start inactive

// Create handlers with both HardwareState and MacroManager
EncoderEventHandler encoderHandler(dispatcher, &hardwareState, &macroManager);
ButtonEventHandler buttonHandler(eventQueue, configManager, bleKeyboardService, powerManager, &hardwareState, &macroManager);

// Handlers now have:
// - Read access to HardwareState for checking macro mode
// - Access to MacroManager for executing macros and toggling mode
// - HardwareState is the single source of truth for macro mode state
```

### Inactivity Timer Integration

```cpp
// In ButtonEventHandler::handleEvent()
if (event.pin == MACRO_BUTTON_PIN) {
    macroManager->setMacroButtonState(event.pressed);
    powerManager->resetActivity();  // Reset timer on macro button too
    return;
}

// Then continue with normal button handling (which also resets)
powerManager->resetActivity();  // Reset on all user input
```

### Files to Modify

```
src/main.cpp - MacroManager instantiation and injection
src/Event/Handler/EncoderEventHandler.h/.cpp - Add MacroManager member
src/Event/Handler/ButtonEventHandler.h/.cpp - Add MacroManager member
src/Config/FactoryReset.cpp - Clear macro NVS keys
```

### Key Design Decisions

- **HardwareState is the single source of truth** for macro mode state (not MacroManager)
- Handlers get HardwareState injected to check macro mode flag
- After toggle, handlers sync HardwareState immediately
- Inject both HardwareState and MacroManager via constructor (not global references)
- Load all macros immediately after instantiation
- Initialize hardwareState.macroModeActive = false at boot
- Clear macros on factory reset (not partial)
- Macro button also resets inactivity timer (consistency)
- All logging follows project standards

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Storing macro mode state in MacroManager only
class MacroManager {
    bool macroModeActive;  // This belongs in HardwareState!
};
// Problem: Handlers can't see state without calling getter

// ❌ WRONG - Passing only MacroManager to handlers
EncoderEventHandler handler(dispatcher, &macroManager);
// Problem: No way for handler to check macro state or for display to show it

// ❌ WRONG - Not syncing HardwareState after toggling
macroManager->toggleMacroMode();
// Missing: hardwareState->macroModeActive = macroManager->isMacroModeActive();

// ❌ WRONG - Checking MacroManager state instead of HardwareState
if (macroManager->isMacroModeActive()) {  // Don't use this in handlers
    macroManager->executeMacro(input);
}

// ❌ WRONG - Global static MacroManager
static MacroManager macroManager(&bleKeyboard);

// ❌ WRONG - Null pointer check after injection
EncoderEventHandler handler(nullptr);  // Never pass null

// ❌ WRONG - Clearing non-macro config on factory reset
configManager.clearAllSettings();  // Too broad, only clear macro keys

// ❌ WRONG - Forgetting to initialize hardwareState macro flag
// Don't leave it uninitialized at boot

// ✅ CORRECT - HardwareState is source of truth
extern HardwareState hardwareState;  // Global (initialized early)

MacroManager macroManager(&bleKeyboard);
macroManager.loadFromNVS(configManager);
hardwareState.macroModeActive = false;  // Initialize state

// Inject both dependencies
EncoderEventHandler encoderHandler(dispatcher, &hardwareState, &macroManager);
ButtonEventHandler buttonHandler(eventQueue, config, ble, pm, &hardwareState, &macroManager);

// In handlers:
if (hardwareState->macroModeActive) {  // Read from HardwareState
    if (macroManager->executeMacro(input)) {
        return;
    }
}

// After toggling:
macroManager->toggleMacroMode();
hardwareState->macroModeActive = macroManager->isMacroModeActive();  // Sync state

// In FactoryReset:
for (uint8_t i = 0; i < MACRO_INPUT_COUNT; i++) {
    preferences.remove("macro." + String(i));
}
```

### Testing Approach

1. **Initialization Test**:
   - Boot device
   - Check serial logs for "MacroManager initialized"
   - Verify no hang or crash

2. **Dependency Test**:
   - Boot with configured macros
   - Use a macro (hold macro button, press input)
   - Verify macro executes (check BLE output)

3. **Persistence Test**:
   - Configure macro
   - Reboot device
   - Check menu: macro should still be there

4. **Factory Reset Test**:
   - Configure multiple macros
   - Perform factory reset
   - Check all macros cleared
   - Verify other settings intact (wheel mode, etc.)

5. **Inactivity Integration Test**:
   - Set inactivity timer to short interval (for testing)
   - Hold macro button continuously
   - Verify timer doesn't fire (button resets it)
   - Release macro button
   - Verify timer resumes and fires normally

## Dev Agent Record

Status: backlog - ready for implementation

**Story 11.4 covers:** main.cpp initialization, dependency injection to handlers, HardwareState setup, factory reset integration, inactivity timer integration.

**Key Points:**
- HardwareState is global and initialized early (pre-existing)
- Initialize hardwareState.macroModeActive = false at boot
- MacroManager instantiated in setup() before handlers
- Both handlers receive BOTH HardwareState and MacroManager references via constructor
- Handlers read macro state from HardwareState, not MacroManager
- After macro toggle, handlers sync HardwareState immediately
- Factory reset selectively clears only macro keys
- Macro button triggers inactivity timer reset like other inputs
- All initialization logged for debugging

**Critical Architectural Detail:**
- HardwareState is the single source of truth for macro mode
- This makes macro mode visible to all modules (display, other handlers, etc.)
- No need for MacroManager getters to expose internal state
- Simpler, more consistent with existing hardware state pattern

**Dependencies:** Requires Stories 11.1, 11.2, and 11.3 (all prior stories complete)

**Final Step:** After this story, Epic 11 is complete and macros are fully functional end-to-end.

---
