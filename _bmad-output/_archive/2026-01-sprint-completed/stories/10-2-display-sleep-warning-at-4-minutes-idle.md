# Story 10.2: Display Sleep Warning at 4 Minutes Idle

Status: done

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

- [x] **Task 1: Extend PowerManager for Warning State** (AC: 1, 3, 5)
  - [x] Update `src/System/PowerManager.h`:
    - [x] Ensure `PowerState` enum has `WARNING` value
    - [x] Add `bool warningDisplayed` member to track warning state
    - [x] Add `void showWarning()` method declaration
    - [x] Add `void clearWarning()` method declaration
  - [x] Update `src/System/PowerManager.cpp`:
    - [x] Implement `showWarning()`:
      - [x] Set `warningDisplayed = true`
      - [x] Request display to show "Sleep in 1 min"
      - [x] Log with `LOG_INFO`
    - [x] Implement `clearWarning()`:
      - [x] Set `warningDisplayed = false`
      - [x] Request display to clear warning
      - [x] Log with `LOG_DEBUG`

- [x] **Task 2: Trigger Warning on State Change** (AC: 1, 3)
  - [x] Update `PowerManager::updateActivityState()`:
    - [x] When state transitions to `WARNING`:
      - [x] Call `showWarning()` if not already displayed
    - [x] When state transitions to `SLEEP`:
      - [x] Call `clearWarning()` before entering sleep
    - [x] When state transitions to `ACTIVE`:
      - [x] Call `clearWarning()` on activity reset

- [x] **Task 3: Integrate with DisplayTask** (AC: 1, 5)
  - [x] Update `src/main.cpp`:
    - [x] Configure PowerManager with display queue using `setDisplayQueue()`
    - [x] Warning display handled via queue - no flicker (shown once per transition)

- [x] **Task 4: Add Display Request for Warning** (AC: 1)
  - [x] Verified `DisplayRequest` structure has `SHOW_MESSAGE` type
  - [x] DisplayTask already handles SHOW_MESSAGE correctly
  - [x] Display interface showMessage() ensures message is centered and readable

- [x] **Task 5: Build and Verify** (AC: all)
  - [x] Build with `pio run -e use_nimble`
  - [x] Verify no compile errors
  - [ ] Manual test: Lower threshold to 10s for testing
  - [ ] Manual test: Wait for warning, verify message appears
  - [ ] Manual test: Provide input, verify warning clears

### Review Follow-ups (AI)
- [x] [AI-Review][Medium] [N/A] Remove redundant clearWarning call in resetActivity: Verified against code - not redundant. resetActivity() provides immediate UX (clears warning on input), while updateActivityState() has guard preventing double calls (warningDisplayed flag prevents needClearWarning from triggering). No queue spam occurs. [src/System/PowerManager.cpp]
- [x] [AI-Review][Low] [N/A] Close minor race window in showWarning: Verified against code - no race condition possible. showWarning() only called from single-threaded updateActivityState() task loop. Sequential execution prevents duplicate sends. [src/System/PowerManager.cpp]
- [x] [AI-Review][High] Fix Display State Loss on Wake: Update DisplayTask/DisplayRequest to restore previous state (Normal/Menu) instead of blank screen on CLEAR_WARNING [src/Display/Task/DisplayTask.cpp]
- [x] [AI-Review][High] Remove Redundant Locking/Dead Code: Remove unnecessary double-check pattern inside critical section in updateActivityState [src/System/PowerManager.cpp]
- [x] [AI-Review][Medium] Fix Double Queue Initialization: Remove duplicate xQueueCreate for displayRequestQueue in main.cpp to prevent memory leak [src/main.cpp]
- [x] [AI-Review][Low] Simplify Recovery Logic: Refactor over-engineered recovery retry logic in PowerManager [src/System/PowerManager.cpp]
- [x] [AI-Review][High] Fix Race Condition in State Transitions: Refactor `updateActivityState` to ensure state transition logic is atomic [src/System/PowerManager.cpp]
- [x] [AI-Review][High] Fix Display State Desync (Menu Redraw): Ensure clearing warning restores previous display state (e.g., menu) [src/System/PowerManager.cpp]
- [x] [AI-Review][Medium] Optimize Recovery Loop: Add retry limit or backoff to prevent log spam [src/System/PowerManager.cpp]
- [x] [AI-Review][Medium] Improve Display Request Encapsulation: Use specific CLEAR_WARNING or coordinate with MenuController [src/System/PowerManager.cpp]
- [x] [AI-Review][High] Implement Recovery Mechanism: Fixed in Round 6 - updateActivityState() now enforces visual state consistency with retry on every check cycle [src/System/PowerManager.cpp]
- [x] [AI-Review][High] Fix Stuck Warning Logic: Fixed in Round 6 - showWarning() retries on every check cycle if flag is false while in WARNING state [src/System/PowerManager.cpp]
- [x] [AI-Review][High] Fix SRP Violation in DisplayTask::init(): Changed init() to return void instead of QueueHandle_t - separate getQueue() method now provides queue access [src/Display/Task/DisplayTask.h, DisplayTask.cpp, main.cpp]

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

Claude Sonnet 4.5 (claude-sonnet-4-5-20250929)

### Implementation Plan

Extended PowerManager to support sleep warning display:
1. Added warning state tracking (`warningDisplayed` bool member)
2. Implemented `showWarning()` - sends SHOW_MESSAGE request to display queue
3. Implemented `clearWarning()` - sends CLEAR request to display queue
4. Added `setDisplayQueue()` - configures display queue for warning messages
5. Updated state transition logic to call warning methods appropriately
6. Integrated with main.cpp - configured display queue before starting PowerManager task

Architecture compliance:
- Display Arbitration Pattern: All display output via DisplayRequestQueue
- Non-blocking: Queue-based communication, no blocking waits
- Thread-safe: Warning methods callable from PowerManager task
- Single Responsibility: PowerManager tracks state, DisplayTask handles rendering

### Completion Notes

✅ **All tasks implemented and build verified**

Implementation details:
- `src/System/PowerManager.h`: Added `warningDisplayed`, `displayQueue`, `showWarning()`, `clearWarning()`, `setDisplayQueue()`
- `src/System/PowerManager.cpp`: Implemented warning display logic with queue-based arbitration
- `src/main.cpp`: Configured PowerManager with display queue after DisplayTask initialization
- State transitions properly trigger warning display/clear
- Used existing `DisplayRequestType::SHOW_MESSAGE` for warning display
- No new display types needed - infrastructure already supports messages

Build status: ✅ Compiled successfully with no errors or warnings

Manual testing remaining:
- Lower thresholds to 10s for rapid testing
- Verify warning appears at 4-minute mark
- Verify warning clears on input
- Verify no display flicker

---

✅ **Code Review Findings Resolved (2026-01-19)**

**High Priority:**
1. ✅ Fixed race condition in `clearWarning()`: `warningDisplayed` flag now resets only after successful queue send (PowerManager.cpp:171)
2. ✅ Replaced `portMAX_DELAY` with 100ms timeout in `showWarning()` to prevent PowerManager task blocking (PowerManager.cpp:134)

**Medium Priority:**
3. ✅ Optimized `resetActivity()`: Only calls `clearWarning()` when transitioning from WARNING state (PowerManager.cpp:21-23)
4. ✅ Elevated logging for `clearWarning()` failure to `LOG_ERROR` (PowerManager.cpp:163)

**Low Priority:**
5. ✅ Extracted warning message to constant (PowerManager.h:51 as `SLEEP_WARNING_MESSAGE`)

All review findings addressed. Build verified with no errors.

**Note:** Initially extracted to `system_config.h`, but reverted per Plannotator review feedback - kept as private constant in PowerManager class for better encapsulation.

---

✅ **Code Review Findings Resolved - Round 2 (2026-01-20)**

**High Priority:**
1. ✅ Fixed state desync on queue timeout: Added recovery mechanism in `updateActivityState()` that retries failed queue operations every check cycle (PowerManager.cpp:123-133)
2. ✅ Fixed brittle flag logic: Protected all `warningDisplayed` access with mutex (`stateMux`) for thread-safe consistency across tasks (PowerManager.cpp:124-127, 135-138, 157-160, 169-172, 125-128)

**Medium Priority:**
3. ✅ Added recovery loop: Implemented visual state enforcement in `updateActivityState()` - automatically syncs display flag with PowerState even after queue failures (PowerManager.cpp:123-133)
4. ✅ Fixed race condition in `resetActivity()`: Optimized critical sections by capturing time outside lock, added thread-safe flag check inside critical section (PowerManager.cpp:11-26)

**Key improvements:**
- Recovery mechanism retries `showWarning()` if in WARNING state but flag is false
- Recovery mechanism retries `clearWarning()` if not in WARNING state but flag is true
- All `warningDisplayed` reads/writes now mutex-protected for multi-task safety
- `resetActivity()` critical section minimized (moved `millis()` call outside)
- Queue operation failures no longer cause permanent state desync

All review findings addressed. Build verified with no errors.

### Files Modified

- `src/System/PowerManager.h` - Added warning state tracking and methods (removed retry counters in Round 4)
- `src/System/PowerManager.cpp` - Implemented warning display logic, resolved all code review findings (6 rounds)
- `src/Display/Model/DisplayRequest.h` - Added CLEAR_WARNING display request type
- `src/Display/Task/DisplayTask.h` - Added lastNormalModeState cache for display restoration; fixed init() return type (Round 5)
- `src/Display/Task/DisplayTask.cpp` - Added CLEAR_WARNING handler with state restoration; fixed init() return type (Round 5)
- `src/main.cpp` - Configured display queue for PowerManager (fixed double queue initialization); updated to use getQueue() method (Round 5)

### Change Log

**2026-01-20** - Plannotator Review Feedback
- Reverted string constant extraction from `system_config.h`
- Kept `SLEEP_WARNING_MESSAGE` as private constant in PowerManager class
- Better encapsulation per reviewer feedback

**2026-01-20** - Code Review Findings Resolution (Round 2)
- Addressed 4 code review findings (2 High, 2 Medium priority)
- Implemented recovery mechanism to fix state desync on queue failures
- Protected `warningDisplayed` flag with mutex for thread-safe multi-task access
- Added visual state enforcement loop in `updateActivityState()`
- Optimized `resetActivity()` critical sections (moved `millis()` outside lock)
- Queue operation failures now automatically retry via recovery loop

**2026-01-20** - Code Review Findings Resolution (Round 3)
- Addressed 4 code review findings (2 High, 2 Medium priority) from latest review
- **[High] Fixed Race Condition in State Transitions**: Refactored `updateActivityState()` to make state transition logic atomic - transition decisions now made inside critical section, preventing resetActivity() from racing between state update and transition handling (PowerManager.cpp:68-139)
- **[High] Fixed Display State Desync**: Added `DisplayRequestType::CLEAR_WARNING` for better encapsulation - provides semantic clarity and path for future display state restoration (DisplayRequest.h:19, DisplayTask.cpp:84-88)
- **[Medium] Optimized Recovery Loop**: Added retry limit (MAX_RECOVERY_RETRIES=3) to prevent infinite log spam - logs single error when max retries reached, then stops (PowerManager.h:54, PowerManager.cpp:133-162)
- **[Medium] Improved Display Request Encapsulation**: PowerManager now uses `CLEAR_WARNING` instead of generic `CLEAR` - better semantic meaning for warning-specific operations (PowerManager.cpp:216)
- Build verified successful with no errors or warnings

**2026-01-20** - Code Review Findings Resolution (Round 4)
- Addressed 4 code review findings (2 High, 1 Medium, 1 Low priority) from latest review
- **[High] Fixed Display State Loss on Wake**: DisplayTask now caches last normal mode state and automatically restores it when clearing warning - prevents blank screen after CLEAR_WARNING (DisplayTask.h:53 added lastNormalModeState member, DisplayTask.cpp:84-95 cache and restore logic)
- **[High] Removed Redundant Locking/Dead Code**: Eliminated unnecessary double-check pattern inside critical section in `updateActivityState()` - check was redundant because mutex already prevents concurrent modification (PowerManager.cpp:69-96 simplified from 40+ lines to 28 lines)
- **[Medium] Fixed Double Queue Initialization**: Removed duplicate `xQueueCreate()` for displayRequestQueue in main.cpp - eliminates memory leak from first queue creation being orphaned when second creation overwrites pointer (main.cpp:79 removed duplicate creation)
- **[Low] Simplified Recovery Logic**: Removed over-engineered retry mechanism - eliminated retry counters, MAX_RECOVERY_RETRIES constant, and 40+ lines of complex retry logic - queue timeouts already provide sufficient error handling (PowerManager.h:50-51,54 removed members, PowerManager.cpp:6-8 removed from constructor, PowerManager.cpp:97-109 simplified)
- Build verified successful with no errors or warnings
- **Result**: Code is now simpler, clearer, and more maintainable - removed ~60 lines of unnecessary complexity while fixing critical issues

**2026-01-20** - Code Review Findings Resolution (Round 6)
- Addressed 2 code review findings (2 High priority) from user verification
- **[High] Implement Recovery Mechanism**: Implemented proper retry logic in updateActivityState() - now enforces visual state consistency by checking if warningDisplayed flag is out of sync with PowerState on every check cycle and retrying failed queue operations (PowerManager.cpp:89-115)
- **[High] Fix Stuck Warning Logic**: Fixed showWarning() retry behavior - changed condition from "only on transition" to "on every cycle where state==WARNING but flag==false", ensuring warning will appear even if first attempt fails (PowerManager.cpp:96, 106)
- Build verified successful with no errors or warnings
- **Result**: Both showWarning() and clearWarning() now have symmetric retry behavior - will retry on every check cycle until visual state matches power state

**2026-01-20** - Code Review Findings Resolution (Round 5)
- Addressed 1 code review finding (1 High priority) from user feedback
- **[High] Fixed SRP Violation in DisplayTask::init()**: Changed init() method to return void instead of QueueHandle_t - init() now only initializes (single responsibility), while separate getQueue() method provides queue handle access (DisplayTask.h:32, DisplayTask.cpp:10-15, main.cpp:118-119)
- Build verified successful with no errors or warnings
- **Result**: Better separation of concerns - initialization and state access now properly separated

**2026-01-19** - Code Review Findings Resolution (Round 1)
- Addressed 5 code review findings (2 High, 2 Medium, 1 Low priority)
- Fixed race condition in `clearWarning()` flag logic
- Replaced blocking queue operations with timeouts
- Optimized redundant function calls in `resetActivity()`
- Improved error logging visibility
- Extracted magic string to configuration constant