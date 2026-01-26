# Story 11.2: Hardware Layer (Macro Button + Event Interception)

Status: backlog

## Story

As a **user wanting single-hand macro control**,
I want **to toggle macro mode on/off by long-pressing the macro button**,
So that **I can execute macros with a simple press-and-release gesture**.

## Acceptance Criteria

1. **Given** the macro button (GPIO 10) is long-pressed (held ≥500ms) and released
   **When** ButtonEventHandler detects the long-press event
   **Then** MacroManager.toggleMacroMode() is called
   **And** macro mode transitions from inactive→active
   **And** a confirmation is logged or displayed

2. **Given** macro mode is currently active
   **When** the macro button is long-pressed (held ≥500ms) and released again
   **Then** MacroManager.toggleMacroMode() is called
   **And** macro mode transitions from active→inactive
   **And** a confirmation is logged or displayed

3. **Given** the macro button is short-pressed (< 500ms)
   **When** ButtonEventHandler receives the event
   **Then** the short-press is ignored (no action triggered, no mode change)

4. **Given** macro mode is active and I rotate the wheel left
   **When** the encoder event is processed
   **Then** MacroManager.executeMacro(WHEEL_LEFT) is called
   **And** if macro exists, it executes and event is consumed
   **And** if no macro, normal mode handler receives the event

5. **Given** macro mode is active and I press Button 1
   **When** the button event is processed
   **Then** MacroManager.executeMacro(BUTTON_1) is called
   **And** if macro exists, it executes and event is consumed

6. **Given** menu is active
   **When** I hold macro button and press an input
   **Then** menu handles the event (menu takes priority over macro mode)

7. **Given** the priority order is enforced
   **When** checking event handling
   **Then** order is: Menu > Macro > Normal Mode
   **And** once consumed, event does not propagate further

## Tasks

- [ ] **Task 1: Implement Long-Press Detection for Macro Button** (AC: 1, 2, 3, 7)
  - [ ] Update `src/Event/Handler/ButtonEventHandler.h`:
    - [ ] Add member `MacroManager* macroManager`
    - [ ] Add member `uint32_t macroPressStart` (timestamp of press)
    - [ ] Update constructor to accept `MacroManager*`
    - [ ] Import `MacroInputEnum.h` and `macro_config.h`
  - [ ] Update `src/Event/Handler/ButtonEventHandler.cpp`:
    - [ ] In handleEvent() or taskLoop():
      - [ ] Check if `event.pin == MACRO_BUTTON_PIN`
      - [ ] **If pressed:** Record `macroPressStart = millis()`
      - [ ] **If released:**
        - [ ] Calculate hold duration = `millis() - macroPressStart`
        - [ ] If duration ≥ 500ms: call `macroManager->toggleMacroMode()`
        - [ ] If duration < 500ms: Ignore (no action)
      - [ ] Return (do not process as normal button)
      - [ ] If not macro button: continue with normal button handling

- [ ] **Task 2: Add Macro Interception to EncoderEventHandler** (AC: 4, 6, 7)
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

- [ ] **Task 3: Add Macro Interception to ButtonEventHandler** (AC: 5, 6, 7)
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

- [ ] **Task 4: Add Logging for Macro Mode Toggle** (AC: 1, 2, 4, 5, 6, 7)
  - [ ] In ButtonEventHandler macro button handling:
    - [ ] Log long-press detection with LOG_DEBUG
    - [ ] Log macro mode toggle with LOG_INFO (state change)
    - [ ] Log short-press ignored with LOG_DEBUG
  - [ ] In macro interception points:
    - [ ] Log menu consumed event with LOG_DEBUG
    - [ ] Log macro executed with LOG_DEBUG
    - [ ] Log normal mode fallback with LOG_DEBUG
  - [ ] Use format: `LOG_DEBUG("ButtonEventHandler", "Macro long-press detected, toggling macro mode")`

- [ ] **Task 5: Build and Verify Toggle Behavior** (AC: 1, 2, 3, 7)
  - [ ] Compile with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Verify no warnings
  - [ ] Manual test: Long-press macro button (≥500ms), verify mode toggles on
  - [ ] Manual test: Long-press again, verify mode toggles off
  - [ ] Manual test: Short-press macro button (<500ms), verify ignored (no action)
  - [ ] Manual test: With macro mode active, rotate wheel, check logs for priority order
  - [ ] Manual test: With menu active, long-press macro button, check menu consumes event
  - [ ] Manual test: Verify macro mode persists across menu navigation

## Dev Notes

### Event Handling Priority

```
Input Event Arrives
    ↓
[Macro Button Long-Press?] → If yes: toggleMacroMode(), return
    ↓ (No / Short-press)
[Menu Active?] → If yes: Menu handles, return
    ↓ (No)
[Macro Mode Active?] → If yes: Try macro
    ├─ Macro exists? → Execute, return
    └─ No macro? → Fall through
    ↓ (Fall through)
[Normal Mode Handler] → Process normally
```

### Macro Button Behavior (Toggle Activation)

- GPIO 10, active low with pull-up (per macro_config.h)
- **Long-press detection:** Minimum 500ms hold time
- **When long-pressed + released:** `macroManager->toggleMacroMode()`
  - Transitions macro mode from OFF→ON or ON→OFF
  - Logs/displays confirmation
- **When short-pressed (< 500ms):** Ignored, no action
- Does NOT trigger any macro itself (toggle control only)
- Does NOT prevent menu access during active macro mode
- MacroManager state: `macroModeActive` boolean (toggle state, not button state)

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
// ❌ WRONG - Not detecting long-press duration
if (event.pin == MACRO_BUTTON_PIN && event.pressed) {
    macroManager->toggleMacroMode();  // Should wait for release + duration check
}

// ❌ WRONG - Tracking button state instead of toggle state
bool macroButtonHeld;  // Wrong! Use macroModeActive toggle state instead
if (event.pin == MACRO_BUTTON_PIN) {
    macroButtonHeld = event.pressed;
}

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

// ✅ CORRECT - Long-press detection with toggle
if (event.pin == MACRO_BUTTON_PIN) {
    if (event.pressed) {
        macroPressStart = millis();
    } else {
        uint32_t duration = millis() - macroPressStart;
        if (duration >= 500) {
            macroManager->toggleMacroMode();
        }
    }
    return;
}

// ✅ CORRECT - Event priority order
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

1. **Long-Press Toggle Test (ON)**:
   - Press and hold GPIO 10 for ≥500ms
   - Release button
   - Verify macro mode activated
   - Check log: "Macro mode toggled ON"

2. **Long-Press Toggle Test (OFF)**:
   - Long-press GPIO 10 again for ≥500ms
   - Release button
   - Verify macro mode deactivated
   - Check log: "Macro mode toggled OFF"

3. **Short-Press Ignored Test**:
   - Press GPIO 10 for <500ms
   - Release button
   - Verify no action (no toggle, no log)

4. **Macro Execution Test** (with mode ON):
   - Toggle macro mode ON (long-press macro button)
   - Rotate wheel left
   - Check logs for priority order
   - Verify BLE receives key combo (if connected)

5. **Priority Order Test**:
   - Enter menu
   - Toggle macro mode ON (long-press macro button)
   - Press button
   - Verify menu received event (not consumed by macro)

6. **Fallback Test**:
   - Toggle macro mode ON
   - Press input with no macro assigned
   - Verify normal action occurs (fallback)

7. **Persistence Test**:
   - Toggle macro mode ON
   - Enter menu and navigate
   - Verify macro mode still ON when exiting menu

## Dev Agent Record

Status: backlog - ready for implementation

**Story 11.2 covers:** Macro button GPIO handling, event interception logic, priority order enforcement.

**Key Components:**
- ButtonEventHandler: Detect GPIO 10, call setMacroButtonState()
- EncoderEventHandler: Priority check (Menu → Macro → Normal)
- ButtonEventHandler: Priority check for buttons 1-4
- Helper mappers: Convert events to MacroInput enum

**Dependencies:** Requires Story 11.1 (MacroManager complete with toggleMacroMode() interface)

---
