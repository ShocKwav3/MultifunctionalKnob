# Story 8.3: Implement Bluetooth Disconnect Action

Status: ready-for-dev

## Story

As a **user wanting to disconnect from the current host**,
I want **to disconnect Bluetooth from the menu**,
so that **I can release the connection without turning off the device**.

## Acceptance Criteria

1. **Given** I navigate to Bluetooth → Disconnect
   **When** I select "Disconnect"
   **Then** the system disconnects the current BLE connection
   **And** the menu exits or returns to parent

2. **Given** disconnection occurs successfully
   **When** the disconnect action completes
   **Then** the display shows "Disconnected" status feedback
   **And** the menu returns to parent or exits

3. **Given** I am not connected to any BLE device
   **When** I select "Disconnect"
   **Then** the action shows "Not connected" feedback
   **And** the menu returns to parent or exits

4. **Given** the disconnect action is triggered
   **When** the BLE connection is active
   **Then** the disconnect is initiated immediately
   **And** no blocking wait occurs

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Audit BleKeyboard Wrapper** (AC: 1, 4)
  - [ ] Review `src/Helper/BleKeyboard.h` for existing BLE methods
  - [ ] Identify `isConnected()` method
  - [ ] Identify `disconnect()` method
  - [ ] Document BLE disconnect behavior (synchronous vs asynchronous)
  - [ ] Verify disconnect method returns void or error status

- [ ] **Task 2: Create DisconnectAction Class** (AC: 1, 2, 3)
  - [ ] Create `src/Menu/Action/DisconnectAction.h`:
    - [ ] Inherit from `MenuAction`
    - [ ] Store `BleKeyboard* bleKeyboard` member
    - [ ] Store `DisplayInterface* display` member (or use DisplayRequestQueue)
    - [ ] Constructor: `DisconnectAction(BleKeyboard* ble, DisplayInterface* disp)`
    - [ ] Override `execute()` method
    - [ ] Override `getConfirmationMessage()` method
  - [ ] Create `src/Menu/Action/DisconnectAction.cpp`:
    - [ ] Implement `execute()`:
      - [ ] Check connection state with `bleKeyboard->isConnected()`
      - [ ] If connected: Call `bleKeyboard->disconnect()`, show "Disconnected"
      - [ ] If not connected: Show "Not connected"
      - [ ] Log action with `LOG_INFO`
    - [ ] Implement `getConfirmationMessage()`:
      - [ ] Return "Disconnected" or "Not connected" based on state

- [ ] **Task 3: Handle Display Feedback** (AC: 2, 3)
  - [ ] Use `DisplayRequestQueue` for thread-safe display updates
  - [ ] Show "Disconnected" when disconnect succeeds
  - [ ] Show "Not connected" when already disconnected
  - [ ] Ensure feedback is visible to user

- [ ] **Task 4: Wire up in MenuTree** (AC: 1)
  - [ ] Initialize `DisconnectAction` in `initBluetoothActions()` function
  - [ ] Assign action to "Disconnect" menu item in `bluetoothSubmenu`
  - [ ] Ensure parent pointers are set correctly (from Story 8.1)

- [ ] **Task 5: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Connect to PC, trigger disconnect, verify PC loses connection
  - [ ] Manual test: Disconnect when not connected, verify "Not connected" message

## Dev Notes

### Architecture Compliance

- **Command Pattern**: Encapsulate logic in Action class following established pattern
- **Display Arbitration**: Use `DisplayRequestQueue` to show status messages (prevents race conditions)
- **State Check**: Always check `isConnected()` before attempting disconnect
- **Non-Blocking**: Disconnect should be immediate, no blocking waits

### BLE Disconnect Behavior

```cpp
// BLE disconnect flow:
// CONNECTED → disconnect() → DISCONNECTED
// DISCONNECTED → disconnect() → No-op (show "Not connected")
```

### Action Class Pattern

```cpp
// From architecture/core-architectural-decisions.md#Command Pattern for Actions
class DisconnectAction : public MenuAction {
    BleKeyboard* bleKeyboard;
    DisplayInterface* display;  // Or use DisplayRequestQueue
public:
    DisconnectAction(BleKeyboard* ble, DisplayInterface* disp)
        : bleKeyboard(ble), display(disp) {}

    void execute() override {
        if (bleKeyboard->isConnected()) {
            LOG_INFO("DisconnectAction", "Disconnecting BLE");
            bleKeyboard->disconnect();
            display->showStatus("Status", "Disconnected");
        } else {
            LOG_INFO("DisconnectAction", "Already disconnected");
            display->showStatus("Status", "Not connected");
        }
    }

    const char* getConfirmationMessage() const override {
        return bleKeyboard->isConnected() ? "Disconnected" : "Not connected";
    }
};
```

### Display Arbitration Pattern

```cpp
// From architecture/core-architectural-decisions.md#Display Arbitration Pattern
// Use DisplayRequestQueue for thread-safe display updates
struct DisplayRequest {
    enum Type { SHOW_STATUS, CLEAR } type;
    union {
        struct { const char* key; const char* value; } status;
    } data;
};

// In DisconnectAction::execute():
DisplayRequest req;
req.type = DisplayRequest::SHOW_STATUS;
req.data.status.key = "Status";
req.data.status.value = bleKeyboard->isConnected() ? "Disconnected" : "Not connected";
xQueueSend(displayQueue, &req, portMAX_DELAY);
```

### Existing Code Locations

```
src/Helper/BleKeyboard.h/cpp           - BLE wrapper with disconnect method
src/Menu/Action/DisconnectAction.h/cpp   - New action class
src/Menu/Model/MenuTree.h                - Update menu items
src/Display/Task/DisplayTask.cpp           - Handle display requests
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Menu/Action/DisconnectAction.h` | New action class header |
| `src/Menu/Action/DisconnectAction.cpp` | New action class implementation |
| `src/Menu/Model/MenuTree.h` | Update menu items and action initialization |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Disconnect Test**:
   - Connect to a device (phone/PC)
   - Navigate to Bluetooth → Disconnect
   - Verify "Disconnected" message displays
   - Verify device loses connection
3. **Already Disconnected Test**:
   - Ensure no BLE connection is active
   - Navigate to Bluetooth → Disconnect
   - Verify "Not connected" message displays
4. **Menu Exit Test**:
   - Verify menu returns to parent after action
5. **Non-Blocking Test**:
   - Verify disconnect happens immediately
   - Verify no system freeze or delay

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No state check
void DisconnectAction::execute() {
    bleKeyboard->disconnect();  // Might fail if not connected
}

// ❌ WRONG - Blocking wait
void DisconnectAction::execute() {
    bleKeyboard->disconnect();
    while (bleKeyboard->isConnected()) {
        delay(100);  // Blocks entire system!
    }
}

// ❌ WRONG - Direct display access (race condition)
void DisconnectAction::execute() {
    display->clear();
    display->println("Disconnected");  // Bypasses DisplayTask arbitration
}

// ❌ WRONG - No feedback
void DisconnectAction::execute() {
    bleKeyboard->disconnect();
    // User has no idea what happened
}

// ❌ WRONG - Wrong confirmation message
const char* getConfirmationMessage() const override {
    return "Disconnected";  // Always says disconnected even if not connected
}

// ✅ CORRECT - With state check and display arbitration
void DisconnectAction::execute() {
    DisplayRequest req;
    req.type = DisplayRequest::SHOW_STATUS;
    req.data.status.key = "Status";

    if (bleKeyboard->isConnected()) {
        LOG_INFO("DisconnectAction", "Disconnecting BLE");
        bleKeyboard->disconnect();
        req.data.status.value = "Disconnected";
    } else {
        LOG_INFO("DisconnectAction", "Already disconnected");
        req.data.status.value = "Not connected";
    }

    xQueueSend(displayQueue, &req, portMAX_DELAY);
}

const char* getConfirmationMessage() const override {
    return bleKeyboard->isConnected() ? "Disconnected" : "Not connected";
}
```

### BLE Disconnect Flow

```
User Action: Select "Disconnect"
    ↓
DisconnectAction::execute()
    ↓
Check: isConnected()?
    ├─ YES → disconnect()
    │         ↓
    │       Display: "Disconnected"
    │         ↓
    │       Menu exits
    │
    └─ NO → Display: "Not connected"
              ↓
            Menu exits
```

### References

- [Source: architecture/core-architectural-decisions.md#Command Pattern for Actions] - Action class structure
- [Source: architecture/core-architectural-decisions.md#Display Arbitration Pattern] - Display request queue usage
- [Source: architecture/implementation-patterns-consistency-rules.md#Format Patterns] - Error return pattern
- [Source: project-context.md#BLE HID Gotchas] - Always check isConnected() before operations
- [Source: epics.md#Story 8.3] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
