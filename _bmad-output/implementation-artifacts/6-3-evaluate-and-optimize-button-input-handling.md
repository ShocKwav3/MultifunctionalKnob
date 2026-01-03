# Story 6.3: Evaluate and Optimize Button Input Handling

Status: in-progress

## Story

As a **developer maintaining the codebase**,
I want **button input handling reviewed for efficiency**,
so that **the code uses the simplest approach that works reliably**.

## Acceptance Criteria

1. **Given** buttons currently use `buttonManager->poll()` in `main.cpp::loop()`
   **When** I investigate whether event-driven input (like the wheel) is simpler
   **Then** I document the finding with a clear recommendation

2. **Given** the investigation is complete
   **When** event-driven is simpler
   **Then** implement event-driven approach

3. **Given** the investigation is complete
   **When** poll() is simpler
   **Then** keep poll() and document why

4. **Given** a decision is made
   **Then** the decision prioritizes simplicity over forced consistency

## Tasks

- [x] **Task 1: Document current implementation** (AC: 1)
  - [x] Review `src/Button/ButtonManager.cpp` poll() implementation
  - [x] Review `src/main.cpp` loop() button handling
  - [x] Document how encoder event-driven approach works for comparison

- [x] **Task 2: Analyze trade-offs** (AC: 1)
  - [x] **Poll-based in main loop advantages:**
    - Simpler implementation (no task creation)
    - No additional memory overhead
  - [x] **Poll-based in FreeRTOS task advantages:**
    - Consistent with encoder button pattern
    - Predictable 10ms timing
    - Clean main loop
  - [x] **Event-driven (ISR) advantages:**
    - Immediate response
    - Lower power (no continuous polling)
    - Most complex to implement

- [x] **Task 3: Make recommendation** (AC: 1, 4)
  - [x] Document decision with rationale
  - [x] Consider ESP32-C3 single-core constraint
  - [x] Prioritize simplicity (KISS principle)
  - [x] **DECISION: Move to dedicated FreeRTOS task** (Option C - consistent with encoder, simple implementation)

- [ ] **Task 4: Implement FreeRTOS task approach** (AC: 2, 3) - PENDING IMPLEMENTATION
  - [ ] Add static task function to `ButtonManager.h`
  - [ ] Modify `ButtonManager.cpp::init()` to create FreeRTOS task
  - [ ] Remove `buttonManager->poll()` from `main.cpp::loop()`
  - [ ] Update code comment to reflect FreeRTOS task approach

- [ ] **Task 5: Build and test** (AC: all) - PENDING IMPLEMENTATION
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Test button responsiveness (verify 10ms polling works correctly)
  - [ ] Verify no timing issues or task conflicts

## Dev Notes

### Current Implementation Analysis

**ButtonManager.poll() approach:**
```cpp
// main.cpp loop()
buttonManager->poll();  // Called every loop iteration
```

**EncoderDriver event-driven approach:**
```cpp
// Uses hardware interrupts via ESP32 PCNT
// Events dispatched to queue from ISR
```

### Decision Framework

| Factor | Poll-Based | Event-Driven |
|--------|------------|--------------|
| Complexity | Low | Medium (ISR handling) |
| Latency | ~10-50ms (loop dependent) | <1ms |
| Power | Higher (continuous polling) | Lower |
| Debugging | Easier | Harder (ISR context) |
| Single-Core Safety | Safer | Needs careful ISR design |

### Recommendation Guidance

**Keep Poll-Based IF:**
- Current latency is acceptable (<50ms per NFR1)
- No user complaints about button responsiveness
- Code is simpler and easier to maintain
- Single-core ESP32-C3 benefits from predictable execution

**Switch to Event-Driven IF:**
- Latency is unacceptable
- Power consumption is critical
- Consistency with encoder is more important than simplicity

### ESP32-C3 Considerations

- **Single-core RISC-V**: ISRs block main execution
- **GPIO interrupt capability**: Available but adds complexity
- **FreeRTOS integration**: Would need queue from ISR context

### Documentation Template

If keeping poll():
```cpp
// ButtonManager uses poll-based input handling.
// Decision rationale (Story 6.3):
// - Simpler than ISR-based approach
// - Acceptable latency for button inputs (~10-50ms)
// - Easier debugging and maintenance
// - Safe for single-core ESP32-C3
```

### References

- [Source: project-context.md#ESP32-C3 Specific Gotchas] - Single-core constraints
- [Source: project-context.md#Input Handling Gotchas] - Debounce handled by EncoderDriver
- [Source: architecture/core-architectural-decisions.md#Communication Patterns] - Event pipeline patterns

## Change Log

- **2026-01-03**: Evaluated button input handling approaches. Initial recommendation was to keep current poll-based approach in main loop. After user discussion, recommendation updated to move ButtonManager polling to dedicated FreeRTOS task for consistency with encoder button pattern. Story marked for continued implementation by another agent.

## Dev Agent Record

### Agent Model Used

Claude Opus 4.5

### Implementation Plan

**Task 1-3: Investigation and Analysis**

Reviewed current implementations:
- **ButtonManager.cpp/h**: Poll-based, called in `main.cpp::loop()` continuously
- **EncoderDriver**: Hybrid approach - ISR for rotation (PCNT hardware), **polling for button in dedicated FreeRTOS task every 10ms**
- **Key insight**: Encoder button is also polled (not ISR-driven), just in a FreeRTOS task instead of main loop

**Current Implementation Details:**

```cpp
// EncoderDriver.cpp - Encoder button polling
void EncoderDriver::encoderTask(void* pvParameters) {
    while (true) {
        if (encoderDriverInstance) {
            encoderDriverInstance->runLoop();  // Calls handleButton()
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Polls every 10ms
    }
}
```

```cpp
// main.cpp - Four buttons polling
void loop() {
    if (buttonManager) {
        buttonManager->poll();  // Polls continuously in main loop
    }
}
```

**Task 4: Architecture Options Evaluated**

After discussion with user, three options were identified:

**Option A: Keep Current Approach (Initial Recommendation - REJECTED)**
- Four buttons: Polled in `main.cpp::loop()`
- Encoder button: Polled in FreeRTOS task
- **Con**: Inconsistent - two different polling mechanisms
- **User feedback**: Not acceptable due to inconsistency

**Option B: Consolidate Into Encoder Task (Considered - REJECTED)**
- Move four buttons into same FreeRTOS task as encoder
- **Con**: Couples button and encoder concerns together
- **User feedback**: Not the desired approach

**Option C: Separate FreeRTOS Task for Buttons (RECOMMENDED BY USER)**
- **Encoder button**: Stays in `EncoderDriver` FreeRTOS task (no change)
- **Four buttons**: Move to **dedicated FreeRTOS task** in `ButtonManager`
- **Pro**: Consistent pattern - both use FreeRTOS task polling independently
- **Pro**: Predictable 10ms timing, not dependent on main loop speed
- **Pro**: Clean separation of concerns
- **Pro**: Simple - no ISR complexity
- **Effort**: LOW (30-60 minutes estimated)

**Task 5: Recommendation**

**MOVE FOUR BUTTONS TO DEDICATED FREERTOS TASK** (Option C)

### Updated Implementation Approach

**Changes Required:**

1. **ButtonManager.h** - Add static task function declaration
2. **ButtonManager.cpp** - Modify `init()` to create FreeRTOS task
3. **main.cpp** - Remove `buttonManager->poll()` from `loop()`

**Effort Estimate:** LOW (~30-60 minutes)

**Code Pattern (Reference Implementation):**

```cpp
// ButtonManager.cpp - Proposed changes
void ButtonManager::init() {
    // Existing pin setup
    for (size_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(BUTTONS[i].pin, BUTTONS[i].activeLow ? INPUT_PULLUP : INPUT_PULLDOWN);
        buttonStates[i] = {false, false, 0};
    }
    
    // NEW: Create dedicated task for button polling
    xTaskCreate(
        buttonTask,           // Task function
        "ButtonTask",         // Name
        2048,                 // Stack size (match encoder pattern)
        this,                 // Pass ButtonManager instance
        1,                    // Priority (same as encoder)
        nullptr              // Task handle (not needed)
    );
}

// NEW: Static task function
void ButtonManager::buttonTask(void* pvParameters) {
    ButtonManager* manager = (ButtonManager*)pvParameters;
    while (true) {
        manager->poll();      // Existing poll logic unchanged
        vTaskDelay(pdMS_TO_TICKS(10));  // 10ms interval (match encoder)
    }
}

// poll() method - NO CHANGES NEEDED (already thread-safe via queue)
```

```cpp
// main.cpp - Remove from loop()
void loop() {
    // Empty or minimal - buttons now in dedicated task
}
```

**Benefits:**
- ✅ Consistent architecture - both encoder button and four buttons use FreeRTOS tasks
- ✅ Predictable timing - 10ms polling interval independent of main loop
- ✅ Clean main loop - becomes empty/minimal
- ✅ Simple implementation - no ISR complexity
- ✅ Thread-safe - dispatcher queues already handle cross-task communication

**Risks/Considerations:**
- ⚠️ Task overhead - One additional FreeRTOS task (~2KB stack)
- ⚠️ Thread safety - `eventDispatcher->onButtonPressed()` called from task context (should be fine - FreeRTOS queues are thread-safe)
- ⚠️ Initialization order - Task must start after dispatcher/queue setup in `main.cpp::setup()`

### Completion Notes

✅ Completed investigation of button input handling approaches
✅ Analyzed current implementations - discovered encoder button uses FreeRTOS task polling
✅ Evaluated three architecture options with user
✅ **RECOMMENDATION UPDATED**: Move four buttons to dedicated FreeRTOS task (Option C)
✅ Documented implementation approach with code patterns
✅ Initial documentation comment added (will need updating based on final implementation)

**Decision Summary:**
Move ButtonManager polling to dedicated FreeRTOS task to match encoder button pattern. This provides consistent architecture (both use task-based polling) while maintaining simplicity (no ISR complexity). Estimated effort: 30-60 minutes.

**Next Steps for Implementing Agent:**
1. Review proposed code changes above
2. Modify `ButtonManager.h` to add static task declaration
3. Modify `ButtonManager.cpp::init()` to create task
4. Remove `buttonManager->poll()` from `main.cpp::loop()`
5. Build and test button responsiveness
6. Update documentation comment in ButtonManager.cpp to reflect FreeRTOS task approach

### Files Modified (Investigation Phase)

- src/Button/ButtonManager.cpp (added TODO comment for implementing agent)

### Files Pending Modification (Implementation Phase)

- src/Button/ButtonManager.h (add static task function declaration)
- src/Button/ButtonManager.cpp (modify init() to create task, update TODO to final comment)
- src/main.cpp (remove buttonManager->poll() from loop())
