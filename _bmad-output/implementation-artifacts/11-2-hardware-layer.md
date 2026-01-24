# Story 11.2: Hardware Layer (Macro Button + Event Interception)

Status: backlog

## Story

As a **user holding the macro button**,
I want **my inputs to execute macros instead of normal actions**,
So that **I can trigger key combinations with a single press**.

## Acceptance Criteria

1. **Given** the macro button (GPIO 10) is pressed
   **When** ButtonEventHandler receives the event
   **Then** MacroManager.setMacroButtonState(true) is called
   **And** no other action is taken (modifier doesn't trigger anything itself)

2. **Given** the macro button is released
   **When** ButtonEventHandler receives the event
   **Then** MacroManager.setMacroButtonState(false) is called

3. **Given** macro mode is active (button held) and I rotate the wheel left
   **When** the encoder event is processed
   **Then** MacroManager.executeMacro(WHEEL_LEFT) is called
   **And** if macro exists, it executes and event is consumed
   **And** if no macro, normal mode handler receives the event

4. **Given** macro mode is active and I press Button 1
   **When** the button event is processed
   **Then** MacroManager.executeMacro(BUTTON_1) is called
   **And** if macro exists, it executes and event is consumed

5. **Given** menu is active
   **When** I hold macro button and press an input
   **Then** menu handles the event (menu takes priority over macro mode)

6. **Given** the priority order is enforced
   **When** checking event handling
   **Then** order is: Menu > Macro > Normal Mode
   **And** once consumed, event does not propagate further

## Tasks

- [ ] **Task 1: Update ButtonEventHandler for Macro Button** (AC: 1, 2, 6)
  - [ ] Update `src/Event/Handler/ButtonEventHandler.h`:
    - [ ] Add member `MacroManager* macroManager`
    - [ ] Update constructor to accept `MacroManager*`
    - [ ] Import `MacroInputEnum.h` and `macro_config.h`
  - [ ] Update `src/Event/Handler/ButtonEventHandler.cpp`:
    - [ ] In handleEvent() or taskLoop():
      - [ ] Check if `event.pin == MACRO_BUTTON_PIN`
      - [ ] If yes: call `macroManager->setMacroButtonState(event.pressed)`
      - [ ] If yes: return (do not process as normal button)
      - [ ] If no: continue with normal button handling

- [ ] **Task 2: Add Macro Interception to EncoderEventHandler** (AC: 3, 5, 6)
  - [ ] Update `src/Event/Handler/EncoderEventHandler.h`:
    - [ ] Add member `MacroManager* macroManager`
    - [ ] Update constructor to accept `MacroManager*`
    - [ ] Import `MacroInputEnum.h` and `MacroManager.h`
  - [ ] Update `src/Event/Handler/EncoderEventHandler.cpp`:
    - [ ] In handleEvent() or taskLoop():
      - [ ] **Priority 1:** Check if menu is active
        - [ ] If yes: `menuController.handleEvent(event)` and return
      - [ ] **Priority 2:** Check if macro mode active
        - [ ] If yes: call `MacroInput input = mapEncoderEventToMacroInput(event)`
        - [ ] If yes: call `macroManager->executeMacro(input)`
        - [ ] If macro executed (returned true): return (event consumed)
        - [ ] If no macro (returned false): fall through to Priority 3
      - [ ] **Priority 3:** Normal mode handling
        - [ ] Call `currentModeHandler->handleEvent(event)`
  - [ ] Create helper function `MacroInput mapEncoderEventToMacroInput(const EncoderInputEvent& event)`:
    - [ ] Wheel rotation left → MacroInput::WHEEL_LEFT
    - [ ] Wheel rotation right → MacroInput::WHEEL_RIGHT
    - [ ] Wheel button press → MacroInput::WHEEL_BUTTON

- [ ] **Task 3: Add Macro Interception to ButtonEventHandler** (AC: 4, 5, 6)
  - [ ] In `src/Event/Handler/ButtonEventHandler.cpp` handleEvent() or taskLoop():
    - [ ] After macro button check (Task 1)
    - [ ] **Priority 1:** Check if menu is active
      - [ ] If yes: `menuController.handleEvent(event)` and return
    - [ ] **Priority 2:** Check if macro mode active
      - [ ] If yes: call `MacroInput input = mapButtonEventToMacroInput(event)`
      - [ ] If yes: call `macroManager->executeMacro(input)`
      - [ ] If macro executed (returned true): return (event consumed)
      - [ ] If no macro (returned false): fall through to Priority 3
    - [ ] **Priority 3:** Normal button action
      - [ ] Call existing button action handler (SelectWheelModeAction, etc.)
  - [ ] Create helper function `MacroInput mapButtonEventToMacroInput(const ButtonEvent& event)`:
    - [ ] Button 1 (GPIO 3) → MacroInput::BUTTON_1
    - [ ] Button 2 (GPIO 4) → MacroInput::BUTTON_2
    - [ ] Button 3 (GPIO 5) → MacroInput::BUTTON_3
    - [ ] Button 4 (GPIO 9) → MacroInput::BUTTON_4

- [ ] **Task 4: Add Logging for Priority Order** (AC: 3, 4, 5, 6)
  - [ ] In macro interception points:
    - [ ] Log menu consumed event with LOG_DEBUG
    - [ ] Log macro executed with LOG_DEBUG
    - [ ] Log normal mode fallback with LOG_DEBUG
  - [ ] Use format: `LOG_DEBUG("EncoderEventHandler", "Priority 2: Macro mode, executing input %d", input)`

- [ ] **Task 5: Build and Verify Priority Order** (AC: 6)
  - [ ] Compile with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Verify no warnings
  - [ ] Manual test: Press macro button, verify no action
  - [ ] Manual test: Hold macro button, rotate wheel, check logs for priority order
  - [ ] Manual test: With menu active, hold macro button, check menu consumes event
  - [ ] Manual test: Release macro button, verify normal mode resumes

## Dev Notes

### Event Handling Priority

```
Input Event Arrives
    ↓
[Macro Button?] → If yes: set state, return
    ↓ (No)
[Menu Active?] → If yes: Menu handles, return
    ↓ (No)
[Macro Mode Active?] → If yes: Try macro
    ├─ Macro exists? → Execute, return
    └─ No macro? → Fall through
    ↓ (Fall through)
[Normal Mode Handler] → Process normally
```

### Macro Button Behavior

- GPIO 10, active low with pull-up (per macro_config.h)
- When pressed: `macroManager->setMacroButtonState(true)`
- When released: `macroManager->setMacroButtonState(false)`
- Does NOT trigger any macro itself (modifier-only)
- Does NOT prevent menu access

### Encoder Event Mapping

```cpp
EncoderInputEvent: type
├─ ROTATE_LEFT → MacroInput::WHEEL_LEFT
├─ ROTATE_RIGHT → MacroInput::WHEEL_RIGHT
└─ BUTTON_PRESSED → MacroInput::WHEEL_BUTTON
```

### Button Event Mapping

```cpp
ButtonEvent: pin
├─ GPIO 3 → MacroInput::BUTTON_1
├─ GPIO 4 → MacroInput::BUTTON_2
├─ GPIO 5 → MacroInput::BUTTON_3
└─ GPIO 9 → MacroInput::BUTTON_4
```

### Critical Priority Order

**MUST be enforced strictly:**

1. Menu always takes precedence (user navigating settings)
2. Macro mode next (when macro button held)
3. Normal mode last (default behavior)

Without this order:
- Menu items become hard to select (macros consume events)
- Macro mode can't be toggled (menu intercepts button)
- User experience broken

### Files to Modify

```
src/Event/Handler/EncoderEventHandler.h (add MacroManager*)
src/Event/Handler/EncoderEventHandler.cpp (add macro interception)
src/Event/Handler/ButtonEventHandler.h (add MacroManager*)
src/Event/Handler/ButtonEventHandler.cpp (add macro button + interception)
```

### Helper Functions Location

Place in same file as handler:
- `mapEncoderEventToMacroInput()` in EncoderEventHandler.cpp
- `mapButtonEventToMacroInput()` in ButtonEventHandler.cpp

Or in shared header if multiple files need them.

### Key Implementation Points

- Macro button is special: no mapping to MacroInput, just state
- Regular buttons 1-4 map to BUTTON_1-4 in MacroInput enum
- Wheel events (left/right/button) map to WHEEL_* enum values
- Always check BLE in MacroManager.executeMacro(), not in handler

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Wrong priority order
if (macroManager.isMacroModeActive()) {
    macroManager.executeMacro(input);
}
if (menuController.isActive()) {  // Should be FIRST
    menuController.handleEvent(event);
}

// ❌ WRONG - Consuming all events when macro not assigned
if (macroManager.isMacroModeActive()) {
    // Consume event even if no macro assigned
    return;
}

// ❌ WRONG - Checking BLE in handler instead of manager
if (macroManager.isMacroModeActive() && bleKeyboard.isConnected()) {
    macroManager.executeMacro(input);  // Should check inside executeMacro()
}

// ❌ WRONG - Treating macro button like normal button
if (event.pin == MACRO_BUTTON_PIN) {
    actionCache[index] = SelectWheelModeAction(...);  // Wrong!
}

// ✅ CORRECT
if (menuController.isActive()) {
    menuController.handleEvent(event);
    return;
}
if (macroManager.isMacroModeActive()) {
    MacroInput input = mapButtonEventToMacroInput(event);
    if (macroManager.executeMacro(input)) {
        return;  // Event consumed by macro
    }
}
// Normal handling only if no macro or no macro mode
```

### Testing Approach

1. **Macro Button Test**:
   - Hold GPIO 10
   - Verify no action triggered
   - Check log: "Macro mode active"
   - Release, verify normal mode resumes

2. **Macro Execution Test**:
   - Hold macro button
   - Rotate wheel left
   - Check logs for priority order
   - Verify BLE receives key combo (if connected)

3. **Priority Order Test**:
   - Enter menu
   - Hold macro button
   - Press button
   - Verify menu received event (not consumed by macro)

4. **Fallback Test**:
   - Hold macro button for input with no macro assigned
   - Verify normal action occurs (fallback)

## Dev Agent Record

Status: backlog - ready for implementation

**Story 11.2 covers:** Macro button GPIO handling, event interception logic, priority order enforcement.

**Key Components:**
- ButtonEventHandler: Detect GPIO 10, call setMacroButtonState()
- EncoderEventHandler: Priority check (Menu → Macro → Normal)
- ButtonEventHandler: Priority check for buttons 1-4
- Helper mappers: Convert events to MacroInput enum

**Dependencies:** Requires Story 11.1 (MacroManager complete)

---
