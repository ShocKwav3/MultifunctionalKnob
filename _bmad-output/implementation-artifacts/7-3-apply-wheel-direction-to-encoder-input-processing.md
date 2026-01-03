# Story 7.3: Apply Wheel Direction to Encoder Input Processing

Status: ready-for-dev

## Story

As a **user with reversed wheel direction configured**,
I want **the encoder rotation to respect my direction setting**,
so that **rotating clockwise scrolls/zooms/adjusts in my preferred direction**.

## Acceptance Criteria

1. **Given** wheel direction is set to "Reversed" in NVS
   **When** I rotate the encoder clockwise
   **Then** the system behaves as if it was rotated counter-clockwise (logic inverted)
   **And** the HID command sent reflects the inverted direction

2. **Given** wheel direction is set to "Normal" in NVS
   **When** I rotate the encoder in either direction
   **Then** the system behaves normally (no inversion)
   **And** the HID command sent matches the physical rotation

3. **Given** the direction setting applies
   **When** I use any wheel mode (Scroll, Volume, Zoom)
   **Then** the direction logic is consistent across all modes
   **And** each mode correctly interprets the inverted direction

4. **Given** the direction setting is changed via menu
   **When** I switch from Normal to Reversed (or vice versa)
   **Then** the new direction takes effect immediately
   **And** no reboot is required

5. **Given** the implementation is complete
   **When** I build the project
   **Then** the build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Audit EncoderModeManager Structure** (AC: 3)
  - [ ] Review `src/EncoderMode/Manager/EncoderModeManager.h` for current structure
  - [ ] Identify constructor signature and dependencies
  - [ ] Identify `handleEvent()` method implementation
  - [ ] Identify how events are passed to mode handlers
  - [ ] Document current event flow from encoder to mode handler

- [ ] **Task 2: Add ConfigManager Dependency to EncoderModeManager** (AC: 1, 2, 4)
  - [ ] Update `EncoderModeManager.h` constructor to accept `ConfigManager*`
  - [ ] Add `ConfigManager* configManager` member variable
  - [ ] Update `EncoderModeManager.cpp` constructor to store the pointer
  - [ ] Update instantiation in `main.cpp` to pass `ConfigManager` instance

- [ ] **Task 3: Implement Direction Inversion Logic** (AC: 1, 2)
  - [ ] In `EncoderModeManager::handleEvent()`, read wheel direction from config:
    ```cpp
    WheelDirection dir = configManager->getWheelDirection();
    ```
  - [ ] Create a copy of the event for processing:
    ```cpp
    EncoderInputEvent processedEvent = event;
    ```
  - [ ] If direction is `REVERSED`, invert the `event.direction`:
    ```cpp
    if (dir == WheelDirection::REVERSED) {
        processedEvent.direction = (event.direction == Direction::CW)
                                 ? Direction::CCW
                                 : Direction::CW;
    }
    ```
  - [ ] Pass `processedEvent` to the mode handler instead of original event

- [ ] **Task 4: Verify Consistency Across Modes** (AC: 3)
  - [ ] Test Scroll mode with Reversed direction:
     - [ ] Clockwise rotation should send scroll down
     - [ ] Counter-clockwise rotation should send scroll up
  - [ ] Test Volume mode with Reversed direction:
     - [ ] Clockwise rotation should send volume down
     - [ ] Counter-clockwise rotation should send volume up
  - [ ] Test Zoom mode with Reversed direction:
     - [ ] Clockwise rotation should send zoom out
     - [ ] Counter-clockwise rotation should send zoom in

- [ ] **Task 5: Test Immediate Effect** (AC: 4)
  - [ ] Set direction to Normal
  - [ ] Rotate encoder and verify behavior
  - [ ] Change direction to Reversed via menu
  - [ ] Rotate encoder and verify inverted behavior (no reboot)

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Set to Reversed, verify all modes invert correctly
  - [ ] Manual test: Set to Normal, verify all modes behave normally

## Dev Notes

### Architecture Compliance

- **DI Pattern**: Pass `ConfigManager` to `EncoderModeManager` constructor for testability
- **Event Processing**: Invert at the handler level, not the driver level (Driver should report physical reality)
- **Single Responsibility**: `EncoderModeManager` handles direction inversion before delegating to mode handlers
- **No Blocking**: Direction check is O(1) and non-blocking

### Event Flow Architecture

```
EncoderDriver (ISR)
    ↓
EncoderEventDispatcher
    ↓
EncoderEventQueue
    ↓
EncoderEventHandler
    ↓
EncoderModeManager::handleEvent()
    ↓ [Direction inversion here]
Mode Handler (Scroll/Volume/Zoom)
    ↓
BleKeyboard (HID output)
```

### Implementation Detail

```cpp
// src/EncoderMode/Manager/EncoderModeManager.cpp
void EncoderModeManager::handleEvent(const EncoderInputEvent& event) {
    // Create a copy for processing
    EncoderInputEvent processedEvent = event;

    // Apply direction inversion if configured
    WheelDirection dir = configManager->getWheelDirection();
    if (dir == WheelDirection::REVERSED) {
        // Invert direction: CW ↔ CCW
        processedEvent.direction = (event.direction == Direction::CW)
                                 ? Direction::CCW
                                 : Direction::CW;
    }

    // Pass processed event to current mode handler
    if (currentMode) {
        currentMode->handle(processedEvent);
    }
}
```

### Constructor Update Pattern

```cpp
// src/EncoderMode/Manager/EncoderModeManager.h
class EncoderModeManager {
private:
    ConfigManager* configManager;  // NEW: Dependency injection
    EncoderModeHandlerInterface* currentMode;
    // ... other members

public:
    EncoderModeManager(
        AppEventDispatcher* dispatcher,
        ConfigManager* configManager,  // NEW: Parameter
        EncoderModeSelector* selector
    );
    // ... other methods
};

// src/EncoderMode/Manager/EncoderModeManager.cpp
EncoderModeManager::EncoderModeManager(
    AppEventDispatcher* dispatcher,
    ConfigManager* configManager,
    EncoderModeSelector* selector
) : dispatcher(dispatcher),
    configManager(configManager),  // NEW: Store dependency
    selector(selector),
    currentMode(nullptr) {
    // ... initialization
}
```

### Main.cpp Update

```cpp
// src/main.cpp
// In setup() or initialization
ConfigManager configManager;
EncoderModeManager modeManager(
    &eventDispatcher,
    &configManager,  // NEW: Pass ConfigManager
    &modeSelector
);
```

### Existing Code Locations

```
src/EncoderMode/Manager/EncoderModeManager.h/cpp  - Add config dependency and inversion logic
src/EncoderMode/Handler/EncoderModeHandlerScroll.cpp  - Scroll mode (no changes needed)
src/EncoderMode/Handler/EncoderModeHandlerVolume.cpp  - Volume mode (no changes needed)
src/EncoderMode/Handler/EncoderModeHandlerZoom.cpp    - Zoom mode (no changes needed)
src/main.cpp  - Update instantiation
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/EncoderMode/Manager/EncoderModeManager.h` | Add ConfigManager* member and constructor parameter |
| `src/EncoderMode/Manager/EncoderModeManager.cpp` | Implement direction inversion in handleEvent() |
| `src/main.cpp` | Pass ConfigManager to EncoderModeManager constructor |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Normal Direction Test**:
   - Set direction to Normal
   - Rotate CW → Verify correct behavior for each mode
   - Rotate CCW → Verify correct behavior for each mode
3. **Reversed Direction Test**:
   - Set direction to Reversed
   - Rotate CW → Verify inverted behavior for each mode
   - Rotate CCW → Verify inverted behavior for each mode
4. **Immediate Effect Test**:
   - Change direction while device is running
   - Verify new direction takes effect without reboot
5. **Mode Consistency Test**:
   - Test all three modes (Scroll, Volume, Zoom)
   - Verify direction inversion works consistently

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Invert at driver level (reports wrong physical reality)
void EncoderDriver::read() {
    if (isReversed) {
        direction = (direction == CW) ? CCW : CW;  // Wrong place
    }
    // ...
}

// ❌ WRONG - Modify original event (breaks const correctness)
void EncoderModeManager::handleEvent(const EncoderInputEvent& event) {
    if (isReversed) {
        event.direction = (event.direction == CW) ? CCW : CW;  // Can't modify const
    }
    // ...
}

// ❌ WRONG - Read config every time (inefficient)
void EncoderModeManager::handleEvent(const EncoderInputEvent& event) {
    WheelDirection dir = configManager->getWheelDirection();  // OK, but could cache
    // ...
}

// ❌ WRONG - Invert in each mode handler (code duplication)
void ScrollHandler::handle(const EncoderInputEvent& event) {
    EncoderInputEvent processed = event;
    if (isReversed) { /* invert */ }  // Duplicated in Volume, Zoom
    // ...
}

// ✅ CORRECT - Invert once at manager level
void EncoderModeManager::handleEvent(const EncoderInputEvent& event) {
    EncoderInputEvent processedEvent = event;
    if (configManager->getWheelDirection() == WheelDirection::REVERSED) {
        processedEvent.direction = (event.direction == Direction::CW)
                                 ? Direction::CCW
                                 : Direction::CW;
    }
    currentMode->handle(processedEvent);  // Single inversion point
}
```

### Direction Mapping Reference

| Physical Rotation | Normal Direction | Reversed Direction |
|-------------------|------------------|-------------------|
| Clockwise (CW) | Scroll Up / Vol+ / Zoom In | Scroll Down / Vol- / Zoom Out |
| Counter-Clockwise (CCW) | Scroll Down / Vol- / Zoom Out | Scroll Up / Vol+ / Zoom In |

### References

- [Source: architecture/core-architectural-decisions.md#Communication Patterns] - Event pipeline architecture
- [Source: architecture/implementation-patterns-consistency-rules.md#Naming Patterns] - Naming conventions
- [Source: project-context.md#Event Architecture Rules] - Event flow and queue usage
- [Source: epics.md#Story 7.3] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
