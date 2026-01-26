---
epicNumber: 11
title: "Macro Activation System"
complexity: Low
dependencies: "None (uses existing BLE, menu, NVS infrastructure)"
frsCovered: "FR-1, FR-2, FR-3, FR-4, FR-5, FR-6"
nfrsCovered: "NFR-1, NFR-2, NFR-3"
stories: 4
sourceDocuments:
  - "_bmad-output/analysis/macro-feature-requirement-summary.md"
  - "_bmad-output/architecture/macro-feature-architecture.md"
---

# Epic 11: Macro Activation System

**Goal:** Enable users to toggle macro mode on/off via long-press of a dedicated macro button, allowing key combination macros to be triggered on existing inputs.

**FRs covered:** FR-10 (toggle activation), FR-11 (mode execution), FR-12 (state persistence)

**NFRs covered:** NFR-1 (no latency), NFR-2 (memory constraints), NFR-3 (pattern consistency)

**Story Status Update:**
- Story 11.1: ✅ DONE (infrastructure complete; pending toggle interface refactoring)
- Story 11.2: 📋 READY-FOR-DEV (updated for long-press toggle behavior)
- Story 11.3: 📋 READY-FOR-DEV (menu config; no changes from toggle behavior)
- Story 11.4: 📋 READY-FOR-DEV (system integration; simplified by toggle pattern)

**User Outcome:** User holds macro button, presses any input (wheel button/rotation, buttons 1-4), and a pre-configured key combination (e.g., Ctrl+C) is sent via BLE. Macros are configured through the menu and persist across power cycles.

**Implementation Notes:** Uses GPIO 10 for macro button. Macro mode intercepts events after menu but before normal mode handlers. See architecture document for complete patterns.

---

## Functional Requirements

- FR-1: New macro button (GPIO 10) enables toggle-based macro mode activation
- FR-2: 7 configurable macro slots (wheel button, wheel left/right, buttons 1-4)
- FR-3: Macros are key combinations (modifier keys + keycode)
- FR-4: Macros persist to NVS across power cycles
- FR-5: Menu-based configuration for each macro slot
- FR-6: Long-press and release macro button activates macro mode; inputs execute assigned macros. Long-press again deactivates macro mode; inputs return to normal behavior

## Non-Functional Requirements

- NFR-1: No additional latency on normal input path when macro not active
- NFR-2: Memory footprint within ESP32-C3 constraints (static allocation)
- NFR-3: Consistent with existing event-driven architecture patterns

---

## Story 11.1: Macro Infrastructure (Types, Manager, NVS)

As a **developer building the macro system**,
I want **the foundational types, MacroManager, and NVS persistence in place**,
So that **macro definitions can be stored, loaded, and executed**.

**Scope:**
- `include/Config/macro_config.h` - GPIO 10 definition
- `include/Enum/MacroInputEnum.h` - 7 input slots enum
- `include/Type/MacroDefinition.h` - packed uint16_t struct
- `src/Macro/Manager/MacroManager.cpp/.h` - state, lookup, execute
- `src/Config/ConfigManager.cpp/.h` - NVS methods for macro read/write

**Acceptance Criteria:**

**Given** the macro infrastructure is implemented
**When** the system initializes
**Then** MacroManager loads all 7 macro definitions from NVS
**And** empty macros (0x0000) are treated as "no macro assigned"
**And** MacroDefinition correctly packs/unpacks modifiers + keycode

**Given** a macro is saved via ConfigManager
**When** the device reboots
**Then** the macro persists and is loaded correctly
**And** NVS keys follow pattern "macro.0" through "macro.6"

**Given** MacroManager.executeMacro() is called with a valid macro
**When** BLE is connected
**Then** the key combination is sent via BleKeyboard
**And** returns true indicating macro was executed

**Given** MacroManager.executeMacro() is called
**When** BLE is NOT connected
**Then** the macro is not sent
**And** returns false (graceful skip, not error)

---

## Story 11.2: Hardware Layer (Macro Button + Event Interception)

As a **user holding the macro button**,
I want **my inputs to execute macros instead of normal actions**,
So that **I can trigger key combinations with a single press**.

**Scope:**
- Macro button GPIO 10 input handling in ButtonEventHandler
- MacroManager state tracking (held/released)
- EncoderEventHandler macro interception (wheel button, left, right)
- ButtonEventHandler macro interception (buttons 1-4)

**Acceptance Criteria:**

**Given** the macro button (GPIO 10) is pressed
**When** ButtonEventHandler receives the event
**Then** MacroManager.setMacroButtonState(true) is called
**And** no other action is taken (modifier doesn't trigger anything itself)

**Given** the macro button is released
**When** ButtonEventHandler receives the event
**Then** MacroManager.setMacroButtonState(false) is called

**Given** macro mode is active (button held) and I rotate the wheel left
**When** the encoder event is processed
**Then** MacroManager.executeMacro(WHEEL_LEFT) is called
**And** if macro exists, it executes and event is consumed
**And** if no macro, normal mode handler receives the event

**Given** macro mode is active and I press Button 1
**When** the button event is processed
**Then** MacroManager.executeMacro(BUTTON_1) is called
**And** if macro exists, it executes and event is consumed

**Given** menu is active
**When** I hold macro button and press an input
**Then** menu handles the event (menu takes priority over macro mode)

**Priority Order Verification:**
1. Menu active → MenuController handles
2. Macro mode active → MacroManager handles
3. Else → Normal mode handler

---

## Story 11.3: User Configuration (Menu + SetMacroAction)

As a **user wanting to configure macros**,
I want **to set key combinations for each input through the menu**,
So that **I can customize what each macro slot does**.

**Scope:**
- `src/Menu/Model/MenuTree.h` - Add Macros submenu with 7 items
- `src/Menu/Action/SetMacroAction.cpp/.h` - Configuration action

**Acceptance Criteria:**

**Given** I navigate to Settings in the menu
**When** I select "Macros"
**Then** I see a submenu with 7 items:
  - Wheel Button: [current combo or "None"]
  - Wheel Left: [current combo or "None"]
  - Wheel Right: [current combo or "None"]
  - Button 1: [current combo or "None"]
  - Button 2: [current combo or "None"]
  - Button 3: [current combo or "None"]
  - Button 4: [current combo or "None"]

**Given** I select an input to configure (e.g., "Wheel Button")
**When** the SetMacroAction executes
**Then** I can select modifier(s) (Ctrl, Shift, Alt, GUI/Cmd)
**And** I can select a key (A-Z, 0-9, F1-F12, etc.)
**And** the combination is saved to NVS
**And** display confirms the save

**Given** I configure a macro as "Ctrl+C"
**When** I view that slot in the menu later
**Then** it displays "Ctrl+C" (not raw hex values)

**Display Format:**
- "Ctrl+C" for single modifier
- "Ctrl+Shift+V" for multiple modifiers
- "None" for empty macro

---

## Story 11.4: System Integration (Initialization + Factory Reset)

As a **user of the complete macro system**,
I want **macros to initialize on boot and clear on factory reset**,
So that **the feature works reliably and resets cleanly**.

**Scope:**
- `src/main.cpp` - MacroManager initialization, inject into handlers
- `src/Config/FactoryReset.cpp` - Clear macro NVS keys

**Acceptance Criteria:**

**Given** the device boots
**When** initialization completes
**Then** MacroManager is created with BleKeyboard dependency
**And** MacroManager is injected into EncoderEventHandler
**And** MacroManager is injected into ButtonEventHandler
**And** all macros are loaded from NVS

**Given** the user performs a factory reset (hold encoder button 5+ sec at boot)
**When** the reset completes
**Then** all macro NVS keys ("macro.0" through "macro.6") are cleared
**And** macros revert to empty (no macro assigned)

**Given** macro button activity occurs
**When** the inactivity timer is running
**Then** the timer resets (consistent with other inputs)

---

## Implementation Checklist

### Story 11.1: Macro Infrastructure
- [ ] Create `include/Config/macro_config.h` with GPIO 10, MACRO_INPUT_COUNT
- [ ] Create `include/Enum/MacroInputEnum.h` with 7 input enum values
- [ ] Create `include/Type/MacroDefinition.h` with packed struct
- [ ] Extend ConfigManager with `loadMacro(index)` and `saveMacro(index, packed)`
- [ ] Create `src/Macro/Manager/MacroManager.h/.cpp`
- [ ] Implement MacroManager constructor with BleKeyboard dependency
- [ ] Implement `loadFromNVS()` to load all 7 macros
- [ ] Implement `executeMacro()` with BLE connection check
- [ ] Test NVS persistence across reboots

### Story 11.2: Hardware Layer
- [ ] Add macro button handling in ButtonEventHandler
- [ ] Implement `setMacroButtonState()` calls
- [ ] Add macro interception to EncoderEventHandler (before mode handler)
- [ ] Add macro interception to ButtonEventHandler (for buttons 1-4)
- [ ] Implement `mapEncoderEventToMacroInput()` helper
- [ ] Implement `mapButtonEventToMacroInput()` helper
- [ ] Verify priority order: Menu > Macro > Normal
- [ ] Test macro execution with BLE connected/disconnected

### Story 11.3: User Configuration
- [ ] Add Macros submenu to MenuTree under Settings
- [ ] Create SetMacroAction class
- [ ] Implement modifier selection UI
- [ ] Implement key selection UI
- [ ] Display current macro value in menu items
- [ ] Format display as "Ctrl+C" style
- [ ] Save to NVS on confirmation
- [ ] Test full configuration flow

### Story 11.4: System Integration
- [ ] Initialize MacroManager in main.cpp
- [ ] Inject MacroManager into EncoderEventHandler
- [ ] Inject MacroManager into ButtonEventHandler
- [ ] Add macro key clearing to FactoryReset
- [ ] Verify inactivity timer reset on macro button activity
- [ ] End-to-end test of complete macro flow

---

## Technical Notes

- GPIO 10 is confirmed available and conflict-free
- Use existing BleKeyboard modifier constants (KEY_LEFT_CTRL, etc.)
- Packed format: `(modifiers << 8) | keycode` as uint16_t
- NVS namespace remains "knobkoky" (existing)
- MacroManager uses static allocation (no heap)
- Empty macro check: `modifiers == 0 && keycode == 0`

---

## Related Files

**New Files:**
- `include/Config/macro_config.h`
- `include/Enum/MacroInputEnum.h`
- `include/Type/MacroDefinition.h`
- `src/Macro/Manager/MacroManager.cpp`
- `src/Macro/Manager/MacroManager.h`
- `src/Menu/Action/SetMacroAction.cpp`
- `src/Menu/Action/SetMacroAction.h`

**Modified Files:**
- `src/Config/ConfigManager.cpp/.h` - NVS methods
- `src/Config/FactoryReset.cpp` - Clear macros
- `src/Event/Handler/EncoderEventHandler.cpp` - Interception
- `src/Event/Handler/ButtonEventHandler.cpp` - Macro button + interception
- `src/Menu/Model/MenuTree.h` - Macros submenu
- `src/main.cpp` - Initialization

---

## Architecture Reference

Full architectural decisions, patterns, and anti-patterns documented in:
`_bmad-output/architecture/macro-feature-architecture.md`

Key patterns:
- Handler-level event interception (not dispatcher level)
- NVS keys: "macro.0" through "macro.6"
- Return `Error` enum from NVS operations
- Use LOG_* macros for all logging

---
