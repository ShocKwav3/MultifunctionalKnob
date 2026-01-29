# Story 12.2: Implement LED Power Toggle with NVS Persistence

Status: ready-for-dev

## Story

As a **user controlling the LED strip**,
I want **to toggle LED power On/Off from the menu**,
so that **I can turn the strip on or off and have it remember my choice**.

## Acceptance Criteria

1. **Given** I navigate to LED → Power
   **When** I toggle it
   **Then** LED strip turns on/off
   **And** change is immediate

2. **Given** I change the setting
   **When** confirmed
   **Then** saved to NVS
   **And** setting persists across power cycles

3. **Given** power cycle occurs
   **When** device reboots
   **Then** LED power state is restored from NVS
   **And** strip returns to previous state

4. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

5. **Given** LED power is toggled
   **When** I check the menu
   **Then** current state is reflected in menu display
   **And** "On" or "Off" is clearly shown

## Tasks

- [ ] **Task 1: Create LedController Class** (AC: 1)
  - [ ] Create `src/Hardware/LedController.h`:
    - [ ] Class `LedController` with private members:
      - [ ] `bool powerOn` - tracks LED power state
      - [ ] `uint8_t brightness` - tracks brightness (0-100)
      - [ ] `CRGB leds[]` - FastLED LED array
      - [ ] `uint8_t numLeds` - number of LEDs in strip
    - [ ] Public methods:
      - [ ] `void init()` - initialize FastLED and GPIO
      - [ ] `void setPower(bool on)` - turn strip on/off
      - [ ] `void setBrightness(uint8_t percent)` - set brightness (0-100)
      - [ ] `void update()` - update LED display
      - [ ] `bool isPowerOn() const` - get current power state
  - [ ] Create `src/Hardware/LedController.cpp`:
    - [ ] Implement `init()`:
      - [ ] Call `FastLED.addLeds<CRGB>(leds, numLeds)`
      - [ ] Set initial brightness
      - [ ] Call `FastLED.begin()`
      - [ ] Log initialization with `LOG_INFO`
    - [ ] Implement `setPower(bool on)`:
      - [ ] If `on` is true: Call `FastLED.show()` with current brightness
      - [ ] If `on` is false: Call `FastLED.clear()` and `FastLED.show()`
      - [ ] Update `powerOn` state
      - [ ] Log with `LOG_INFO`
    - [ ] Implement `setBrightness(uint8_t percent)`:
      - [ ] Convert percent (0-100) to FastLED brightness (0-255)
      - [ ] Update `brightness` member
      - [ ] If `powerOn` is true: Call `FastLED.show()` with new brightness
      - [ ] Log with `LOG_DEBUG`
    - [ ] Implement `update()`:
      - [ ] Call `FastLED.show()` to update display
      - [ ] Use `FastLED.delay()` for non-blocking updates if needed
    - [ ] Implement `isPowerOn() const`:
      - [ ] Return `powerOn` state

- [ ] **Task 2: Add LED Power State to ConfigManager** (AC: 2, 3)
  - [ ] Add `KEY_LED_POWER` constant to `include/Config/device_config.h`:
    ```cpp
    constexpr const char* KEY_LED_POWER = "led.power";
    ```
  - [ ] Add `getLedPower()` method to `ConfigManager.h`:
    ```cpp
    bool getLedPower() const;
    ```
  - [ ] Add `setLedPower()` method to `ConfigManager.h`:
    ```cpp
    Error setLedPower(bool on);
    ```
  - [ ] Implement methods in `ConfigManager.cpp`:
    - [ ] `getLedPower()`: Use `preferences.getBool()` with default `true`
    - [ ] `setLedPower()`: Use `preferences.putBool()` to save state
    - [ ] Return `Error::OK` on success, `Error::NVS_WRITE_FAIL` on failure

- [ ] **Task 3: Create LedPowerAction Class** (AC: 1, 5)
  - [ ] Create `src/Menu/Action/LedPowerAction.h`:
    - [ ] Inherit from `MenuAction`
    - [ ] Store `LedController* ledController` member
    - [ ] Store `ConfigManager* configManager` member
    - [ ] Constructor: `LedPowerAction(LedController* led, ConfigManager* config)`
    - [ ] Override `execute()` method
    - [ ] Override `getConfirmationMessage()` method
  - [ ] Create `src/Menu/Action/LedPowerAction.cpp`:
    - [ ] Implement `execute()`:
      - [ ] Get current power state: `configManager->getLedPower()`
      - [ ] Toggle power: `ledController->setPower(!currentPower)`
      - [ ] Save new state: `configManager->setLedPower(!currentPower)`
      - [ ] Log action with `LOG_INFO`
    - [ ] Implement `getConfirmationMessage()`:
      - [ ] Return "LED On" or "LED Off" based on new state

- [ ] **Task 4: Wire up in MenuTree** (AC: 1, 5)
  - [ ] Update `initLedActions()` function (or add to existing init function)
  - [ ] Initialize `LedPowerAction` instance
  - [ ] Assign action to "Power" menu item in `ledSubmenu`
  - [ ] Ensure parent pointers are set correctly (from Story 12.1)

- [ ] **Task 5: Integrate LedController in Main** (AC: 1)
  - [ ] Update `src/main.cpp`:
    - [ ] Instantiate `LedController` in setup()
    - [ ] Call `ledController->init()`
    - [ ] Pass `LedController` to `LedPowerAction` constructor
    - [ ] Ensure initialization order: Wire → FastLED → LedController

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Toggle LED power, verify strip turns on/off
  - [ ] Manual test: Reboot device, verify power state persists

## Dev Notes

### Architecture Compliance

- **Controller Pattern**: Encapsulate FastLED logic in `LedController` for clean separation
- **Action Pattern**: Create dedicated `MenuAction` subclass following Command pattern
- **NVS Persistence**: LED power state should persist across power cycles
- **Non-Blocking**: LED updates should be fast and non-blocking

### FastLED Configuration

```cpp
// FastLED library configuration
#define LED_PIN     5  // GPIO pin for LED strip (verify actual pin)
#define LED_TYPE    WS2812B  // LED strip type (verify actual type)
#define COLOR_ORDER GRB  // Color order (verify actual order)
#define NUM_LEDS    30  // Number of LEDs (verify actual count)
```

### LedController Class Structure

```cpp
// src/Hardware/LedController.h
#pragma once
#include <FastLED.h>

class LedController {
public:
    LedController();
    void init();
    void setPower(bool on);
    void setBrightness(uint8_t percent);
    void update();
    bool isPowerOn() const;

private:
    bool powerOn;
    uint8_t brightness;
    CRGB leds[NUM_LEDS];
    uint8_t numLeds;
};
```

### Action Class Pattern

```cpp
// From architecture/core-architectural-decisions.md#Command Pattern for Actions
class LedPowerAction : public MenuAction {
    LedController* ledController;
    ConfigManager* configManager;
public:
    LedPowerAction(LedController* led, ConfigManager* config)
        : ledController(led), configManager(config) {}

    void execute() override {
        bool currentPower = configManager->getLedPower();
        bool newPower = !currentPower;

        LOG_INFO("LedPower", "Toggling LED power: %d", newPower);

        // Update LED hardware
        ledController->setPower(newPower);

        // Save to NVS
        Error result = configManager->setLedPower(newPower);
        if (result != Error::OK) {
            LOG_ERROR("LedPower", "Failed to save state: %d", result);
        }
    }

    const char* getConfirmationMessage() const override {
        return configManager->getLedPower() ? "LED Off" : "LED On";
    }
};
```

### ConfigManager Pattern

```cpp
// src/Config/ConfigManager.cpp
bool ConfigManager::getLedPower() const {
    return preferences.getBool(KEY_LED_POWER, true);  // Default: on
}

Error ConfigManager::setLedPower(bool on) {
    if (!preferences.putBool(KEY_LED_POWER, on)) {
        return Error::NVS_WRITE_FAIL;
    }
    return Error::OK;
}
```

### Existing Code Locations

```
src/Hardware/LedController.h/cpp           - New LED controller class
src/Menu/Action/LedPowerAction.h/cpp       - New action class
include/Config/device_config.h                - Add KEY_LED_POWER constant
src/Config/ConfigManager.h/cpp             - Add getter/setter methods
src/Menu/Model/MenuTree.h                - Update menu items
src/main.cpp                                    - Initialize LedController
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Hardware/LedController.h` | New LED controller class header |
| `src/Hardware/LedController.cpp` | New LED controller class implementation |
| `src/Menu/Action/LedPowerAction.h` | New action class header |
| `src/Menu/Action/LedPowerAction.cpp` | New action class implementation |
| `include/Config/device_config.h` | Add `KEY_LED_POWER` constant |
| `src/Config/ConfigManager.h` | Add getter/setter method declarations |
| `src/Config/ConfigManager.cpp` | Implement NVS read/write |
| `src/Menu/Model/MenuTree.h` | Update menu items and action initialization |
| `src/main.cpp` | Initialize LedController |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Power Toggle Test**:
   - Navigate to LED → Power
   - Toggle to Off → Verify strip turns off
   - Toggle to On → Verify strip turns on
3. **Persistence Test**:
   - Set LED power to On
   - Reboot device
   - Verify LED power is On after reboot
4. **Confirmation Test**:
   - Toggle LED power
   - Verify confirmation message displays
   - Verify menu shows correct state
5. **Brightness Test** (for Story 12.3):
   - Set LED power to On
   - Adjust brightness
   - Verify brightness changes

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No NVS persistence
void LedPowerAction::execute() {
    ledController->setPower(!currentPower);
    // State lost on reboot
}

// ❌ WRONG - No confirmation message
const char* getConfirmationMessage() const override {
    return nullptr;  // User has no feedback
}

// ❌ WRONG - Wrong NVS key
preferences.putBool("led_power", on);  // Wrong key format

// ❌ WRONG - No state tracking
void setPower(bool on) {
    FastLED.show();
    // Can't query current state
}

// ❌ WRONG - Blocking wait
void setPower(bool on) {
    FastLED.show();
    delay(100);  // Unnecessary blocking
}

// ❌ WRONG - Dynamic allocation
CRGB* leds = new CRGB[NUM_LEDS];  // Never do this

// ✅ CORRECT - With persistence, feedback, state tracking
void LedPowerAction::execute() {
    bool currentPower = configManager->getLedPower();
    bool newPower = !currentPower;

    LOG_INFO("LedPower", "Toggling LED power: %d", newPower);

    // Update LED hardware
    ledController->setPower(newPower);

    // Save to NVS
    Error result = configManager->setLedPower(newPower);
    if (result != Error::OK) {
        LOG_ERROR("LedPower", "Failed to save state: %d", result);
    }
}

void LedController::setPower(bool on) {
    powerOn = on;

    if (on) {
        FastLED.show(brightness);  // Show with current brightness
        LOG_INFO("LedController", "LED ON");
    } else {
        FastLED.clear();
        FastLED.show();
        LOG_INFO("LedController", "LED OFF");
    }
}

bool ConfigManager::getLedPower() const {
    return preferences.getBool(KEY_LED_POWER, true);
}

Error ConfigManager::setLedPower(bool on) {
    if (!preferences.putBool(KEY_LED_POWER, on)) {
        return Error::NVS_WRITE_FAIL;
    }
    return Error::OK;
}
```

### LED Power Flow

```
User Action: Select "Power"
    ↓
LedPowerAction::execute()
    ↓
Toggle power state
    ↓
ledController->setPower(newPower)
    ↓
FastLED.show() or FastLED.clear()
    ↓
Save to NVS
    ↓
Menu exits with confirmation
```

### References

- [Source: architecture/core-architectural-decisions.md#Command Pattern for Actions] - Action class structure
- [Source: architecture/implementation-patterns-consistency-rules.md#Format Patterns] - Error return pattern
- [Source: project-context.md#Configuration Persistence Gotchas] - NVS write validation
- [Source: epics.md#Story 12.2] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
