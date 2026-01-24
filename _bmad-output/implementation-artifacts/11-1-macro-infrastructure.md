# Story 11.1: Macro Infrastructure (Types, Manager, NVS)

Status: backlog

## Story

As a **developer building the macro system**,
I want **the foundational types, MacroManager, and NVS persistence in place**,
So that **macro definitions can be stored, loaded, and executed**.

## Acceptance Criteria

1. **Given** the macro infrastructure is implemented
   **When** the system initializes
   **Then** MacroManager loads all 7 macro definitions from NVS
   **And** empty macros (0x0000) are treated as "no macro assigned"
   **And** MacroDefinition correctly packs/unpacks modifiers + keycode

2. **Given** a macro is saved via ConfigManager
   **When** the device reboots
   **Then** the macro persists and is loaded correctly
   **And** NVS keys follow pattern "macro.0" through "macro.6"

3. **Given** MacroManager.executeMacro() is called with a valid macro
   **When** BLE is connected
   **Then** the key combination is sent via BleKeyboard
   **And** returns true indicating macro was executed

4. **Given** MacroManager.executeMacro() is called
   **When** BLE is NOT connected
   **Then** the macro is not sent
   **And** returns false (graceful skip, not error)

5. **Given** the build is run
   **When** compilation completes
   **Then** build succeeds with no errors or warnings
   **And** all new files compile without issues

## Tasks

- [ ] **Task 1: Create Macro Configuration Header** (AC: 1, 5)
  - [ ] Create `include/Config/macro_config.h`:
    - [ ] Define `MACRO_BUTTON_PIN` = 10
    - [ ] Define `MACRO_BUTTON_ACTIVE_LOW` = true
    - [ ] Define `MACRO_INPUT_COUNT` = 7
    - [ ] Add header guard and comments

- [ ] **Task 2: Create Macro Input Enum** (AC: 1, 5)
  - [ ] Create `include/Enum/MacroInputEnum.h`:
    - [ ] Enum class `MacroInput : uint8_t`
    - [ ] Values: WHEEL_BUTTON=0, WHEEL_LEFT=1, WHEEL_RIGHT=2, BUTTON_1=3, BUTTON_2=4, BUTTON_3=5, BUTTON_4=6
    - [ ] Verify enum matches MACRO_INPUT_COUNT

- [ ] **Task 3: Create MacroDefinition Struct** (AC: 1, 2, 5)
  - [ ] Create `include/Type/MacroDefinition.h`:
    - [ ] Struct with uint8_t modifiers, uint8_t keycode
    - [ ] Method `isEmpty()` returns true if both are 0
    - [ ] Method `toPacked()` returns (modifiers << 8) | keycode as uint16_t
    - [ ] Static method `fromPacked(uint16_t)` unpacks to MacroDefinition

- [ ] **Task 4: Extend ConfigManager with NVS Methods** (AC: 2, 5)
  - [ ] Update `src/Config/ConfigManager.h`:
    - [ ] Add method `Error loadMacro(uint8_t index, MacroDefinition& out)`
    - [ ] Add method `Error saveMacro(uint8_t index, uint16_t packed)`
    - [ ] Add validation check for index < MACRO_INPUT_COUNT
  - [ ] Update `src/Config/ConfigManager.cpp`:
    - [ ] Implement loadMacro() reading from "macro.{index}" NVS key
    - [ ] Return Error::NOT_FOUND if key doesn't exist, Error::OK if loaded
    - [ ] Implement saveMacro() writing to "macro.{index}" NVS key
    - [ ] Use existing preferences object in "knobkoky" namespace

- [ ] **Task 5: Create MacroManager Class** (AC: 1, 3, 4, 5)
  - [ ] Create `src/Macro/Manager/MacroManager.h`:
    - [ ] Class MacroManager with private members:
      - [ ] `BleKeyboard* bleKeyboard` (injected)
      - [ ] `bool macroButtonHeld` (state)
      - [ ] `MacroDefinition macros[MACRO_INPUT_COUNT]` (static array)
    - [ ] Public methods:
      - [ ] Constructor `MacroManager(BleKeyboard* ble)`
      - [ ] `void setMacroButtonState(bool held)`
      - [ ] `bool isMacroModeActive() const`
      - [ ] `bool executeMacro(MacroInput input)`
      - [ ] `void loadFromNVS(ConfigManager& config)`
      - [ ] `Error saveMacro(ConfigManager& config, MacroInput input, MacroDefinition macro)`
  - [ ] Create `src/Macro/Manager/MacroManager.cpp`:
    - [ ] Implement `loadFromNVS()` loading all 7 macros
    - [ ] Implement `setMacroButtonState()` updating boolean
    - [ ] Implement `isMacroModeActive()` returning current state
    - [ ] Implement `executeMacro()`:
      - [ ] Check if macro is empty, return false if so
      - [ ] Check if BLE connected, return false if not
      - [ ] Send key combination via BleKeyboard
      - [ ] Return true on success
    - [ ] Implement `saveMacro()`:
      - [ ] Validate index < MACRO_INPUT_COUNT
      - [ ] Update local array element
      - [ ] Call ConfigManager.saveMacro()
      - [ ] Return Error result

- [ ] **Task 6: Add Logging** (AC: 1, 3, 4, 5)
  - [ ] In MacroManager constructor:
    - [ ] Log "MacroManager initialized" with LOG_INFO
  - [ ] In loadFromNVS():
    - [ ] Log loaded macro count with LOG_DEBUG
  - [ ] In executeMacro():
    - [ ] Log macro execution with LOG_DEBUG
    - [ ] Log BLE disconnect skip with LOG_DEBUG
  - [ ] All logging uses project LOG_* macros

- [ ] **Task 7: Build and Verify** (AC: 5)
  - [ ] Compile with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Verify no warnings
  - [ ] Manual test: Load macros from NVS
  - [ ] Manual test: Save macro, reboot, verify loaded

## Dev Notes

### Architecture Compliance

- **Enum Naming**: MacroInput enum with 7 values (0-6 matching array indices)
- **Static Allocation**: MacroDefinition array[7] - no dynamic allocation
- **Dependency Injection**: BleKeyboard passed via constructor
- **Error Handling**: Return Error enum from NVS operations per project standard
- **Logging**: Use LOG_DEBUG/LOG_INFO macros, never Serial.print

### Data Structure

```cpp
// Packed Format (uint16_t):
// High byte: HID modifier bitmask (KEY_LEFT_CTRL, etc.)
// Low byte: HID keycode
// 0x0000 = empty/no macro

// Example: Ctrl+C = (KEY_LEFT_CTRL << 8) | 'C'
// Example: Ctrl+Shift+V = (modifier_combo << 8) | 'V'
```

### NVS Storage Pattern

```
Namespace: "knobkoky" (existing)
Keys: "macro.0", "macro.1", ..., "macro.6"
Type: uint16_t (packed format)
Default: 0x0000 if key not found
```

### MacroManager Usage

```cpp
MacroManager macroManager(&bleKeyboard);
macroManager.loadFromNVS(configManager);

// Macro button held/released
macroManager.setMacroButtonState(true);

// Check if macro mode active
if (macroManager.isMacroModeActive()) {
    macroManager.executeMacro(MacroInput::WHEEL_LEFT);
}

macroManager.setMacroButtonState(false);

// Save new macro
MacroDefinition newMacro{KEY_LEFT_CTRL, 'C'};
Error err = macroManager.saveMacro(configManager, MacroInput::WHEEL_BUTTON, newMacro);
```

### Files to Create

```
include/Config/macro_config.h
include/Enum/MacroInputEnum.h
include/Type/MacroDefinition.h
src/Macro/Manager/MacroManager.h
src/Macro/Manager/MacroManager.cpp
```

### Files to Modify

```
src/Config/ConfigManager.h (add methods)
src/Config/ConfigManager.cpp (implement methods)
```

### Key Design Decisions

- Macros stored as packed uint16_t for compact NVS storage
- Empty macro = 0x0000 (zero check for isEmpty)
- No complex validation at runtime - assume valid HID codes
- Graceful skip if BLE not connected (not an error)
- All 7 slots always loaded (even if empty)

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Dynamic allocation
MacroDefinition* macros = new MacroDefinition[7];

// ❌ WRONG - String keys
config.saveMacro("wheel_button", value);  // Use "macro.0" pattern

// ❌ WRONG - No BLE check
bleKeyboard->press(keycode);  // Must check isConnected() first

// ❌ WRONG - Redefining modifier constants
#define CTRL 0x01  // Use BleKeyboard's KEY_LEFT_CTRL

// ✅ CORRECT
MacroDefinition macros[MACRO_INPUT_COUNT];  // Static allocation
config.saveMacro(0, packed);  // Numeric key index
if (bleKeyboard->isConnected()) { ... }  // Check first
```

### References

- Macro architecture: `_bmad-output/architecture/macro-feature-architecture.md`
- BleKeyboard modifier constants from library
- Project logging: `include/Logging/LogMacros.h`
- ConfigManager NVS pattern: `src/Config/ConfigManager.cpp`

## Dev Agent Record

Status: backlog - ready for implementation

**Story 11.1 covers:** Types (MacroDefinition, MacroInputEnum), MacroManager core class, NVS read/write integration.

**Implementation Sequence:** Create types → Extend ConfigManager → Create MacroManager → Build & verify

**Key Points:**
- Static allocation throughout (no heap)
- Packed format (uint16_t) for compact storage
- 7 macro slots (MACRO_INPUT_COUNT)
- GPIO 10 reserved for macro button
- Integration with existing BleKeyboard + ConfigManager

---
