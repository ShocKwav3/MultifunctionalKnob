# Story 10.2: Display Sleep Warning at 4 Minutes Idle

Status: ready-for-dev

## Story

As a **user who has been idle**,
I want **to see a warning 1 minute before the device sleeps**,
so that **I can interact to prevent sleep if needed**.

## Acceptance Criteria

1. **Given** the inactivity timer reaches 4 minutes
   **When** the check occurs
   **Then** the display shows "Sleep in 1 min"
   **And** the warning remains visible until input or sleep

2. **Given** warning is displayed
   **When** user provides input
   **Then** warning is dismissed
   **And** timer resets (handled by Story 10.1)

3. **Given** warning is displayed
   **When** 5-minute threshold is reached
   **Then** device enters deep sleep (handled by Story 10.3)
   **And** warning is cleared

4. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

5. **Given** warning is active
   **When** display refreshes
   **Then** warning remains visible
   **And** no flickering occurs

## Tasks

- [ ] **Task 1: Extend PowerManager for Warning State** (AC: 1, 3, 5)
  - [ ] Update `src/System/PowerManager.h`:
    - [ ] Ensure `PowerState` enum has `WARNING` value
    - [ ] Add `bool warningDisplayed` member to track warning state
    - [ ] Add `void showWarning()` method declaration
    - [ ] Add `void clearWarning()` method declaration
  - [ ] Update `src/System/PowerManager.cpp`:
    - [ ] Implement `showWarning()`:
      - [ ] Set `warningDisplayed = true`
      - [ ] Request display to show "Sleep in 1 min"
      - [ ] Log with `LOG_INFO`
    - [ ] Implement `clearWarning()`:
      - [ ] Set `warningDisplayed = false`
      - [ ] Request display to clear warning
      - [ ] Log with `LOG_DEBUG`

- [ ] **Task 2: Trigger Warning on State Change** (AC: 1, 3)
  - [ ] Update `PowerManager::checkActivity()`:
    - [ ] When state transitions to `WARNING`:
      - [ ] Call `showWarning()` if not already displayed
    - [ ] When state transitions to `SLEEP`:
      - [ ] Call `clearWarning()` before entering sleep
    - [ ] When state transitions to `ACTIVE`:
      - [ ] Call `clearWarning()` on activity reset

- [ ] **Task 3: Integrate with DisplayTask** (AC: 1, 5)
  - [ ] Update `src/main.cpp` loop():
    - [ ] When `PowerManager::checkActivity()` returns `WARNING`:
      - [ ] Request display to show warning message
      - [ ] Ensure warning doesn't flicker (only show once)

- [ ] **Task 4: Add Display Request for Warning** (AC: 1)
  - [ ] Update `DisplayRequest` structure if needed:
    - [ ] Add `SHOW_WARNING` type or use existing `SHOW_STATUS`
  - [ ] In `OLEDDisplay::showStatus()`:
    - [ ] Handle "Sleep in 1 min" message
    - [ ] Ensure message is centered and readable

- [ ] **Task 5: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Lower threshold to 10s for testing
  - [ ] Manual test: Wait for warning, verify message appears
  - [ ] Manual test: Provide input, verify warning clears

## Dev Notes

### Architecture Compliance

- **State Machine**: `PowerManager` manages `Active -> Warning -> Sleep` transitions
- **Display Arbitration**: Use `DisplayRequestQueue` for thread-safe display updates
- **Non-Blocking**: Warning display should be immediate, no blocking waits
- **Single Responsibility**: `PowerManager` tracks state, `DisplayTask` handles rendering

### Power State Machine

```
ACTIVE ──[4 min idle]──> WARNING ──[1 min idle]──> SLEEP
    │                              │
    └─[any input]─────────────────────┘
```

### Warning Display Pattern

```cpp
// src/System/PowerManager.cpp
void PowerManager::showWarning() {
    if (warningDisplayed) {
        return;  // Already showing warning
    }

    warningDisplayed = true;

    // Request display to show warning
    DisplayRequest req;
    req.type = DisplayRequest::SHOW_STATUS;
    req.data.status.key = "Warning";
    req.data.status.value = "Sleep in 1 min";
    xQueueSend(displayQueue, &req, portMAX_DELAY);

    LOG_INFO("PowerManager", "Warning displayed");
}

void PowerManager::clearWarning() {
    if (!warningDisplayed) {
        return;  // No warning to clear
    }

    warningDisplayed = false;

    // Request display to clear warning
    DisplayRequest req;
    req.type = DisplayRequest::CLEAR;
    xQueueSend(displayQueue, &req, portMAX_DELAY);

    LOG_DEBUG("PowerManager", "Warning cleared");
}
```

### State Transition Logic

```cpp
// In PowerManager::checkActivity()
PowerState PowerManager::checkActivity() {
    uint32_t elapsed = millis() - lastActivityTime;
    PowerState newState = currentState;

    if (elapsed >= SLEEP_THRESHOLD_MS) {
        newState = PowerState::SLEEP;
    } else if (elapsed >= WARNING_THRESHOLD_MS) {
        newState = PowerState::WARNING;
    } else {
        newState = PowerState::ACTIVE;
    }

    // Handle state transitions
    if (newState != currentState) {
        handleStateTransition(currentState, newState);
        currentState = newState;
    }

    return currentState;
}

void PowerManager::handleStateTransition(PowerState oldState, PowerState newState) {
    switch (newState) {
        case PowerState::WARNING:
            if (oldState != PowerState::WARNING) {
                showWarning();
            }
            break;
        case PowerState::SLEEP:
            clearWarning();
            // Enter sleep logic (Story 10.3)
            break;
        case PowerState::ACTIVE:
            clearWarning();
            break;
    }
}
```

### Display Request Structure

```cpp
// From DisplayRequest.h
struct DisplayRequest {
    enum Type { DRAW_MENU, SHOW_STATUS, CLEAR, SHOW_WARNING } type;
    union {
        struct { const char* key; const char* value; } status;
        struct { const char* message; } warning;
        // ... other types
    } data;
};
```

### Existing Code Locations

```
src/System/PowerManager.h/cpp           - Power state management
src/Display/Task/DisplayTask.cpp          - Display arbitration
src/main.cpp                                    - Main loop integration
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/System/PowerManager.h` | Add warning state tracking and methods |
| `src/System/PowerManager.cpp` | Implement warning display logic |
| `src/main.cpp` | Handle WARNING state in main loop |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Warning Display Test**:
   - Lower threshold to 10s for testing
   - Wait for warning
   - Verify "Sleep in 1 min" message displays
3. **Warning Clear Test**:
   - With warning displayed, provide input
   - Verify warning clears
   - Verify timer resets
4. **Sleep Transition Test**:
   - Wait for sleep threshold
   - Verify warning clears before sleep
   - Verify device enters sleep
5. **No Flicker Test**:
   - Verify warning doesn't flicker on display refresh
   - Verify warning only shows once per warning cycle

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No state tracking
void checkActivity() {
    if (elapsed >= 240000) {
        showWarning();  // Shows warning every loop iteration!
    }
}

// ❌ WRONG - Blocking wait
void showWarning() {
    display.println("Sleep in 1 min");
    delay(60000);  // Blocks for 1 minute!
}

// ❌ WRONG - No clear on activity
void resetActivity() {
    lastActivityTime = millis();
    currentState = ACTIVE;
    // Warning not cleared, stays on screen
}

// ❌ WRONG - Wrong threshold
static constexpr uint32_t WARNING_THRESHOLD_MS = 60000;  // 1 minute, too short

// ❌ WRONG - Dynamic allocation
char* warningMsg = new char[20];  // Never do this

// ✅ CORRECT - State tracking, non-blocking, proper threshold
void PowerManager::showWarning() {
    if (warningDisplayed) {
        return;  // Already showing
    }

    warningDisplayed = true;

    DisplayRequest req;
    req.type = DisplayRequest::SHOW_STATUS;
    req.data.status.key = "Warning";
    req.data.status.value = "Sleep in 1 min";
    xQueueSend(displayQueue, &req, portMAX_DELAY);

    LOG_INFO("PowerManager", "Warning displayed");
}

void PowerManager::handleStateTransition(PowerState oldState, PowerState newState) {
    switch (newState) {
        case PowerState::WARNING:
            if (oldState != PowerState::WARNING) {
                showWarning();  // Only show once
            }
            break;
        case PowerState::SLEEP:
            clearWarning();  // Clear before sleep
            break;
        case PowerState::ACTIVE:
            clearWarning();  // Clear on activity
            break;
    }
}
```

### Time Threshold Reference

| State | Threshold | Time | Purpose |
|-------|-----------|------|---------|
| ACTIVE | 0-4 min | Normal operation |
| WARNING | 4-5 min | Show warning to user |
| SLEEP | 5+ min | Enter deep sleep |

### References

- [Source: architecture/core-architectural-decisions.md#Display Arbitration Pattern] - Display request queue usage
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#Non-Blocking Rules] - Use millis() instead of delay()
- [Source: epics.md#Story 10.2] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
