# Story 10.3: Enter Deep Sleep After 5 Minutes Inactivity

Status: ready-for-dev

## Story

As a **user who has left the device idle**,
I want **the device to enter deep sleep after 5 minutes**,
so that **power is conserved when not in use**.

## Acceptance Criteria

1. **Given** the inactivity timer reaches 5 minutes
   **When** the timeout occurs
   **Then** the ESP32 enters deep sleep mode
   **And** all peripherals are properly shut down

2. **Given** entering deep sleep
   **When** transition happens
   **Then** display turns off
   **And** BLE disconnects (if connected)

3. **Given** device is in deep sleep
   **When** no wake source is triggered
   **Then** device remains in sleep indefinitely
   **And** power consumption is minimal

4. **Given** device enters deep sleep
   **When** wake source is triggered
   **Then** device reboots and initializes normally
   **And** all systems restore to previous state

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Implement Deep Sleep Logic** (AC: 1, 3)
  - [ ] Update `src/System/PowerManager.cpp`:
    - [ ] Add `enterDeepSleep()` method:
      - [ ] Log with `LOG_INFO`
      - [ ] Call `esp_deep_sleep_start()`
    - [ ] Update `checkActivity()`:
      - [ ] When state transitions to `SLEEP`, call `enterDeepSleep()`

- [ ] **Task 2: Configure Deep Sleep Parameters** (AC: 1, 3)
  - [ ] Define wake source configuration:
    - [ ] Wheel button GPIO as wake source (from Story 10.4)
    - [ ] Wake mode: `ESP_WAKEUP_EXT0` (external wake)
    - [ ] Disable other wake sources (RTC timer, etc.)
  - [ ] Configure sleep mode:
    - [ ] Use `esp_deep_sleep_enable_uart_wakeup(0)` (disable UART wake)
    - [ ] Use `esp_deep_sleep_enable_wifi_wakeup(0)` (disable WiFi wake)

- [ ] **Task 3: Cleanup Before Sleep** (AC: 2)
  - [ ] In `PowerManager::enterDeepSleep()`:
    - [ ] Turn off display: `display->setPower(false)`
    - [ ] Disconnect BLE: `bleKeyboard->end()` or equivalent
    - [ ] Log cleanup actions with `LOG_INFO`
    - [ ] Ensure all peripherals are in low-power state

- [ ] **Task 4: Handle Wake from Sleep** (AC: 4)
  - [ ] Update `src/main.cpp`:
    - [ ] Add wake cause detection in `setup()`:
      - [ ] Use `esp_sleep_get_wakeup_cause()`
      - [ ] Log wake cause with `LOG_INFO`
    - [ ] Ensure normal initialization after wake
    - [ ] Reinitialize all systems (display, BLE, etc.)

- [ ] **Task 5: Add Sleep State Tracking** (AC: 1, 3)
  - [ ] Update `src/System/PowerManager.h`:
    - [ ] Add `bool inDeepSleep` member
    - [ ] Set to `true` before entering sleep
    - [ ] Set to `false` after wake

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Wait 5 minutes, verify device sleeps
  - [ ] Manual test: Press wheel button, verify device wakes
  - [ ] Manual test: Verify power consumption drops (if ammeter available)

## Dev Notes

### Architecture Compliance

- **ESP32 API**: Use standard ESP-IDF/Arduino deep sleep functions
- **Hardware Config**: Check `button_config.h` for correct GPIO pin
- **State Management**: `PowerManager` manages sleep entry and wake handling
- **Non-Blocking**: Deep sleep entry should be immediate, no blocking waits

### Deep Sleep Configuration

```cpp
// ESP32-C3 deep sleep configuration
esp_sleep_enable_ext0_wakeup(GPIO_NUM, level);  // Wheel button wake
esp_sleep_enable_ext1_wakeup(GPIO_NUM, level);  // Optional: second wake source
esp_deep_sleep_enable_uart_wakeup(0);  // Disable UART wake
esp_deep_sleep_enable_wifi_wakeup(0);  // Disable WiFi wake
esp_deep_sleep_enable_bt_wakeup(0);    // Disable BLE wake
```

### Wake Cause Detection

```cpp
// Detect wake cause after deep sleep
esp_sleep_wakeup_cause_t wakeCause = esp_sleep_get_wakeup_cause();

switch (wakeCause) {
    case ESP_SLEEP_WAKEUP_EXT0:
        LOG_INFO("PowerManager", "Woke up by EXT0 (wheel button)");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        LOG_INFO("PowerManager", "Woke up by timer");
        break;
    default:
        LOG_INFO("PowerManager", "Woke up by unknown cause: %d", wakeCause);
        break;
}
```

### Sleep Entry Pattern

```cpp
// src/System/PowerManager.cpp
void PowerManager::enterDeepSleep() {
    LOG_INFO("PowerManager", "Entering deep sleep");

    // Turn off display
    display->setPower(false);

    // Disconnect BLE
    bleKeyboard->end();

    // Flush serial buffers
    Serial.flush();

    // Set sleep state
    inDeepSleep = true;

    // Enter deep sleep
    LOG_INFO("PowerManager", "Calling esp_deep_sleep_start()");
    esp_deep_sleep_start();

    // Code never reaches here (device sleeps)
    // After wake, device reboots and starts from setup()
}
```

### Main Loop Integration

```cpp
// src/main.cpp
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
            // Enter deep sleep
            powerManager.enterDeepSleep();
            // Code never reaches here
            break;
    }

    // ... existing loop code ...
}
```

### Existing Code Locations

```
src/System/PowerManager.h/cpp           - Power management and sleep logic
src/Display/Interface/DisplayInterface.h  - Display power control
src/Helper/BleKeyboard.cpp                   - BLE disconnect
src/main.cpp                                    - Main loop and wake detection
include/Config/button_config.h                 - GPIO pin configuration
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/System/PowerManager.h` | Add `enterDeepSleep()` and `inDeepSleep` member |
| `src/System/PowerManager.cpp` | Implement deep sleep logic |
| `src/main.cpp` | Add wake cause detection in setup() |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Sleep Entry Test**:
   - Wait 5 minutes
   - Verify device enters deep sleep
   - Verify display turns off
   - Verify BLE disconnects
3. **Wake Test**:
   - Press wheel button while sleeping
   - Verify device wakes and reboots
   - Verify all systems reinitialize
4. **Power Consumption Test**:
   - Measure current in active state
   - Measure current in deep sleep
   - Verify significant power reduction
5. **Wake Cause Test**:
   - Check logs for wake cause
   - Verify correct wake source is logged

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No cleanup before sleep
void enterDeepSleep() {
    esp_deep_sleep_start();
    // Display and BLE still on, wasting power
}

// ❌ WRONG - Blocking wait
void enterDeepSleep() {
    delay(1000);  // Unnecessary blocking
    esp_deep_sleep_start();
}

// ❌ WRONG - Wrong wake source
esp_sleep_enable_ext0_wakeup(21, 0);  // Wrong GPIO pin

// ❌ WRONG - No wake cause detection
void setup() {
    // Normal initialization
    // No wake cause detection, can't debug sleep issues
}

// ❌ WRONG - Dynamic allocation
char* wakeMsg = new char[50];  // Never do this

// ✅ CORRECT - Cleanup, non-blocking, proper wake config
void PowerManager::enterDeepSleep() {
    LOG_INFO("PowerManager", "Entering deep sleep");

    // Cleanup before sleep
    display->setPower(false);
    bleKeyboard->end();
    Serial.flush();

    // Set sleep state
    inDeepSleep = true;

    // Enter deep sleep
    LOG_INFO("PowerManager", "Calling esp_deep_sleep_start()");
    esp_deep_sleep_start();
}

void setup() {
    // Detect wake cause
    esp_sleep_wakeup_cause_t wakeCause = esp_sleep_get_wakeup_cause();

    switch (wakeCause) {
        case ESP_SLEEP_WAKEUP_EXT0:
            LOG_INFO("PowerManager", "Woke up by EXT0 (wheel button)");
            break;
        // ... other cases
    }

    // Normal initialization
    // ...
}
```

### Deep Sleep Flow

```
5 minutes idle reached
    ↓
PowerManager::checkActivity() returns SLEEP
    ↓
PowerManager::enterDeepSleep()
    ↓
Cleanup:
  - display->setPower(false)
  - bleKeyboard->end()
  - Serial.flush()
    ↓
esp_deep_sleep_start()
    ↓
Device sleeps (minimal power)
    ↓
[User presses wheel button]
    ↓
Device wakes and reboots
    ↓
setup() runs
    ↓
Detect wake cause
    ↓
Normal initialization
```

### References

- [Source: architecture/core-architectural-decisions.md#Communication Patterns] - Event pipeline architecture
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#ESP32-C3 Specific Gotchas] - Single-core constraints
- [Source: epics.md#Story 10.3] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
