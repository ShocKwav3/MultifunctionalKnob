# Story 9.6: Implement Display Wake on Menu Activation

Status: ready-for-dev

## Story

As a **user with the display turned off**,
I want **the display to wake when I long-press the wheel button**,
so that **I can see the menu and device status again**.

## Acceptance Criteria

1. **Given** display is off
   **When** I long-press the wheel button (Menu activation)
   **Then** display wakes up immediately
   **And** menu opens simultaneously

2. **Given** display wakes
   **When** subsequent inputs occur
   **Then** they work normally
   **And** display remains on

3. **Given** display is already on
   **When** I long-press the wheel button
   **Then** menu opens normally
   **And** no unnecessary display wake occurs

4. **Given** display is off
   **When** I wake it via menu activation
   **Then** display power state is updated in NVS
   **And** menu shows "Display On" state

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Identify Menu Activation Point** (AC: 1, 3)
  - [ ] Review `src/Menu/Controller/MenuController.cpp`
  - [ ] Identify where menu activation occurs (long-press wheel button)
  - [ ] Document the activation flow
  - [ ] Identify where display wake should be triggered

- [ ] **Task 2: Modify MenuController to Wake Display** (AC: 1, 2, 3)
  - [ ] Update `MenuController::activate()` method:
    - [ ] Call `display->setPower(true)` before menu activation
    - [ ] Log display wake with `LOG_INFO`
    - [ ] Ensure wake happens regardless of current display state
  - [ ] Update `MenuController.h`:
    - [ ] Add `DisplayInterface* display` member if not present
    - [ ] Update constructor to accept `DisplayInterface*`

- [ ] **Task 3: Update Display Power State on Wake** (AC: 4)
  - [ ] In `MenuController::activate()`:
    - [ ] After waking display, update ConfigManager
    - [ ] Call `configManager->setDisplayPower(true)`
    - [ ] Log state update with `LOG_INFO`

- [ ] **Task 4: Update MenuController Constructor** (AC: 1)
  - [ ] Update `src/Menu/Controller/MenuController.h`:
    - [ ] Add `DisplayInterface* display` member
    - [ ] Add `ConfigManager* configManager` member
  - [ ] Update constructor in `MenuController.cpp`:
    - [ ] Accept `DisplayInterface* display` parameter
    - [ ] Accept `ConfigManager* configManager` parameter
    - [ ] Store pointers in member variables

- [ ] **Task 5: Update MenuController Instantiation** (AC: 1)
  - [ ] Update `src/main.cpp`:
    - [ ] Pass `OLEDDisplay` instance to `MenuController` constructor
    - [ ] Pass `ConfigManager` instance to `MenuController` constructor

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Turn display off, long-press wheel button
  - [ ] Verify display wakes and menu opens
  - [ ] Manual test: Verify display power state updates in NVS

## Dev Notes

### Architecture Compliance

- **Event Flow**: `MenuController` is the logical place to force display on when entering menu state
- **DI Pattern**: Pass `DisplayInterface` and `ConfigManager` to `MenuController` constructor for testability
- **State Management**: Display power state should be updated in NVS when waking
- **Non-Blocking**: Display wake should be immediate, no blocking waits

### Menu Activation Flow

```
User Action: Long-press wheel button
    ↓
MenuController::activate()
    ↓
display->setPower(true)
    ↓
SSD1306_DISPLAYON command
    ↓
Display wakes
    ↓
configManager->setDisplayPower(true)
    ↓
NVS updated
    ↓
Menu opens
```

### MenuController Pattern

```cpp
// src/Menu/Controller/MenuController.h
class MenuController {
private:
    DisplayInterface* display;
    ConfigManager* configManager;
    bool menuActive;
    // ... other members

public:
    MenuController(
        AppEventDispatcher* dispatcher,
        DisplayInterface* display,
        ConfigManager* configManager
    );

    void activate();
    // ... other methods
};
```

### Activation Implementation

```cpp
// src/Menu/Controller/MenuController.cpp
void MenuController::activate() {
    LOG_INFO("MenuController", "Activating menu");

    // Wake display if it's off
    display->setPower(true);

    // Update display power state in NVS
    Error result = configManager->setDisplayPower(true);
    if (result != Error::OK) {
        LOG_ERROR("MenuController", "Failed to update display power: %d", result);
    }

    // Activate menu
    menuActive = true;

    // Request menu display
    DisplayRequest req;
    req.type = DisplayRequest::DRAW_MENU;
    req.data.menu.title = "Menu";
    req.data.menu.items = getCurrentMenuItems();
    req.data.menu.count = getCurrentMenuCount();
    req.data.menu.selected = 0;
    xQueueSend(displayQueue, &req, portMAX_DELAY);
}
```

### Existing Code Locations

```
src/Menu/Controller/MenuController.h/cpp  - Menu activation logic
src/Display/Interface/DisplayInterface.h       - setPower() method
src/Config/ConfigManager.h/cpp                - Display power state persistence
src/main.cpp                                    - MenuController instantiation
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Menu/Controller/MenuController.h` | Add display and configManager members |
| `src/Menu/Controller/MenuController.cpp` | Wake display on activation |
| `src/main.cpp` | Pass display and configManager to MenuController |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Wake Test**:
   - Turn display off via menu
   - Long-press wheel button
   - Verify display wakes immediately
   - Verify menu opens
3. **Already On Test**:
   - Ensure display is on
   - Long-press wheel button
   - Verify menu opens normally
   - Verify no unnecessary wake occurs
4. **State Update Test**:
   - Turn display off
   - Wake via menu activation
   - Reboot device
   - Verify display stays on (state persisted)
5. **Functionality Test**:
   - With display on, navigate menu
   - Verify all inputs work normally

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No display wake
void MenuController::activate() {
    menuActive = true;
    // Display stays off if it was off
}

// ❌ WRONG - Wake in wrong place
void handleButtonPress() {
    display->setPower(true);  // Wakes on every button press
    // Should only wake on menu activation
}

// ❌ WRONG - No state update
void MenuController::activate() {
    display->setPower(true);
    menuActive = true;
    // NVS not updated, state lost on reboot
}

// ❌ WRONG - Blocking wait
void MenuController::activate() {
    display->setPower(true);
    delay(100);  // Unnecessary blocking
    menuActive = true;
}

// ❌ WRONG - Direct NVS access
void MenuController::activate() {
    display->setPower(true);
    preferences.putBool("display.power", true);  // Bypasses ConfigManager
}

// ✅ CORRECT - Wake on activation, update state
void MenuController::activate() {
    LOG_INFO("MenuController", "Activating menu");

    // Wake display
    display->setPower(true);

    // Update state in NVS
    Error result = configManager->setDisplayPower(true);
    if (result != Error::OK) {
        LOG_ERROR("MenuController", "Failed to update display power: %d", result);
    }

    // Activate menu
    menuActive = true;

    // Request menu display
    DisplayRequest req;
    req.type = DisplayRequest::DRAW_MENU;
    // ... populate request ...
    xQueueSend(displayQueue, &req, portMAX_DELAY);
}
```

### Display Wake Flow

```
Display Off State
    ↓
User Action: Long-press wheel button
    ↓
MenuController::activate()
    ↓
display->setPower(true)
    ↓
SSD1306_DISPLAYON command
    ↓
Display wakes (screen turns on)
    ↓
configManager->setDisplayPower(true)
    ↓
NVS updated (state persists)
    ↓
Menu opens
```

### References

- [Source: architecture/core-architectural-decisions.md#Command Pattern for Actions] - Action class structure
- [Source: architecture/implementation-patterns-consistency-rules.md#Format Patterns] - Error return pattern
- [Source: project-context.md#Configuration Persistence Gotchas] - NVS write validation
- [Source: epics.md#Story 9.6] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
