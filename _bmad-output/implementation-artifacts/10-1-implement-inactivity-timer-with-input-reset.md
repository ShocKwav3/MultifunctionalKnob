# Story 10.1: Implement Inactivity Timer with Input Reset

Status: ready-for-dev

## Story

As a **user actively using the device**,
I want **any input (wheel rotation, button press) to reset the inactivity timer**,
so that **the device stays awake while I'm using it**.

## Acceptance Criteria

1. **Given** the device is running
   **When** an inactivity timer is tracked
   **Then** it increments over time
   **And** timer is accurate to within 100ms

2. **Given** the timer is running
   **When** I rotate the wheel or press any button
   **Then** the inactivity timer resets to 0
   **And** the 5-minute countdown starts fresh

3. **Given** BLE connection status
   **When** checking activity
   **Then** BLE connection itself does NOT count as activity
   **And** only physical inputs (wheel, buttons) reset the timer

4. **Given** the timer is implemented
   **When** I build the project
   **Then** build succeeds with no errors or warnings

5. **Given** the timer is running
   **When** multiple inputs occur rapidly
   **Then** timer resets correctly on each input
   **And** no race conditions occur

## Tasks

- [ ] **Task 1: Create PowerManager Class** (AC: 1, 5)
  - [ ] Create `src/System/PowerManager.h`:
    - [ ] Class `PowerManager` with private members:
      - [ ] `uint32_t lastActivityTime` - tracks last input timestamp
      - [ ] `uint32_t inactivityThreshold` - 5 minutes in ms (300000)
      - [ ] `PowerState` enum: ACTIVE, WARNING, SLEEP
      - [ ] `PowerState currentState` - current power state
    - [ ] Public methods:
      - [ ] `void resetActivity()` - called on any input
      - [ ] `void checkActivity()` - called in main loop
      - [ ] `PowerState getState() const` - returns current state
  - [ ] Create `src/System/PowerManager.cpp`:
    - [ ] Implement `resetActivity()`:
      - [ ] Set `lastActivityTime = millis()`
      - [ ] Set `currentState = ACTIVE`
      - [ ] Log with `LOG_DEBUG`
    - [ ] Implement `checkActivity()`:
      - [ ] Calculate elapsed time: `millis() - lastActivityTime`
      - [ ] If elapsed >= 4 minutes (240000ms): Set state to WARNING
      - [ ] If elapsed >= 5 minutes (300000ms): Set state to SLEEP
      - [ ] Return current state

- [ ] **Task 2: Instrument Input Handlers** (AC: 2, 3, 5)
  - [ ] Review `src/Event/Handler/EncoderEventHandler.cpp`:
    - [ ] Identify where encoder events are processed
    - [ ] Add `PowerManager* powerManager` member
    - [ ] Update constructor to accept `PowerManager*`
    - [ ] Call `powerManager->resetActivity()` in event handler
  - [ ] Review `src/Event/Handler/ButtonEventHandler.cpp`:
    - [ ] Identify where button events are processed
    - [ ] Add `PowerManager* powerManager` member
    - [ ] Update constructor to accept `PowerManager*`
    - [ ] Call `powerManager->resetActivity()` in event handler

- [ ] **Task 3: Integrate PowerManager in Main Loop** (AC: 1, 5)
  - [ ] Update `src/main.cpp`:
    - [ ] Instantiate `PowerManager` in setup()
    - [ ] Pass `PowerManager` to input handlers
    - [ ] In `loop()`, call `powerManager->checkActivity()`:
      - [ ] Check return value for state changes
      - [ ] Handle WARNING state (Story 10.2)
      - [ ] Handle SLEEP state (Story 10.3)

- [ ] **Task 4: Add Logging for Debugging** (AC: 1, 5)
  - [ ] In `PowerManager::resetActivity()`:
    - [ ] Log activity reset with `LOG_DEBUG`
  - [ ] In `PowerManager::checkActivity()`:
    - [ ] Log elapsed time with `LOG_DEBUG`
    - [ ] Log state transitions with `LOG_INFO`

- [ ] **Task 5: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Wait 1 minute, rotate wheel, verify timer resets
  - [ ] Manual test: Verify BLE connection doesn't reset timer

## Dev Notes

### Architecture Compliance

- **Manager Pattern**: `PowerManager` centralizes power management logic
- **Cross-Cutting Concern**: Input handlers need reference to `PowerManager` (or fire an event it listens to)
- **Non-Blocking**: Timer checks should be O(1) and non-blocking
- **Single Responsibility**: `PowerManager` only tracks time, doesn't handle sleep logic (Story 10.3)

### Power State Machine

```
ACTIVE ──[4 min idle]──> WARNING ──[1 min idle]──> SLEEP
    │                              │
    └─[any input]─────────────────────┘
```

### PowerManager Class Structure

```cpp
// src/System/PowerManager.h
#pragma once
#include <Arduino.h>

class PowerManager {
public:
    enum class PowerState : uint8_t {
        ACTIVE = 0,
        WARNING = 1,
        SLEEP = 2
    };

    PowerManager();
    void resetActivity();
    PowerState checkActivity();
    PowerState getState() const;

private:
    uint32_t lastActivityTime;
    static constexpr uint32_t WARNING_THRESHOLD_MS = 240000;  // 4 minutes
    static constexpr uint32_t SLEEP_THRESHOLD_MS = 300000;    // 5 minutes
    PowerState currentState;
};
```

### Timer Implementation

```cpp
// src/System/PowerManager.cpp
void PowerManager::resetActivity() {
    lastActivityTime = millis();
    currentState = PowerState::ACTIVE;
    LOG_DEBUG("PowerManager", "Activity reset");
}

PowerManager::PowerState PowerManager::checkActivity() {
    uint32_t elapsed = millis() - lastActivityTime;

    if (elapsed >= SLEEP_THRESHOLD_MS) {
        currentState = PowerState::SLEEP;
        LOG_INFO("PowerManager", "Sleep threshold reached: %lu ms", elapsed);
    } else if (elapsed >= WARNING_THRESHOLD_MS) {
        currentState = PowerState::WARNING;
        LOG_DEBUG("PowerManager", "Warning threshold reached: %lu ms", elapsed);
    } else {
        currentState = PowerState::ACTIVE;
    }

    return currentState;
}
```

### Input Handler Integration

```cpp
// src/Event/Handler/EncoderEventHandler.cpp
class EncoderEventHandler {
private:
    PowerManager* powerManager;  // NEW: Dependency injection

public:
    EncoderEventHandler(AppEventDispatcher* dispatcher, PowerManager* pm)
        : dispatcher(dispatcher), powerManager(pm) {}

    void handleEvent(const EncoderInputEvent& event) override {
        // Reset activity on any encoder input
        powerManager->resetActivity();

        // ... existing event handling ...
    }
};

// src/Event/Handler/ButtonEventHandler.cpp
class ButtonEventHandler {
private:
    PowerManager* powerManager;  // NEW: Dependency injection

public:
    ButtonEventHandler(AppEventDispatcher* dispatcher, PowerManager* pm)
        : dispatcher(dispatcher), powerManager(pm) {}

    void handleEvent(const ButtonEvent& event) override {
        // Reset activity on any button press
        powerManager->resetActivity();

        // ... existing event handling ...
    }
};
```

### Main Loop Integration

```cpp
// src/main.cpp
PowerManager powerManager;

void setup() {
    // ... existing setup ...

    // Initialize power manager
    powerManager = PowerManager();
}

void loop() {
    // Check inactivity timer
    PowerState state = powerManager.checkActivity();

    switch (state) {
        case PowerState::ACTIVE:
            // Normal operation
            break;
        case PowerState::WARNING:
            // Show warning (Story 10.2)
            break;
        case PowerState::SLEEP:
            // Enter deep sleep (Story 10.3)
            break;
    }

    // ... existing loop code ...
}
```

### Existing Code Locations

```
src/System/PowerManager.h/cpp           - New power management class
src/Event/Handler/EncoderEventHandler.cpp  - Hook encoder input
src/Event/Handler/ButtonEventHandler.cpp  - Hook button input
src/main.cpp                              - Main loop integration
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/System/PowerManager.h` | New power management class header |
| `src/System/PowerManager.cpp` | New power management class implementation |
| `src/Event/Handler/EncoderEventHandler.h` | Add PowerManager dependency |
| `src/Event/Handler/EncoderEventHandler.cpp` | Call resetActivity() on encoder input |
| `src/Event/Handler/ButtonEventHandler.h` | Add PowerManager dependency |
| `src/Event/Handler/ButtonEventHandler.cpp` | Call resetActivity() on button input |
| `src/main.cpp` | Instantiate PowerManager and call checkActivity() |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Timer Reset Test**:
   - Wait 1 minute
   - Rotate wheel
   - Verify timer resets (check logs)
3. **Multiple Input Test**:
   - Rapidly press buttons and rotate wheel
   - Verify timer resets correctly on each input
4. **BLE Connection Test**:
   - Connect to BLE device
   - Wait 1 minute
   - Verify timer doesn't reset (BLE doesn't count as activity)
5. **Threshold Test**:
   - Wait 4 minutes
   - Verify state changes to WARNING
   - Wait 1 more minute
   - Verify state changes to SLEEP

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Blocking wait
void checkActivity() {
    while (millis() - lastActivityTime < 300000) {
        delay(100);  // Blocks entire system!
    }
}

// ❌ WRONG - BLE counts as activity
void handleBLEEvent() {
    powerManager->resetActivity();  // Wrong! BLE doesn't count
}

// ❌ WRONG - No state tracking
void resetActivity() {
    lastActivityTime = millis();
    // No state update
}

// ❌ WRONG - Wrong threshold
static constexpr uint32_t SLEEP_THRESHOLD_MS = 5000;  // 5 seconds, too short

// ❌ WRONG - Dynamic allocation
PowerManager* pm = new PowerManager();  // Never do this

// ✅ CORRECT - Non-blocking, input-only, state tracking
void PowerManager::resetActivity() {
    lastActivityTime = millis();
    currentState = PowerState::ACTIVE;
    LOG_DEBUG("PowerManager", "Activity reset");
}

PowerState PowerManager::checkActivity() {
    uint32_t elapsed = millis() - lastActivityTime;

    if (elapsed >= SLEEP_THRESHOLD_MS) {
        currentState = PowerState::SLEEP;
    } else if (elapsed >= WARNING_THRESHOLD_MS) {
        currentState = PowerState::WARNING;
    } else {
        currentState = PowerState::ACTIVE;
    }

    return currentState;
}
```

### Time Threshold Reference

| State | Threshold | Time | Purpose |
|-------|-----------|------|---------|
| ACTIVE | 0-4 min | Normal operation |
| WARNING | 4-5 min | Show warning to user |
| SLEEP | 5+ min | Enter deep sleep |

### References

- [Source: architecture/core-architectural-decisions.md#Communication Patterns] - Event pipeline architecture
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#ESP32-C3 Specific Gotchas] - Single-core constraints
- [Source: epics.md#Story 10.1] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
