# Story 10.5: Auto-Reconnect Bluetooth After Wake

Status: ready-for-dev

## Story

As a **user waking the device from sleep**,
I want **the device to automatically reconnect to the previously paired Bluetooth host**,
so that **I can resume using it without manually re-pairing**.

## Acceptance Criteria

1. **Given** the device was connected to a BLE host before entering deep sleep
   **When** the device wakes from deep sleep
   **Then** the system automatically attempts to reconnect to the last paired device
   **And** reconnection happens without user intervention

2. **Given** reconnection attempt is successful
   **When** connection is established
   **Then** the display shows connection status during reconnection
   **And** the display shows "Connected" when complete

3. **Given** reconnection fails (timeout)
   **When** 5 seconds pass without connection
   **Then** the device enters advertising/pairing mode
   **And** the display shows "Pairing..." status

4. **Given** device wakes from sleep
   **When** BLE initialization occurs
   **Then** reconnection attempt starts automatically
   **And** no manual menu navigation is required

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Verify NimBLE/BleKeyboard Auto-Reconnect Behavior** (AC: 1, 4)
  - [ ] Review BleKeyboard library (external PlatformIO dependency in `.pio/`) for existing methods
  - [ ] Check if `begin()` or `startAdvertising()` handles auto-reconnect
  - [ ] Verify if NimBLE stores bonding data automatically
  - [ ] Document current reconnection behavior

- [ ] **Task 2: Add Reconnection State Tracking** (AC: 2, 3)
  - [ ] Update `src/System/PowerManager.h`:
    - [ ] Add `bool wasConnectedBeforeSleep` member
    - [ ] Add `void setWasConnected(bool connected)` method
  - [ ] Update `src/System/PowerManager.cpp`:
    - [ ] Implement `setWasConnected()`:
      - [ ] Store connection state before sleep
      - [ ] Log with `LOG_DEBUG`

- [ ] **Task 3: Trigger Reconnection on Wake** (AC: 1, 4)
  - [ ] Update `src/main.cpp`:
    - [ ] In `setup()`:
      - [ ] Detect wake cause using `esp_sleep_get_wakeup_cause()`
      - [ ] If wake from deep sleep AND `wasConnectedBeforeSleep`:
        - [ ] Call `bleKeyboard->begin()` or `startAdvertising()`
        - [ ] Log reconnection attempt with `LOG_INFO`
      - [ ] If wake from deep sleep AND NOT `wasConnectedBeforeSleep`:
        - [ ] Skip reconnection (wasn't connected before sleep)

- [ ] **Task 4: Add Reconnection Status Display** (AC: 2)
  - [ ] Update `src/main.cpp`:
    - [ ] During reconnection attempt:
      - [ ] Request display to show "Reconnecting..."
    - [ ] When connection established:
      - [ ] Request display to show "Connected"
    - [ ] When timeout occurs:
      - [ ] Request display to show "Pairing..."

- [ ] **Task 5: Handle Reconnection Timeout** (AC: 3)
  - [ ] Update `src/main.cpp`:
    - [ ] Add timeout tracking for reconnection
    - [ ] If 5 seconds pass without connection:
      - [ ] Enter advertising mode
      - [ ] Request display to show "Pairing..."
      - [ ] Log timeout with `LOG_INFO`

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Connect to device, enter sleep, wake, verify auto-reconnect
  - [ ] Manual test: Verify reconnection timeout enters pairing mode

## Dev Notes

### Architecture Compliance

- **Standard Library Behavior**: Rely on NimBLE's built-in bonding and reconnection features
- **State Management**: Track connection state before sleep to determine if reconnection is needed
- **Display Arbitration**: Use `DisplayRequestQueue` for thread-safe display updates
- **Non-Blocking**: Reconnection should be asynchronous, no blocking waits

### NimBLE Auto-Reconnect Behavior

```cpp
// NimBLE typically handles auto-reconnect automatically:
// 1. Bonding data is stored in NVS
// 2. On wake, NimBLE attempts to reconnect to last bonded device
// 3. If reconnection succeeds, device is connected
// 4. If reconnection fails (timeout), device enters advertising mode
```

### Wake Cause Detection

```cpp
// Detect wake cause after deep sleep
esp_sleep_wakeup_cause_t wakeCause = esp_sleep_get_wakeup_cause();

switch (wakeCause) {
    case ESP_SLEEP_WAKEUP_EXT0:
        LOG_INFO("PowerManager", "Woke up by EXT0 (wheel button)");
        // Trigger reconnection if was connected before sleep
        if (powerManager->wasConnectedBeforeSleep()) {
            LOG_INFO("BLE", "Attempting auto-reconnect");
            bleKeyboard->begin();  // Triggers reconnection
        }
        break;
    // ... other cases
}
```

### Reconnection Flow

```
Device Connected Before Sleep
    ↓
Enter Deep Sleep (Story 10.3)
    ↓
Store: wasConnectedBeforeSleep = true
    ↓
[User presses wheel button]
    ↓
Device Wakes
    ↓
Detect Wake Cause
    ↓
If wasConnectedBeforeSleep:
    ↓
bleKeyboard->begin()
    ↓
NimBLE Auto-Reconnect
    ├─ Success → Display: "Connected"
    └─ Timeout (5s) → Display: "Pairing..."
```

### State Tracking Pattern

```cpp
// src/System/PowerManager.h
class PowerManager {
public:
    // ... existing methods ...

    void setWasConnected(bool connected) {
        wasConnectedBeforeSleep = connected;
        LOG_DEBUG("PowerManager", "Set wasConnectedBeforeSleep: %d", connected);
    }

    bool wasConnectedBeforeSleep() const {
        return wasConnectedBeforeSleep;
    }

private:
    // ... existing members ...
    bool wasConnectedBeforeSleep;
};
```

### Existing Code Locations

```
src/System/PowerManager.h/cpp           - Connection state tracking
BleKeyboard (external library in .pio/) - BLE reconnection (begin, startAdvertising, isConnected)
src/main.cpp                            - Wake detection and reconnection trigger
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/System/PowerManager.h` | Add `wasConnectedBeforeSleep` tracking |
| `src/System/PowerManager.cpp` | Implement connection state methods |
| `src/main.cpp` | Add wake detection and reconnection trigger |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Auto-Reconnect Test**:
   - Connect to a device
   - Enter deep sleep
   - Wake device
   - Verify auto-reconnection attempt starts
   - Verify "Connected" displays when successful
3. **Timeout Test**:
   - Connect to a device
   - Enter deep sleep
   - Wake device
   - Disconnect the paired device (simulate timeout)
   - Verify "Pairing..." displays after 5 seconds
4. **No Reconnection Test**:
   - Ensure device is NOT connected
   - Enter deep sleep
   - Wake device
   - Verify no reconnection attempt occurs
5. **State Tracking Test**:
   - Verify `wasConnectedBeforeSleep` is set correctly
   - Verify reconnection only happens when appropriate

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No state tracking
void setup() {
    // Always attempts reconnection on wake
    bleKeyboard->begin();  // Even if wasn't connected before sleep
}

// ❌ WRONG - Blocking wait for reconnection
void setup() {
    bleKeyboard->begin();
    while (!bleKeyboard->isConnected()) {
        delay(100);  // Blocks entire system!
    }
}

// ❌ WRONG - No timeout handling
void setup() {
    bleKeyboard->begin();
    // Waits indefinitely for connection
    // No fallback to pairing mode
}

// ❌ WRONG - Wrong wake cause
void setup() {
    // Doesn't check wake cause
    bleKeyboard->begin();  // Reconnects on every wake
}

// ❌ WRONG - Dynamic allocation
char* reconnectMsg = new char[50];  // Never do this

// ✅ CORRECT - State tracking, non-blocking, timeout handling
void setup() {
    esp_sleep_wakeup_cause_t wakeCause = esp_sleep_get_wakeup_cause();

    switch (wakeCause) {
        case ESP_SLEEP_WAKEUP_EXT0:
            LOG_INFO("PowerManager", "Woke up by EXT0");

            // Only reconnect if was connected before sleep
            if (powerManager->wasConnectedBeforeSleep()) {
                LOG_INFO("BLE", "Attempting auto-reconnect");
                bleKeyboard->begin();  // NimBLE handles reconnection
            }
            break;
        // ... other cases
    }
}
```

### Reconnection Timeout Reference

| Timeout | Time | Behavior |
|----------|------|----------|
| 5 seconds | NFR6 requirement | Enter advertising mode |
| 10 seconds | Extended timeout | Enter advertising mode |
| No timeout | Indefinite | Waits forever (not recommended) |

### References

- [Source: architecture/core-architectural-decisions.md#Communication Patterns] - Event pipeline architecture
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#BLE HID Gotchas] - Always check isConnected() before operations
- [Source: epics.md#Story 10.5] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
