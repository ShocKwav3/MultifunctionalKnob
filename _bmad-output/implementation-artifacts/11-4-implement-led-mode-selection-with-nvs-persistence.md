# Story 11.4: Implement LED Mode Selection with NVS Persistence

Status: ready-for-dev

## Story

As a **user wanting different LED effects**,
I want **to select an LED mode from a list**,
so that **the strip displays my preferred pattern**.

## Acceptance Criteria

1. **Given** I navigate to LED → Mode
   **When** I see the list of available modes
   **Then** I can scroll through modes using the wheel
   **And** modes are clearly labeled (Static, Rainbow, Breathe)

2. **Given** I select a mode
   **When** I confirm the selection
   **Then** the LED strip applies the mode immediately
   **And** the selection is saved to NVS

3. **Given** the setting is saved
   **When** I cycle power
   **Then** the mode setting is restored from NVS
   **And** the LED strip displays the correct pattern

4. **Given** animated modes (Rainbow, Breathe)
   **When** the mode is active
   **Then** animation runs smoothly without blocking
   **And** other device functions remain responsive

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Define LED Mode Enum** (AC: 1)
  - [ ] Create `include/Enum/LedModeEnum.h`:
    ```cpp
    enum class LedMode : uint8_t {
        STATIC = 0,
        RAINBOW = 1,
        BREATHE = 2
    };
    constexpr uint8_t LED_MODE_COUNT = 3;
    ```
  - [ ] Document each mode's behavior

- [ ] **Task 2: Add LED Mode to ConfigManager** (AC: 2, 3)
  - [ ] Add `KEY_LED_MODE` constant to `include/Config/device_config.h`:
    ```cpp
    constexpr const char* KEY_LED_MODE = "led.mode";
    ```
  - [ ] Add `DEFAULT_LED_MODE` constant:
    ```cpp
    constexpr LedMode DEFAULT_LED_MODE = LedMode::STATIC;
    ```
  - [ ] Add `getLedMode()` method to `ConfigManager.h`
  - [ ] Add `setLedMode(LedMode mode)` method to `ConfigManager.h`
  - [ ] Implement methods in `ConfigManager.cpp`:
    - [ ] Use `readEnumConfig` with validation
    - [ ] Return `Error` from setter

- [ ] **Task 3: Extend LedController for Mode Support** (AC: 2, 4)
  - [ ] Update `src/Hardware/LedController.h`:
    - [ ] Add `LedMode currentMode` member
    - [ ] Add `void setMode(LedMode mode)` method
    - [ ] Add `void update()` method for animations
    - [ ] Add animation state variables:
      - [ ] `uint8_t hue` for Rainbow
      - [ ] `uint8_t breathePhase` for Breathe
      - [ ] `unsigned long lastUpdate` for timing
  - [ ] Implement in `src/Hardware/LedController.cpp`:
    - [ ] `setMode()`: Update mode, reset animation state
    - [ ] `update()`: Run animation based on current mode

- [ ] **Task 4: Implement Animation Logic** (AC: 4)
  - [ ] **Static mode**: Apply solid color (no animation)
  - [ ] **Rainbow mode**: Cycle hue across all LEDs
    ```cpp
    void updateRainbow() {
        fill_rainbow(leds, numLeds, hue++, 7);
        FastLED.show();
    }
    ```
  - [ ] **Breathe mode**: Fade brightness in/out
    ```cpp
    void updateBreathe() {
        uint8_t brightness = cubicwave8(breathePhase++);
        FastLED.setBrightness(brightness);
        FastLED.show();
    }
    ```

- [ ] **Task 5: Create SelectLedModeAction Class** (AC: 1, 2)
  - [ ] Create `src/Menu/Action/SelectLedModeAction.h`:
    - [ ] Inherit from `MenuAction`
    - [ ] Store `LedMode targetMode` member
    - [ ] Store `LedController* ledController` member
    - [ ] Store `ConfigManager* configManager` member
  - [ ] Create `src/Menu/Action/SelectLedModeAction.cpp`:
    - [ ] `execute()`: Set mode on controller, save to NVS
    - [ ] `getConfirmationMessage()`: Return mode name

- [ ] **Task 6: Update MenuTree with Mode Items** (AC: 1)
  - [ ] Create `ledModeSubmenu` array with items:
    - [ ] "Static" with `SelectLedModeAction(LedMode::STATIC)`
    - [ ] "Rainbow" with `SelectLedModeAction(LedMode::RAINBOW)`
    - [ ] "Breathe" with `SelectLedModeAction(LedMode::BREATHE)`
  - [ ] Wire "Mode" menu item to submenu
  - [ ] Initialize actions in `initLedActions()`

- [ ] **Task 7: Add update() Call to Main Loop** (AC: 4)
  - [ ] In `src/main.cpp` loop():
    ```cpp
    ledController.update();  // Non-blocking animation update
    ```

- [ ] **Task 8: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Navigate to LED → Mode
  - [ ] Manual test: Select each mode, verify effect
  - [ ] Manual test: Verify animations are smooth
  - [ ] Manual test: Reboot, verify mode persists

## Dev Notes

### Architecture Compliance

- **Non-Blocking Animations**: `update()` must return immediately, use time-based animation
- **Main Loop Integration**: Call `ledController.update()` every loop iteration
- **Enum Storage**: Use `readEnumConfig` for safe retrieval with validation
- **Action Pattern**: Dedicated `MenuAction` subclass for each mode

### Animation Timing Pattern

```cpp
// Non-blocking animation timing
void LedController::update() {
    unsigned long now = millis();

    // Rate limit updates (e.g., 30 FPS = 33ms)
    if (now - lastUpdate < 33) return;
    lastUpdate = now;

    switch (currentMode) {
        case LedMode::STATIC:
            // No animation needed
            break;
        case LedMode::RAINBOW:
            updateRainbow();
            break;
        case LedMode::BREATHE:
            updateBreathe();
            break;
    }
}
```

### Rainbow Animation

```cpp
void LedController::updateRainbow() {
    // fill_rainbow is a FastLED built-in
    fill_rainbow(leds, numLeds, hue, 7);  // 7 = delta hue between LEDs
    hue++;  // Increment for next frame
    FastLED.show();
}
```

### Breathe Animation

```cpp
void LedController::updateBreathe() {
    // cubicwave8 gives smooth ease-in-out
    uint8_t brightness = cubicwave8(breathePhase);
    breathePhase += 2;  // Speed of breathing

    // Apply to current color
    FastLED.setBrightness(scale8(baseBrightness, brightness));
    FastLED.show();
}
```

### Mode Enum Definition

```cpp
// include/Enum/LedModeEnum.h
#pragma once
#include <stdint.h>

enum class LedMode : uint8_t {
    STATIC = 0,   // Solid color, no animation
    RAINBOW = 1,  // Cycling rainbow colors
    BREATHE = 2   // Fade in/out breathing effect
};

constexpr uint8_t LED_MODE_COUNT = 3;

// Helper for display names
inline const char* getLedModeName(LedMode mode) {
    switch (mode) {
        case LedMode::STATIC:  return "Static";
        case LedMode::RAINBOW: return "Rainbow";
        case LedMode::BREATHE: return "Breathe";
        default: return "Unknown";
    }
}
```

### Action Class Pattern

```cpp
class SelectLedModeAction : public MenuAction {
    LedMode targetMode;
    LedController* ledController;
    ConfigManager* configManager;
public:
    SelectLedModeAction(LedMode mode, LedController* led, ConfigManager* config)
        : targetMode(mode), ledController(led), configManager(config) {}

    void execute() override {
        LOG_INFO("LedMode", "Setting mode to: %s", getLedModeName(targetMode));
        ledController->setMode(targetMode);

        Error result = configManager->setLedMode(targetMode);
        if (result != Error::OK) {
            LOG_ERROR("LedMode", "Failed to save mode: %d", result);
        }
    }

    const char* getConfirmationMessage() const override {
        return getLedModeName(targetMode);
    }
};
```

### LedController Extensions

```cpp
// src/Hardware/LedController.h additions
class LedController {
private:
    // ... existing members ...
    LedMode currentMode = LedMode::STATIC;
    uint8_t hue = 0;              // For rainbow
    uint8_t breathePhase = 0;     // For breathe
    unsigned long lastUpdate = 0;

public:
    void setMode(LedMode mode);
    void update();  // Call from main loop

private:
    void updateRainbow();
    void updateBreathe();
};
```

### Existing Code Locations

```
include/Enum/LedModeEnum.h          - New enum definition
include/Config/device_config.h       - Add KEY_LED_MODE constant
src/Hardware/LedController.h/cpp    - Add mode support and animations
src/Config/ConfigManager.h/cpp      - Add getter/setter methods
src/Menu/Action/SelectLedModeAction.h/cpp - New action class
src/Menu/Model/MenuTree.h           - Update menu items
src/main.cpp                        - Add update() call to loop
```

### Key Files to Create/Modify

| File | Change |
|------|--------|
| `include/Enum/LedModeEnum.h` | New enum definition with helper |
| `include/Config/device_config.h` | Add `KEY_LED_MODE`, `DEFAULT_LED_MODE` |
| `src/Hardware/LedController.h` | Add mode, animation state, update() |
| `src/Hardware/LedController.cpp` | Implement animations |
| `src/Config/ConfigManager.h` | Add getter/setter declarations |
| `src/Config/ConfigManager.cpp` | Implement NVS read/write |
| `src/Menu/Action/SelectLedModeAction.h` | New action class header |
| `src/Menu/Action/SelectLedModeAction.cpp` | New action class implementation |
| `src/Menu/Model/MenuTree.h` | Add mode submenu and items |
| `src/main.cpp` | Add `ledController.update()` to loop |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Mode List Test**:
   - Navigate to LED → Mode
   - Verify 3 modes are listed
   - Verify currently selected mode is highlighted
3. **Static Mode Test**:
   - Select Static
   - Verify LED shows solid color (from Story 11.5)
   - Verify no animation
4. **Rainbow Mode Test**:
   - Select Rainbow
   - Verify colors cycle smoothly
   - Verify device remains responsive
5. **Breathe Mode Test**:
   - Select Breathe
   - Verify brightness fades in/out smoothly
   - Verify device remains responsive
6. **Persistence Test**:
   - Set mode to Rainbow
   - Reboot device
   - Verify Rainbow mode resumes
7. **Mode + Brightness Interaction**:
   - Set Breathe mode
   - Adjust brightness
   - Verify Breathe respects brightness setting

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Blocking animation
void updateRainbow() {
    for (int i = 0; i < 256; i++) {
        fill_rainbow(leds, numLeds, i, 7);
        FastLED.show();
        delay(20);  // BLOCKS EVERYTHING!
    }
}

// ❌ WRONG - No rate limiting
void update() {
    updateRainbow();  // Runs as fast as possible, wastes CPU
}

// ❌ WRONG - Heavy computation in update
void update() {
    // Complex calculations every frame
    for (int i = 0; i < 1000; i++) {
        // ...
    }
}

// ✅ CORRECT - Non-blocking, rate-limited
void LedController::update() {
    unsigned long now = millis();
    if (now - lastUpdate < 33) return;  // 30 FPS limit
    lastUpdate = now;

    switch (currentMode) {
        case LedMode::RAINBOW:
            fill_rainbow(leds, numLeds, hue++, 7);
            FastLED.show();
            break;
        // ...
    }
}
```

### Animation Smoothness Tips

- Use `millis()` for timing, never `delay()`
- Target 30 FPS (33ms per frame) for smooth animation
- Use FastLED built-ins: `fill_rainbow()`, `cubicwave8()`, `scale8()`
- Reset animation state when switching modes

### References

- [Source: architecture/core-architectural-decisions.md#Command Pattern for Actions] - Action class structure
- [Source: architecture/implementation-patterns-consistency-rules.md#Format Patterns] - Error return pattern
- [Source: project-context.md#ESP32-C3 Specific Gotchas] - Non-blocking requirements
- [Source: project-context.md#Configuration Persistence Gotchas] - NVS validation
- [Source: epics.md#Story 11.4] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

{{agent_model_name_version}}

### Completion Notes

### Files Modified
