# Story 9.3: Implement Bluetooth Icon States (Solid/Flashing/None)

Status: ready-for-dev

## Story

As a **user checking Bluetooth connectivity**,
I want **the BT icon to indicate connection state visually**,
so that **I can tell at a glance if I'm connected, pairing, or disconnected**.

## Acceptance Criteria

1. **Given** Bluetooth is connected
   **When** rendering status
   **Then** BT icon displays solid (static, not blinking)
   **And** icon is clearly visible

2. **Given** Bluetooth is in pairing mode
   **When** rendering status
   **Then** BT icon flashes (blinks on/off at ~1Hz)
   **And** flashing is smooth and noticeable

3. **Given** Bluetooth is disconnected and not pairing
   **When** rendering status
   **Then** no BT icon is displayed
   **And** space is left blank or shows "No BT" text

4. **Given** BT state changes
   **When** connection state transitions
   **Then** icon state updates immediately
   **And** no manual refresh is required

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Extend SystemState for BT States** (AC: 1, 2, 3)
  - [ ] Update `src/Display/Model/SystemState.h`:
    - [ ] Ensure `bluetoothConnected` boolean exists
    - [ ] Ensure `bluetoothPairing` boolean exists
    - [ ] Document state transitions:
      - [ ] DISCONNECTED → PAIRING → CONNECTED
      - [ ] CONNECTED → DISCONNECTED
      - [ ] PAIRING → CONNECTED or DISCONNECTED (timeout)

- [ ] **Task 2: Implement Flashing Logic** (AC: 2)
  - [ ] Add flashing state tracking to `OLEDDisplay.h`:
    - [ ] `uint32_t lastFlashTime` member
    - [ ] `bool flashState` member
  - [ ] Implement flashing check in `drawNormalMode()`:
    ```cpp
    if (state.bluetoothPairing) {
        uint32_t now = millis();
        if (now - lastFlashTime >= 500) {  // 500ms = 1Hz
            flashState = !flashState;
            lastFlashTime = now;
        }
        if (flashState) {
            drawBitmap(BT_ICON_X, BT_ICON_Y, btIcon);
        }
    }
    ```

- [ ] **Task 3: Update Drawing Code for All States** (AC: 1, 2, 3)
  - [ ] Modify `OLEDDisplay::drawNormalMode()`:
    - [ ] If `bluetoothConnected`: Draw solid BT icon
    - [ ] If `bluetoothPairing`: Draw flashing BT icon (from Task 2)
    - [ ] If neither: Don't draw BT icon (leave blank)
    - [ ] Ensure icon position is consistent across all states

- [ ] **Task 4: Integrate with BLE State Tracking** (AC: 4)
  - [ ] Identify where BLE state changes are detected
  - [ ] Update `SystemState` when connection state changes
  - [ ] Trigger display refresh when state changes
  - [ ] Ensure state updates are thread-safe (use mutex or queue)

- [ ] **Task 5: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Disconnect → Verify no icon
  - [ ] Manual test: Start pairing → Verify icon flashes
  - [ ] Manual test: Connect → Verify solid icon

## Dev Notes

### Architecture Compliance

- **Non-Blocking**: Flashing shouldn't block execution. Use timestamp checks in the display refresh loop
- **State Tracking**: Single source of truth for BT state in `SystemState`
- **Display Arbitration**: Use `DisplayRequestQueue` for thread-safe updates
- **No Dynamic Allocation**: All timing variables must be stack-allocated

### BT State Machine

```
State Transitions:
DISCONNECTED ──[start pairing]──> PAIRING
    │                              │
    │                              └─[connect]──> CONNECTED
    │                              └─[timeout]──> DISCONNECTED
    │
    └─[disconnect]─────────────────────┘
```

### Flashing Implementation Pattern

```cpp
// Non-blocking flashing using millis()
class OLEDDisplay : public DisplayInterface {
private:
    uint32_t lastFlashTime = 0;
    bool flashState = false;
    static constexpr uint32_t FLASH_INTERVAL_MS = 500;  // 1Hz = 500ms

    void drawBluetoothIcon(bool connected, bool pairing) {
        if (connected) {
            // Solid icon
            drawBitmap(BT_ICON_X, BT_ICON_Y, btIcon);
        } else if (pairing) {
            // Flashing icon
            uint32_t now = millis();
            if (now - lastFlashTime >= FLASH_INTERVAL_MS) {
                flashState = !flashState;
                lastFlashTime = now;
            }
            if (flashState) {
                drawBitmap(BT_ICON_X, BT_ICON_Y, btIcon);
            }
        }
        // Else: disconnected, don't draw
    }
};
```

### Icon Position Reference

```
128x32 OLED Layout (from Story 9.2):
┌────────────────────────────────────────┐
│ [BT]                    [BAT]  │  y=0-7 (status bar)
└────────────────────────────────────────┘
  ↑
BT Icon Position: x=2, y=0 (8x8 icon)
```

### Display Update Flow

```
BLE State Change (connect/disconnect/pairing)
    ↓
Update SystemState.bluetoothConnected/bluetoothPairing
    ↓
Trigger DisplayTask refresh
    ↓
OLEDDisplay::drawNormalMode(state)
    ↓
Check BT state → Draw solid/flashing/none
```

### Existing Code Locations

```
src/Display/Model/SystemState.h           - BT state tracking
src/Display/Impl/OLEDDisplay.h/cpp          - Drawing logic
src/Display/Bitmaps.h                        - BT icon bitmap
src/Helper/BleKeyboard.cpp                   - BLE state changes
src/Display/Task/DisplayTask.cpp              - Display refresh
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Display/Model/SystemState.h` | Ensure BT state fields exist |
| `src/Display/Impl/OLEDDisplay.h` | Add flashing state members |
| `src/Display/Impl/OLEDDisplay.cpp` | Implement flashing logic and state-based rendering |
| `src/Helper/BleKeyboard.cpp` | Update SystemState on BLE changes |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Disconnected State Test**:
   - Ensure BLE is disconnected
   - Verify no BT icon displays
3. **Pairing State Test**:
   - Start BLE pairing
   - Verify BT icon flashes at ~1Hz
   - Verify flashing is smooth
4. **Connected State Test**:
   - Connect to a device
   - Verify BT icon displays solid
   - Verify no flashing
5. **State Transition Test**:
   - Disconnect → Pair → Connect
   - Verify icon transitions correctly at each step
6. **Non-Blocking Test**:
   - Verify system remains responsive during flashing
   - Verify no lag in other operations

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Blocking delay for flashing
void drawBluetoothIcon(bool pairing) {
    if (pairing) {
        while (true) {
            drawIcon();
            delay(500);  // Blocks entire system!
            clearIcon();
            delay(500);
        }
    }
}

// ❌ WRONG - Wrong flash rate
if (millis() % 1000 < 500) {  // 0.5Hz, too slow
    drawIcon();
}

// ❌ WRONG - No state tracking
void drawBluetoothIcon() {
    // Reads BLE state directly every frame
    // No single source of truth
}

// ❌ WRONG - Dynamic allocation
uint32_t* flashTimes = new uint32_t[10];  // Never do this

// ❌ WRONG - Flashing when connected
if (connected || pairing) {
    // Flashing should only happen during pairing
}

// ✅ CORRECT - Non-blocking with state tracking
void OLEDDisplay::drawBluetoothIcon(bool connected, bool pairing) {
    if (connected) {
        // Solid icon
        drawBitmap(BT_ICON_X, BT_ICON_Y, btIcon);
    } else if (pairing) {
        // Flashing icon (non-blocking)
        uint32_t now = millis();
        if (now - lastFlashTime >= FLASH_INTERVAL_MS) {
            flashState = !flashState;
            lastFlashTime = now;
        }
        if (flashState) {
            drawBitmap(BT_ICON_X, BT_ICON_Y, btIcon);
        }
    }
    // Else: disconnected, don't draw
}
```

### Flash Rate Reference

| Flash Rate | Interval (ms) | Use Case |
|-----------|----------------|-----------|
| 0.5 Hz | 1000 | Too slow, hard to notice |
| 1 Hz | 500 | **Recommended** - clearly visible |
| 2 Hz | 250 | Fast, may be distracting |
| 4 Hz | 125 | Too fast, looks like glitch |

### References

- [Source: architecture/core-architectural-decisions.md#Display Arbitration Pattern] - Display request queue usage
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#Non-Blocking Rules] - Use millis() instead of delay()
- [Source: epics.md#Story 9.3] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
