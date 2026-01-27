# Story 11.2: Hardware Layer (Macro Button + Event Interception)

Status: backlog

## Implementation Approach

**CRITICAL: This story must be implemented in TWO parts with a mandatory code review checkpoint.**

### Part 1: ButtonDriver Refactoring (Architectural Foundation)
- Migrate ButtonManager logic to new ButtonDriver (lib/)
- Add duration tracking capability (short/long press detection)
- Mirror EncoderDriver pattern for architectural consistency
- Update event types to include SHORT_PRESS/LONG_PRESS
- **STOP after Part 1 implementation**
- **Request code review from user**
- **Do NOT proceed to Part 2 until review is complete**

### Part 2: Macro Mode Integration (After Code Review)
- Implement macro button toggle logic using ButtonDriver
- Add macro interception to event handlers
- Implement priority-based routing (Menu > Macro > Normal)
- Add HardwareState.macroModeActive synchronization

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

### PART 1: ButtonDriver Refactoring (IMPLEMENT FIRST, THEN STOP FOR REVIEW)

- [x] **Task 1.1: Create ButtonDriver Class Structure** (Foundation)
  - [ ] Create `lib/ButtonDriver/ButtonDriver.h`:
    - [ ] Class declaration with Singleton pattern (mirror EncoderDriver)
    - [ ] Constructor: accept pin configuration (BUTTONS array from button_config.h)
    - [ ] Static `getInstance()` method
    - [ ] `void begin()` - initialize pins and start FreeRTOS task
    - [ ] Callback setters:
      - [ ] `setOnShortPress(uint8_t buttonIndex, std::function<void()> callback)`
      - [ ] `setOnLongPress(uint8_t buttonIndex, std::function<void()> callback)`
    - [ ] Private members:
      - [ ] `static ButtonDriver* instance`
      - [ ] `std::function<void()> shortPressCallbacks[BUTTON_COUNT]`
      - [ ] `std::function<void()> longPressCallbacks[BUTTON_COUNT]`
      - [ ] `struct ButtonState { bool pressed; bool lastReading; uint32_t lastChangeTime; uint32_t pressStartTime; }`
      - [ ] `ButtonState buttonStates[BUTTON_COUNT]`
    - [ ] Private methods:
      - [ ] `static void buttonTask(void* pvParameters)` - FreeRTOS task
      - [ ] `void runLoop()` - main polling loop
      - [ ] `void handleButton(uint8_t index)` - process single button
      - [ ] `bool readButton(uint8_t index)` - GPIO read
      - [ ] `bool isDebounced(uint8_t index, uint32_t currentTime)` - debounce check

  - [ ] Create `lib/ButtonDriver/ButtonDriver.cpp`:
    - [ ] Implement constructor (initialize buttonStates array, clear callbacks)
    - [ ] Implement `getInstance()` (lazy Singleton creation)
    - [ ] Implement `begin()`:
      - [ ] Configure GPIO pins (INPUT_PULLUP/INPUT_PULLDOWN per BUTTONS config)
      - [ ] Create FreeRTOS task with `xTaskCreate(buttonTask, "ButtonDriverTask", 2048, nullptr, 1, nullptr)`
    - [ ] Implement `buttonTask()` (static wrapper calling `runLoop()`)
    - [ ] Implement `runLoop()`:
      - [ ] Loop through all buttons (0 to BUTTON_COUNT-1)
      - [ ] Call `handleButton(i)` for each
      - [ ] `vTaskDelay(pdMS_TO_TICKS(10))` at end
    - [ ] Implement `handleButton(uint8_t index)`:
      - [ ] Read current state: `bool reading = readButton(index)`
      - [ ] If reading != lastReading: restart debounce timer, update lastReading, return
      - [ ] If not debounced: return
      - [ ] If state changed after debounce:
        - [ ] If pressed: record `pressStartTime = millis()`
        - [ ] If released:
          - [ ] Calculate duration: `millis() - pressStartTime`
          - [ ] If duration >= 1000ms: call `longPressCallbacks[index]()` if set
          - [ ] Else if duration >= 50ms: call `shortPressCallbacks[index]()` if set
        - [ ] Update `buttonStates[index].pressed = reading`
    - [ ] Implement `readButton(uint8_t index)` (mirror ButtonManager logic)
    - [ ] Implement `isDebounced(uint8_t index, uint32_t currentTime)` (mirror ButtonManager logic)
    - [ ] Implement callback setters (store in arrays)

- [x] **Task 1.2: Update ButtonEventDispatcher for Duration Events**
  - [ ] Update `include/Enum/EventEnum.h`:
    - [ ] Add to `ButtonEventTypes` enum:
      - [ ] `SHORT_PRESS` (< 1000ms)
      - [ ] `LONG_PRESS` (>= 1000ms)
    - [ ] Keep existing `BUTTON_PRESSED` and `BUTTON_RELEASED` for compatibility

  - [ ] Update `src/Event/Dispatcher/ButtonEventDispatcher.h`:
    - [ ] Add methods:
      - [ ] `void onButtonShortPress(uint8_t buttonIndex)`
      - [ ] `void onButtonLongPress(uint8_t buttonIndex)`

  - [ ] Update `src/Event/Dispatcher/ButtonEventDispatcher.cpp`:
    - [ ] Implement `onButtonShortPress()`: Queue SHORT_PRESS event
    - [ ] Implement `onButtonLongPress()`: Queue LONG_PRESS event

- [x] **Task 1.3: Integrate ButtonDriver in main.cpp**
  - [ ] Update `src/main.cpp`:
    - [ ] Replace `#include "Button/ButtonManager.h"` with `#include "ButtonDriver.h"`
    - [ ] Replace `ButtonManager buttonManager(&buttonEventDispatcher);` with:
      - [ ] `ButtonDriver* buttonDriver = ButtonDriver::getInstance();`
    - [ ] In `setup()`:
      - [ ] Replace `buttonManager.init();` with:
        - [ ] `buttonDriver->begin();`
        - [ ] For each button (0 to BUTTON_COUNT-1):
          - [ ] `buttonDriver->setOnShortPress(i, [i, &buttonEventDispatcher]() { buttonEventDispatcher.onButtonShortPress(i); });`
          - [ ] `buttonDriver->setOnLongPress(i, [i, &buttonEventDispatcher]() { buttonEventDispatcher.onButtonLongPress(i); });`

- [x] **Task 1.4: Remove Old ButtonManager Files**
  - [ ] Delete `src/Button/ButtonManager.h`
  - [ ] Delete `src/Button/ButtonManager.cpp`
  - [ ] Delete `src/Button/` directory if empty

- [x] **Task 1.5: Build and Test ButtonDriver Refactoring**
  - [x] Compile with `.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble`
  - [x] Verify no compile errors
  - [x] Verify no warnings
  - [ ] Manual test: Press each button (1-4), verify SHORT_PRESS events dispatched
  - [ ] Manual test: Hold each button >=1000ms, verify LONG_PRESS events dispatched
  - [ ] Check logs for event dispatching

- [x] **🛑 CHECKPOINT: STOP HERE - REQUEST CODE REVIEW FROM USER**
  - [x] Do NOT proceed to Part 2 until user reviews Part 1
  - [x] User will review ButtonDriver implementation
  - [ ] Wait for approval before continuing

---

### PART 2: Macro Mode Integration (IMPLEMENT AFTER CODE REVIEW)

- [ ] **Task 2.1: Add macroModeActive to HardwareState**
  - [ ] Update `include/state/HardwareState.h`:
    - [ ] Add `bool macroModeActive = false;` member

- [ ] **Task 2.2: Implement Macro Button Toggle Logic**
  - [ ] Update `src/Event/Handler/ButtonEventHandler.h`:
    - [ ] Add member `HardwareState* hardwareState` (injected via constructor)
    - [ ] Add member `MacroManager* macroManager` (injected via constructor)
    - [ ] Update constructor signature to accept both pointers
    - [ ] Import `include/Config/macro_config.h`, `include/Macro/MacroManager.h`, `include/state/HardwareState.h`

  - [ ] Update `src/Event/Handler/ButtonEventHandler.cpp`:
    - [ ] Update constructor to accept and store both pointers
    - [ ] In `taskLoop()`, add macro button check at start:
      - [ ] Check if `event.buttonIndex == MACRO_BUTTON_INDEX` (index 4 per macro_config.h)
      - [ ] AND `event.type == EventEnum::ButtonEventTypes::LONG_PRESS`
      - [ ] If true:
        - [ ] Call `macroManager->toggleMacroMode()`
        - [ ] Update `hardwareState->macroModeActive = macroManager->isMacroModeActive()`
        - [ ] Log with LOG_INFO: "Macro mode toggled to [ON/OFF]"
        - [ ] Return (event consumed)
      - [ ] If macro button SHORT_PRESS: ignore, return
      - [ ] Otherwise: continue to normal button handling

- [ ] **Task 2.3: Add Macro Interception to EncoderEventHandler** (AC: 4, 6, 7)
  - [ ] Update `src/Event/Handler/EncoderEventHandler.h`:
    - [ ] Add member `HardwareState* hardwareState` (injected via constructor)
    - [ ] Add member `MacroManager* macroManager` (injected via constructor)
    - [ ] Update constructor to accept both pointers
    - [ ] Import `include/Enum/MacroInputEnum.h`, `include/Macro/MacroManager.h`, `include/state/HardwareState.h`

  - [ ] Update `src/Event/Handler/EncoderEventHandler.cpp`:
    - [ ] Update constructor to accept and store both pointers
    - [ ] In `taskLoop()`, implement priority routing:
      - [ ] **Priority 1:** If `menuController->isActive()`: handle menu, return
      - [ ] **Priority 2:** If `hardwareState->macroModeActive`:
        - [ ] Convert event to MacroInput: `MacroInput input = mapEncoderEventToMacroInput(event)`
        - [ ] If `macroManager->executeMacro(input)` returns true: return (consumed)
        - [ ] Else: fall through to Priority 3
      - [ ] **Priority 3:** Normal mode handling: `currentModeHandler->handleEvent(event)`

    - [ ] Create helper function `MacroInput mapEncoderEventToMacroInput(const EncoderInputEvent& event)`:
      - [ ] ROTATE_LEFT (delta < 0) → MacroInput::WHEEL_LEFT
      - [ ] ROTATE_RIGHT (delta > 0) → MacroInput::WHEEL_RIGHT
      - [ ] SHORT_CLICK → MacroInput::WHEEL_BUTTON
      - [ ] Default: MacroInput::WHEEL_BUTTON

- [ ] **Task 2.4: Add Macro Interception to ButtonEventHandler** (AC: 5, 6, 7)
  - [ ] In `src/Event/Handler/ButtonEventHandler.cpp` taskLoop():
    - [ ] After macro button check from Task 2.2
    - [ ] Implement priority routing for regular buttons (1-4):
      - [ ] **Priority 1:** If menu active: let menu handle (existing logic)
      - [ ] **Priority 2:** If `hardwareState->macroModeActive && event.type == SHORT_PRESS`:
        - [ ] Convert to MacroInput: `MacroInput input = mapButtonIndexToMacroInput(event.buttonIndex)`
        - [ ] If `macroManager->executeMacro(input)` returns true: return (consumed)
        - [ ] Else: fall through to Priority 3
      - [ ] **Priority 3:** Normal button action: `executeButtonAction(event.buttonIndex)`

    - [ ] Create helper function `MacroInput mapButtonIndexToMacroInput(uint8_t buttonIndex)`:
      - [ ] 0 → MacroInput::BUTTON_1
      - [ ] 1 → MacroInput::BUTTON_2
      - [ ] 2 → MacroInput::BUTTON_3
      - [ ] 3 → MacroInput::BUTTON_4
      - [ ] Default: MacroInput::BUTTON_1

- [ ] **Task 2.5: Update main.cpp to Wire Dependencies**
  - [ ] Update `src/main.cpp`:
    - [ ] Pass `&hardwareState` and `&macroManager` to EncoderEventHandler constructor
    - [ ] Pass `&hardwareState` and `&macroManager` to ButtonEventHandler constructor

- [ ] **Task 2.6: Add Logging for Macro Events**
  - [ ] In ButtonEventHandler (Task 2.2):
    - [ ] LOG_INFO when macro mode toggled
    - [ ] LOG_DEBUG when macro button short-press ignored
  - [ ] In EncoderEventHandler (Task 2.3):
    - [ ] LOG_DEBUG when macro executed
    - [ ] LOG_DEBUG when falling through to normal mode
  - [ ] In ButtonEventHandler (Task 2.4):
    - [ ] LOG_DEBUG when macro executed
    - [ ] LOG_DEBUG when falling through to normal mode

- [ ] **Task 2.7: Build and Test Full Macro Integration**
  - [ ] Compile with `.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Verify no warnings
  - [ ] Manual test: Long-press macro button (GPIO 10), verify mode toggles ON
  - [ ] Manual test: Long-press again, verify mode toggles OFF
  - [ ] Manual test: Short-press macro button, verify ignored (no toggle)
  - [ ] Manual test: With macro mode ON, rotate wheel, check macro execution logs
  - [ ] Manual test: With macro mode ON, press button 1-4, check macro execution
  - [ ] Manual test: Priority order - menu > macro > normal
  - [ ] Manual test: Fallback - macro mode ON but no macro assigned → normal action

## Dev Notes

### Architectural Decision: ButtonDriver Pattern

**Problem:** Original story had ButtonEventHandler tracking `macroPressStart` timestamp and calculating duration on release. This violates Single Responsibility Principle (SRP) - handler should route events, not manage hardware timing state.

**Solution:** Mirror the existing EncoderDriver pattern by creating ButtonDriver in `lib/`:

**EncoderDriver Pattern (existing):**
```
EncoderDriver (lib/)
├─ Hardware polling (GPIO reads)
├─ Duration detection (short/long press in handleButton())
└─ Callbacks → EncoderEventDispatcher
                └─ Queues SHORT_CLICK/LONG_CLICK
                    └─ EncoderEventHandler processes
```

**ButtonDriver Pattern (new):**
```
ButtonDriver (lib/)
├─ Hardware polling (GPIO reads)
├─ Debouncing
├─ Duration tracking (pressStartTime → calculate on release)
└─ Callbacks → ButtonEventDispatcher
                └─ Queues SHORT_PRESS/LONG_PRESS
                    └─ ButtonEventHandler processes
```

**Benefits:**
1. ✅ **SRP Maintained** - Handler focuses on routing, driver handles hardware timing
2. ✅ **Consistency** - Matches EncoderDriver pattern exactly
3. ✅ **Testability** - Duration logic isolated in driver
4. ✅ **Reusability** - Any button can have short/long press detection
5. ✅ **Low Overhead** - Static arrays, ~200 bytes (5 buttons × 2 fields × 4 bytes)

**Migration Path:**
- Part 1: Refactor ButtonManager → ButtonDriver with duration tracking
- Part 2: Use ButtonDriver's LONG_PRESS events for macro toggle

### Architecture: HardwareState as Single Source of Truth

**Key Principle:** The macro mode state lives in `HardwareState`, NOT in `MacroManager`.

- `HardwareState::macroModeActive` is the canonical state (initialized to false)
- `MacroManager` handles toggle logic and macro execution, but doesn't own the state
- Handlers get `HardwareState*` injected and check `hardwareState->macroModeActive`
- Any module can see macro mode state by checking `HardwareState` (consistent with encoder wheel mode, BLE state, etc.)

**Flow:** When macro button long-pressed:
1. ButtonEventHandler detects long-press ≥500ms
2. Calls `macroManager->toggleMacroMode()` (toggle logic)
3. **Immediately updates** `hardwareState->macroModeActive = macroManager->isMacroModeActive()`
4. Now all modules see the new state via `hardwareState`

### Event Handling Priority

```
Input Event Arrives
    ↓
[Macro Button Long-Press (GPIO 10)?] → If yes (≥500ms):
    ├─ Call macroManager->toggleMacroMode()
    ├─ Sync state: hardwareState->macroModeActive = macroManager->isMacroModeActive()
    └─ Return (event consumed)
    ↓ (No / Short-press)
[Menu Active?] → If yes: Menu handles, return
    ↓ (No)
[Macro Mode Active?] → Check hardwareState->macroModeActive:
    ├─ If true: Try to execute macro
    │   ├─ Macro exists? → Execute, return
    │   └─ No macro? → Fall through
    ↓ (Fall through or mode inactive)
[Normal Mode Handler] → Process normally
```

### Macro Button Behavior (Toggle Activation)

- GPIO 10, active low with pull-up (per macro_config.h)
- **Long-press detection:** Minimum 500ms hold time
- **When long-pressed + released:**
  - Call `macroManager->toggleMacroMode()`
  - Update `hardwareState->macroModeActive` to reflect new state
  - Transitions from OFF→ON or ON→OFF
  - Logs confirmation
- **When short-pressed (< 500ms):** Ignored, no action
- Does NOT trigger any macro itself (toggle control only)
- Does NOT prevent menu access during active macro mode
- State source: `HardwareState.macroModeActive` (handlers check this, not MacroManager)

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

### Files to Create/Modify

**Part 1 (ButtonDriver Refactoring):**
```
lib/ButtonDriver/ButtonDriver.h (create - driver class definition)
lib/ButtonDriver/ButtonDriver.cpp (create - driver implementation)
include/Enum/EventEnum.h (update - add SHORT_PRESS/LONG_PRESS to ButtonEventTypes)
src/Event/Dispatcher/ButtonEventDispatcher.h (update - add short/long press methods)
src/Event/Dispatcher/ButtonEventDispatcher.cpp (update - implement short/long press methods)
src/main.cpp (update - replace ButtonManager with ButtonDriver)
src/Button/ButtonManager.h (delete)
src/Button/ButtonManager.cpp (delete)
```

**Part 2 (Macro Integration):**
```
include/state/HardwareState.h (update - add macroModeActive field)
src/Event/Handler/EncoderEventHandler.h (update - add HardwareState*, MacroManager*)
src/Event/Handler/EncoderEventHandler.cpp (update - add macro interception with priority)
src/Event/Handler/ButtonEventHandler.h (update - add HardwareState*, MacroManager*)
src/Event/Handler/ButtonEventHandler.cpp (update - add macro button toggle + interception)
src/main.cpp (update - wire HardwareState and MacroManager to handlers)
```

### Helper Functions Location

Place in same file as handler:
- `mapEncoderEventToMacroInput()` in EncoderEventHandler.cpp
- `mapButtonIndexToMacroInput()` in ButtonEventHandler.cpp

### Key Implementation Points

- Macro button is special: no mapping to MacroInput, just state
- Regular buttons 1-4 map to BUTTON_1-4 in MacroInput enum
- Wheel events (left/right/button) map to WHEEL_* enum values
- Always check BLE in MacroManager.executeMacro(), not in handler

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Storing state in MacroManager instead of HardwareState
class MacroManager {
    bool macroModeActive;  // Wrong! State should be in HardwareState
};

// ❌ WRONG - Handler checking MacroManager state directly
if (macroManager->isMacroModeActive()) {  // Don't use this
    macroManager->executeMacro(input);
}

// ✅ CORRECT - Handler checks HardwareState
if (hardwareState->macroModeActive) {  // Read from HardwareState
    if (macroManager->executeMacro(input)) {
        return;
    }
}

// ❌ WRONG - Handler tracking duration (violates SRP)
class ButtonEventHandler {
    uint32_t macroPressStart;  // Wrong! Handler should not manage hardware timing

    void taskLoop() {
        if (event.type == BUTTON_PRESSED) {
            macroPressStart = millis();  // Handler doing driver's job
        } else {
            uint32_t duration = millis() - macroPressStart;
            if (duration >= 500) { /* ... */ }
        }
    }
};

// ✅ CORRECT - Driver handles duration, handler receives LONG_PRESS event
// ButtonDriver calculates duration internally, dispatches LONG_PRESS event
// ButtonEventHandler simply checks event type
if (event.buttonIndex == MACRO_BUTTON_INDEX && event.type == LONG_PRESS) {
    macroManager->toggleMacroMode();
    hardwareState->macroModeActive = macroManager->isMacroModeActive();
    LOG_INFO("ButtonEventHandler", "Macro mode toggled");
    return;
}

// ❌ WRONG - Forgetting to sync state after toggle
macroManager->toggleMacroMode();
// Missing: hardwareState->macroModeActive = macroManager->isMacroModeActive();

// ❌ WRONG - Wrong priority order
if (hardwareState->macroModeActive) {
    macroManager->executeMacro(input);
}
if (menuController->isActive()) {  // Should be FIRST
    menuController->handleEvent(event);
    return;
}

// ❌ WRONG - Consuming all events when macro not assigned
if (hardwareState->macroModeActive) {
    // Consume event even if no macro assigned
    return;  // Should fall through if executeMacro returns false
}

// ✅ CORRECT - Event priority order (Menu > Macro > Normal)
// Priority 1: Menu
if (menuController->isActive()) {
    // Menu handles everything when active
    return;
}

// Priority 2: Macro mode
if (hardwareState->macroModeActive) {  // Check HardwareState
    MacroInput input = mapButtonIndexToMacroInput(event.buttonIndex);
    if (macroManager->executeMacro(input)) {
        return;  // Event consumed by macro
    }
    // Fall through if no macro assigned
}

// Priority 3: Normal handling
executeButtonAction(event.buttonIndex);
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

**CRITICAL IMPLEMENTATION FLOW:**
1. **Implement Part 1 (ButtonDriver refactoring)**
2. **STOP - Request code review from user**
3. **Wait for approval**
4. **Then implement Part 2 (Macro integration)**

**Part 1 - ButtonDriver Refactoring:**
- Create lib/ButtonDriver mirroring EncoderDriver pattern
- Migrate ButtonManager logic with duration tracking added
- Update ButtonEventDispatcher for SHORT_PRESS/LONG_PRESS event types
- Wire ButtonDriver in main.cpp with callbacks
- Test short/long press detection works for all buttons

**Part 2 - Macro Integration:**
- Add HardwareState.macroModeActive (single source of truth for state)
- ButtonEventHandler: Detect LONG_PRESS on GPIO 10, toggle mode, sync HardwareState
- EncoderEventHandler: Priority routing (Menu → Macro → Normal), macro interception
- ButtonEventHandler: Macro interception for buttons 1-4, priority handling
- Helper mappers: Convert events to MacroInput enum

**Key Architectural Decisions:**
1. **ButtonDriver Pattern** - Duration tracking lives in driver (not handler), maintains SRP
2. **HardwareState Ownership** - Macro mode state in HardwareState (not MacroManager)
3. **Priority Routing** - Menu > Macro > Normal (enforced in both handlers)
4. **Fallback Behavior** - If macro not assigned, fall through to normal action

**Critical Implementation Rules:**
- Handler NEVER tracks timing - ButtonDriver owns duration detection
- State ALWAYS in HardwareState - handlers check `hardwareState->macroModeActive`
- After toggle ALWAYS sync - `hardwareState->macroModeActive = macroManager->isMacroModeActive()`
- Priority MUST be enforced - menu first, then macro, then normal

**Dependencies:**
- Requires Story 11.1 (MacroManager complete with toggleMacroMode() interface)

---
