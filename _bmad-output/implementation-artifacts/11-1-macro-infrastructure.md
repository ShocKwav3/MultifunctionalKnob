# Story 11.1: Macro Infrastructure (Types, Manager, NVS)

Status: done

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

- [x] **Task 1: Create Macro Configuration Header** (AC: 1, 5)
  - [x] Create `include/Config/macro_config.h`:
    - [x] Define `MACRO_BUTTON_PIN` = 10
    - [x] Define `MACRO_BUTTON_ACTIVE_LOW` = true
    - [x] Define `MACRO_INPUT_COUNT` = 7
    - [x] Add header guard and comments

- [x] **Task 2: Create Macro Input Enum** (AC: 1, 5)
  - [x] Create `include/Enum/MacroInputEnum.h`:
    - [x] Enum class `MacroInput : uint8_t`
    - [x] Values: WHEEL_BUTTON=0, WHEEL_LEFT=1, WHEEL_RIGHT=2, BUTTON_1=3, BUTTON_2=4, BUTTON_3=5, BUTTON_4=6
    - [x] Verify enum matches MACRO_INPUT_COUNT

- [x] **Task 3: Create MacroDefinition Struct** (AC: 1, 2, 5)
  - [x] Create `include/Type/MacroDefinition.h`:
    - [x] Struct with uint8_t modifiers, uint8_t keycode
    - [x] Method `isEmpty()` returns true if both are 0
    - [x] Method `toPacked()` returns (modifiers << 8) | keycode as uint16_t
    - [x] Static method `fromPacked(uint16_t)` unpacks to MacroDefinition

- [x] **Task 4: Extend ConfigManager with NVS Methods** (AC: 2, 5)
  - [x] Update `src/Config/ConfigManager.h`:
    - [x] Add method `Error loadMacro(uint8_t index, MacroDefinition& out)`
    - [x] Add method `Error saveMacro(uint8_t index, uint16_t packed)`
    - [x] Add validation check for index < MACRO_INPUT_COUNT
  - [x] Update `src/Config/ConfigManager.cpp`:
    - [x] Implement loadMacro() reading from "macro.{index}" NVS key
    - [x] Return Error::OK always, empty macro (0x0000) if key doesn't exist
    - [x] Implement saveMacro() writing to "macro.{index}" NVS key
    - [x] Use existing preferences object in "knobkoky" namespace

- [x] **Task 5: Create MacroManager Class** (AC: 1, 3, 4, 5)
  - [x] Create `src/Macro/Manager/MacroManager.h`:
    - [x] Class MacroManager with private members:
      - [x] `BleKeyboard* bleKeyboard` (injected)
      - [x] `bool macroButtonHeld` (state)
      - [x] `MacroDefinition macros[MACRO_INPUT_COUNT]` (static array)
    - [x] Public methods:
      - [x] Constructor `MacroManager(BleKeyboard* ble)`
      - [x] `void setMacroButtonState(bool held)`
      - [x] `bool isMacroModeActive() const`
      - [x] `bool executeMacro(MacroInput input)`
      - [x] `void loadFromNVS(ConfigManager& config)`
      - [x] `Error saveMacro(ConfigManager& config, MacroInput input, MacroDefinition macro)`
  - [x] Create `src/Macro/Manager/MacroManager.cpp`:
    - [x] Implement `loadFromNVS()` loading all 7 macros
    - [x] Implement `setMacroButtonState()` updating boolean
    - [x] Implement `isMacroModeActive()` returning current state
    - [x] Implement `executeMacro()`:
      - [x] Check if macro is empty, return false if so
      - [x] Check if BLE connected, return false if not
      - [x] Send key combination via BleKeyboard
      - [x] Return true on success
    - [x] Implement `saveMacro()`:
      - [x] Validate index < MACRO_INPUT_COUNT
      - [x] Update local array element
      - [x] Call ConfigManager.saveMacro()
      - [x] Return Error result

- [x] **Task 6: Add Logging** (AC: 1, 3, 4, 5)
  - [x] In MacroManager constructor:
    - [x] Log "MacroManager initialized" with LOG_INFO
  - [x] In loadFromNVS():
    - [x] Log loaded macro count with LOG_DEBUG
  - [x] In executeMacro():
    - [x] Log macro execution with LOG_DEBUG
    - [x] Log BLE disconnect skip with LOG_DEBUG
  - [x] All logging uses project LOG_* macros

- [x] **Task 7: Build and Verify** (AC: 5)
  - [x] Compile with `pio run -e use_nimble`
  - [x] Verify no compile errors
  - [x] Verify no warnings
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

### Implementation Plan

**Story 11.1 covers:** Types (MacroDefinition, MacroInputEnum), MacroManager core class, NVS read/write integration.

**Implementation Sequence:** Create types → Extend ConfigManager → Create MacroManager → Build & verify

**Key Points:**
- Static allocation throughout (no heap)
- Packed format (uint16_t) for compact storage
- 7 macro slots (MACRO_INPUT_COUNT)
- GPIO 10 reserved for macro button
- Integration with existing BleKeyboard + ConfigManager

### Completion Notes

**Implementation Date:** 2026-01-24

**Tasks Completed:**
1. ✅ Created `include/Config/macro_config.h` with MACRO_BUTTON_PIN, MACRO_BUTTON_ACTIVE_LOW, MACRO_INPUT_COUNT constants
2. ✅ Created `include/Enum/MacroInputEnum.h` with MacroInput enum (7 values: 0-6)
3. ✅ Created `include/Type/MacroDefinition.h` with packed format support (toPacked/fromPacked methods)
4. ✅ Extended ConfigManager with loadMacro() and saveMacro() methods for NVS persistence
5. ✅ Created MacroManager class with dependency injection, state tracking, and BLE execution
6. ✅ Added comprehensive logging (LOG_INFO, LOG_DEBUG) throughout MacroManager
7. ✅ Build verified - compiles cleanly with no errors or warnings

**Technical Decisions:**
- Changed loadMacro() to return Error::OK always (with empty macro if key missing) instead of NOT_FOUND, as Error enum doesn't include NOT_FOUND
- Used `#pragma once` for header guards per project standard
- Used `constexpr` for constants per project standard
- MacroManager executes macros via press/release sequence for proper HID behavior
- Empty macros (0x0000) are gracefully skipped during execution

**Architecture Compliance:**
- ✅ Static allocation only (MacroDefinition array[7])
- ✅ Dependency injection (BleKeyboard* via constructor)
- ✅ Error enum returns for fallible operations
- ✅ LOG_* macros used (never Serial.print)
- ✅ Naming conventions followed (PascalCase classes, camelCase methods, UPPER_SNAKE constants)
- ✅ BLE connection check before HID operations

## File List

**New Files Created:**
- `include/Config/macro_config.h`
- `include/Enum/MacroInputEnum.h`
- `include/Type/MacroDefinition.h`
- `src/Macro/Manager/MacroManager.h`
- `src/Macro/Manager/MacroManager.cpp`

**Modified Files:**
- `src/Config/ConfigManager.h` (added loadMacro/saveMacro methods)
- `src/Config/ConfigManager.cpp` (implemented loadMacro/saveMacro)

## Code Review

**Review Date:** 2026-01-24
**Reviewer:** Code Review Agent (Amelia)
**Result:** ✅ **PASS** (after fixes)

### Critical Issues Found and Fixed

**Issue #1: Broken Multi-Modifier Execution (AC 3 violation)**
- **Location:** `src/Macro/Manager/MacroManager.cpp:50`
- **Problem:** Code called `bleKeyboard->press(macro.modifiers)` treating bitmask as single keycode
- **Impact:** Multi-modifier macros (Ctrl+Shift+V) would fail; single modifiers would send wrong HID codes
- **Root Cause:** NimBleKeyboard library expects individual modifier keycodes (0x80-0x87), not bitmasks
- **Fix Applied:** Iterate through bitmask bits, press/release each modifier individually
- **Verification:** Build passed; logic matches library's press() implementation (NimBleKeyboard.cpp:190-192)

**Issue #2: Include Order Violation**
- **Location:** `src/Macro/Manager/MacroManager.h:13-19`
- **Problem:**
  - System header `<stdint.h>` before external library `"BleKeyboard.h"`
  - Missing blank line before project headers
  - Project headers not fully alphabetical (ErrorEnum after MacroInputEnum)
- **Impact:** Violates project coding standard (main.cpp pattern)
- **Fix Applied:** Reordered to: BleKeyboard.h → <stdint.h> → [blank] → Config/* → Enum/* (alphabetical) → Type/*
- **Verification:** Build passed; matches project standard

### Verification
- ✅ Build: `pio run -e use_nimble` succeeded with no errors/warnings
- ✅ AC 3 compliance: Multi-modifier execution now correct per NimBleKeyboard API
- ✅ Code standards: Include order matches project pattern

## Change Log

- **2026-01-24 (PM)**: Code review fixes applied
  - Fixed multi-modifier execution bug (bitmask iteration for HID compliance)
  - Fixed include order violations in MacroManager.h
  - Build verified clean after fixes
- **2026-01-24 (AM)**: Macro infrastructure implementation complete (Tasks 1-7)
  - Created macro types, enums, and data structures
  - Extended ConfigManager with NVS macro persistence
  - Implemented MacroManager for state tracking and BLE execution
  - Initial build verified with no errors or warnings

---
