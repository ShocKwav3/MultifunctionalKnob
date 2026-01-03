# Story 10.4: Configure Wheel Button as Wake Source

Status: ready-for-dev

## Story

As a **user wanting to wake a sleeping device**,
I want **to press the wheel button to wake the device from deep sleep**,
so that **I can resume using it without unplugging/replugging**.

## Acceptance Criteria

1. **Given** device is in deep sleep
   **When** I press the wheel button
   **Then** device wakes up
   **And** device reboots and initializes normally

2. **Given** device wakes
   **When** booting
   **Then** it initializes normally
   **And** all systems restore to previous state

3. **Given** device is in deep sleep
   **When** other inputs occur (buttons, encoder rotation)
   **Then** device does NOT wake
   **And** only wheel button is wake source

4. **Given** wake source is configured
   **When** I build the project
   **Then** build succeeds with no errors or warnings

5. **Given** device wakes from sleep
   **When** wake cause is checked
   **Then** correct wake source is logged
   **And** wake cause is identifiable in logs

## Tasks

- [ ] **Task 1: Identify Wheel Button GPIO** (AC: 1, 3)
  - [ ] Review `include/Config/button_config.h` for encoder button pin
  - [ ] Document GPIO number (likely GPIO 0, 1, or 2)
  - [ ] Document button logic level (active low or high)
  - [ ] Verify pin is not used for other critical functions

- [ ] **Task 2: Configure Wake Source** (AC: 1, 3)
  - [ ] Update `src/System/PowerManager.cpp`:
    - [ ] In `enterDeepSleep()` or setup function:
      - [ ] Call `esp_sleep_enable_ext0_wakeup(GPIO_NUM, level)`
      - [ ] GPIO_NUM: wheel button pin from Task 1
      - [ ] level: `0` for active low, `1` for active high
    - [ ] Log wake source configuration with `LOG_INFO`

- [ ] **Task 3: Disable Other Wake Sources** (AC: 3)
  - [ ] In `enterDeepSleep()`:
    - [ ] Disable RTC timer wake: `esp_sleep_enable_timer_wakeup(false)`
    - [ ] Disable UART wake: `esp_deep_sleep_enable_uart_wakeup(0)`
    - [ ] Disable WiFi wake: `esp_deep_sleep_enable_wifi_wakeup(0)`
    - [ ] Disable BLE wake: `esp_deep_sleep_enable_bt_wakeup(0)`
    - [ ] Log disabled sources with `LOG_DEBUG`

- [ ] **Task 4: Add Wake Cause Detection** (AC: 2, 5)
  - [ ] Update `src/main.cpp`:
    - [ ] In `setup()`:
      - [ ] Call `esp_sleep_get_wakeup_cause()` after wake
      - [ ] Log wake cause with `LOG_INFO`
      - [ ] Handle different wake causes appropriately

- [ ] **Task 5: Test Wake Functionality** (AC: 1, 2, 3)
  - [ ] Manual test: Enter deep sleep
  - [ ] Manual test: Press wheel button, verify device wakes
  - [ ] Manual test: Press other buttons, verify device stays asleep
  - [ ] Manual test: Rotate encoder, verify device stays asleep

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Verify wake source configuration is correct

## Dev Notes

### Architecture Compliance

- **Hardware Config**: Check `button_config.h` for correct GPIO pin and logic level
- **ESP32 API**: Use standard ESP-IDF/Arduino deep sleep functions
- **State Management**: `PowerManager` manages wake source configuration
- **Non-Blocking**: Wake source configuration should be immediate, no blocking waits

### Wake Source Configuration

```cpp
// ESP32-C3 external wake source configuration
esp_sleep_enable_ext0_wakeup(gpio_num_t gpio_num, int level);

// Parameters:
// - gpio_num: GPIO pin number (wheel button)
// - level: 0 = wake on LOW, 1 = wake on HIGH
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
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        LOG_INFO("PowerManager", "Woke up by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        LOG_INFO("PowerManager", "Woke up by ULP");
        break;
    default:
        LOG_INFO("PowerManager", "Woke up by unknown cause: %d", wakeCause);
        break;
}
```

### Button Configuration Reference

```cpp
// From include/Config/button_config.h
// Encoder button configuration (example)
constexpr uint8_t ENCODER_BUTTON_PIN = 0;  // Verify actual pin
constexpr bool ENCODER_BUTTON_ACTIVE_LOW = true;  // Verify actual logic
```

### Wake Source Setup

```cpp
// In PowerManager::enterDeepSleep() or setup()
void configureWakeSources() {
    // Enable wheel button as wake source
    uint8_t wakeLevel = ENCODER_BUTTON_ACTIVE_LOW ? 0 : 1;
    esp_sleep_enable_ext0_wakeup(ENCODER_BUTTON_PIN, wakeLevel);

    LOG_INFO("PowerManager", "Wake source: GPIO %d, level %d",
              ENCODER_BUTTON_PIN, wakeLevel);

    // Disable other wake sources
    esp_sleep_enable_timer_wakeup(false);
    esp_deep_sleep_enable_uart_wakeup(0);
    esp_deep_sleep_enable_wifi_wakeup(0);
    esp_deep_sleep_enable_bt_wakeup(0);

    LOG_DEBUG("PowerManager", "Other wake sources disabled");
}
```

### Existing Code Locations

```
include/Config/button_config.h  - GPIO pin and logic level
src/System/PowerManager.h/cpp  - Wake source configuration
src/main.cpp                    - Wake cause detection
```

### Key Files to Modify

| File | Change |
|------|--------|
| `include/Config/button_config.h` | Reference for GPIO pin (read-only) |
| `src/System/PowerManager.cpp` | Configure wake source in `enterDeepSleep()` |
| `src/main.cpp` | Add wake cause detection in `setup()` |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Wake Test**:
   - Enter deep sleep
   - Press wheel button
   - Verify device wakes and reboots
3. **No Wake Test**:
   - Enter deep sleep
   - Press other buttons
   - Verify device stays asleep
   - Rotate encoder
   - Verify device stays asleep
4. **Wake Cause Test**:
   - Check logs after wake
   - Verify correct wake cause is logged
5. **State Restoration Test**:
   - Enter deep sleep with specific state
   - Wake device
   - Verify all systems restore correctly

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Wrong GPIO pin
esp_sleep_enable_ext0_wakeup(21, 0);  // Wrong pin number

// ❌ WRONG - Wrong logic level
esp_sleep_enable_ext0_wakeup(0, 0);  // Wrong level for active-low button

// ❌ WRONG - No wake cause detection
void setup() {
    // Normal initialization
    // No wake cause detection, can't debug sleep issues
}

// ❌ WRONG - Multiple wake sources enabled
esp_sleep_enable_ext0_wakeup(0, 0);  // Button
esp_sleep_enable_ext0_wakeup(1, 0);  // Another button
// Should only enable wheel button

// ❌ WRONG - Blocking wait
void enterDeepSleep() {
    delay(100);  // Unnecessary blocking
    esp_deep_sleep_start();
}

// ❌ WRONG - Dynamic allocation
char* wakeMsg = new char[50];  // Never do this

// ✅ CORRECT - Correct GPIO, level, wake detection
void configureWakeSources() {
    uint8_t wakeLevel = ENCODER_BUTTON_ACTIVE_LOW ? 0 : 1;
    esp_sleep_enable_ext0_wakeup(ENCODER_BUTTON_PIN, wakeLevel);

    LOG_INFO("PowerManager", "Wake source: GPIO %d, level %d",
              ENCODER_BUTTON_PIN, wakeLevel);

    // Disable other wake sources
    esp_sleep_enable_timer_wakeup(false);
    esp_deep_sleep_enable_uart_wakeup(0);
    esp_deep_sleep_enable_wifi_wakeup(0);
    esp_deep_sleep_enable_bt_wakeup(0);
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
}
```

### Wake Cause Reference

| Wake Cause | Value | Description |
|-----------|-------|-------------|
| ESP_SLEEP_WAKEUP_EXT0 | External GPIO (wheel button) |
| ESP_SLEEP_WAKEUP_EXT1 | External GPIO (second source) |
| ESP_SLEEP_WAKEUP_TIMER | RTC timer |
| ESP_SLEEP_WAKEUP_TOUCHPAD | Touchpad |
| ESP_SLEEP_WAKEUP_ULP | Ultra Low Power coprocessor |
| ESP_SLEEP_WAKEUP_GPIO | Any GPIO |
| ESP_SLEEP_WAKEUP_UART | UART |
| ESP_SLEEP_WAKEUP_WIFI | WiFi |
| ESP_SLEEP_WAKEUP_BT | Bluetooth |

### References

- [Source: architecture/core-architectural-decisions.md#Communication Patterns] - Event pipeline architecture
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#ESP32-C3 Specific Gotchas] - Single-core constraints
- [Source: epics.md#Story 10.4] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
