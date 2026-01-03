# Story 11.3: Implement Wheel-Controlled Brightness with Live Preview

Status: ready-for-dev

## Story

As a **user adjusting LED brightness**,
I want **to use the wheel to change brightness and see the result in real-time**,
so that **I can find the perfect brightness level visually**.

## Acceptance Criteria

1. **Given** I navigate to LED → Brightness
   **When** I enter the brightness adjustment mode
   **Then** the display shows current brightness percentage (0-100%)
   **And** the wheel is ready to adjust the value

2. **Given** I am in brightness adjustment mode
   **When** I rotate the wheel clockwise
   **Then** brightness increases (capped at 100%)
   **And** the LED strip updates immediately (live preview)
   **And** the display shows updated percentage

3. **Given** I am in brightness adjustment mode
   **When** I rotate the wheel counter-clockwise
   **Then** brightness decreases (minimum 0%)
   **And** the LED strip updates immediately (live preview)
   **And** the display shows updated percentage

4. **Given** I have adjusted brightness to desired level
   **When** I press the wheel button to confirm
   **Then** the brightness is saved to NVS
   **And** the menu exits or returns to parent
   **And** confirmation feedback is shown

5. **Given** I am in brightness adjustment mode
   **When** I long-press the wheel button (cancel)
   **Then** the brightness reverts to previous saved value
   **And** the menu returns to parent without saving

6. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Add Brightness to ConfigManager** (AC: 4)
  - [ ] Add `KEY_LED_BRIGHTNESS` constant to `include/Config/device_config.h`:
    ```cpp
    constexpr const char* KEY_LED_BRIGHTNESS = "led.bright";
    ```
  - [ ] Add `DEFAULT_LED_BRIGHTNESS` constant:
    ```cpp
    constexpr uint8_t DEFAULT_LED_BRIGHTNESS = 128;  // 50%
    ```
  - [ ] Add `getLedBrightness()` method to `ConfigManager.h`
  - [ ] Add `setLedBrightness(uint8_t brightness)` method to `ConfigManager.h`
  - [ ] Implement methods in `ConfigManager.cpp`

- [ ] **Task 2: Extend LedController for Brightness** (AC: 2, 3)
  - [ ] Update `src/Hardware/LedController.h`:
    - [ ] Add `uint8_t brightness` member (0-255 internal, maps to 0-100% UI)
    - [ ] Add `void setBrightness(uint8_t level)` method
    - [ ] Add `uint8_t getBrightness() const` method
  - [ ] Implement in `src/Hardware/LedController.cpp`:
    - [ ] `setBrightness()`: Update FastLED global brightness
    - [ ] Call `FastLED.setBrightness(brightness)`
    - [ ] Call `FastLED.show()` for immediate update
    - [ ] Log with `LOG_DEBUG`

- [ ] **Task 3: Create BrightnessAdjustMode** (AC: 1, 2, 3, 5)
  - [ ] Create `src/EncoderMode/Handler/BrightnessAdjustModeHandler.h`:
    - [ ] Inherit from `EncoderModeHandlerAbstract`
    - [ ] Store `LedController* ledController` member
    - [ ] Store `ConfigManager* configManager` member
    - [ ] Store `uint8_t originalBrightness` for cancel/revert
    - [ ] Store `uint8_t currentBrightness` for live adjustment
  - [ ] Create `src/EncoderMode/Handler/BrightnessAdjustModeHandler.cpp`:
    - [ ] `handleRotate(int delta)`: Adjust brightness by delta * step
    - [ ] `handleShortClick()`: Save to NVS, exit mode, return to menu
    - [ ] `handleLongClick()`: Revert to original, exit mode, return to menu
    - [ ] `onEnter()`: Store original brightness, show current value
    - [ ] `onExit()`: Cleanup

- [ ] **Task 4: Create AdjustBrightnessAction** (AC: 1)
  - [ ] Create `src/Menu/Action/AdjustBrightnessAction.h`:
    - [ ] Inherit from `MenuAction`
    - [ ] Store reference to `EncoderModeManager`
    - [ ] Constructor accepts mode manager and brightness mode
  - [ ] Create `src/Menu/Action/AdjustBrightnessAction.cpp`:
    - [ ] `execute()`: Switch encoder mode to `BrightnessAdjustMode`
    - [ ] Log action with `LOG_INFO`

- [ ] **Task 5: Register Mode and Wire Menu** (AC: 1)
  - [ ] Register `BrightnessAdjustModeHandler` in `main.cpp`
  - [ ] Add mode enum to `WheelModeEnum.h` if needed
  - [ ] Update `MenuTree.h`:
    - [ ] Assign `AdjustBrightnessAction` to "Brightness" menu item

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Navigate to LED → Brightness
  - [ ] Manual test: Rotate wheel, verify live LED update
  - [ ] Manual test: Confirm saves to NVS
  - [ ] Manual test: Cancel reverts brightness
  - [ ] Manual test: Reboot, verify brightness persists

## Dev Notes

### Architecture Compliance

- **Mode Switching Pattern**: Menu action triggers mode change via `EncoderModeManager`
- **Live Preview**: Direct `LedController::setBrightness()` call on each rotation
- **Non-Blocking**: No delays or blocking in rotation handler
- **Cancel/Revert**: Store original value on mode entry, restore on cancel

### Brightness Value Mapping

```cpp
// Internal: 0-255 (FastLED native)
// UI Display: 0-100%
// Conversion: percentage = (brightness * 100) / 255
//             brightness = (percentage * 255) / 100

constexpr uint8_t percentToBrightness(uint8_t percent) {
    return (percent * 255) / 100;
}

constexpr uint8_t brightnessToPercent(uint8_t brightness) {
    return (brightness * 100) / 255;
}
```

### Mode Handler Pattern

```cpp
// src/EncoderMode/Handler/BrightnessAdjustModeHandler.h
#pragma once
#include "EncoderModeHandlerAbstract.h"

class BrightnessAdjustModeHandler : public EncoderModeHandlerAbstract {
private:
    LedController* ledController;
    ConfigManager* configManager;
    uint8_t originalBrightness;
    uint8_t currentBrightness;
    static constexpr uint8_t STEP_SIZE = 5;  // 5% per detent

public:
    BrightnessAdjustModeHandler(
        AppEventDispatcher* dispatcher,
        LedController* led,
        ConfigManager* config
    );

    void handleRotate(int delta) override;
    void handleShortClick() override;
    void handleLongClick() override;
    void onEnter() override;
    void onExit() override;
    const char* getModeName() const override { return "Brightness"; }
};
```

### Rotation Handler Implementation

```cpp
void BrightnessAdjustModeHandler::handleRotate(int delta) {
    // Calculate new brightness
    int newBrightness = currentBrightness + (delta * STEP_SIZE);

    // Clamp to 0-100%
    if (newBrightness < 0) newBrightness = 0;
    if (newBrightness > 100) newBrightness = 100;

    currentBrightness = static_cast<uint8_t>(newBrightness);

    // Live preview - update LED immediately
    uint8_t ledValue = percentToBrightness(currentBrightness);
    ledController->setBrightness(ledValue);

    // Update display with current percentage
    // (via DisplayRequestQueue)
    LOG_DEBUG("Brightness", "Set to %d%%", currentBrightness);
}
```

### Action Class Pattern

```cpp
// src/Menu/Action/AdjustBrightnessAction.cpp
void AdjustBrightnessAction::execute() {
    LOG_INFO("AdjustBrightness", "Entering brightness adjustment mode");

    // Switch to brightness adjustment mode
    encoderModeManager->setActiveMode(WheelMode::BRIGHTNESS_ADJUST);

    // Menu will be inactive while in adjustment mode
    // Mode handler will return to menu on confirm/cancel
}
```

### Existing Code Locations

```
src/EncoderMode/Handler/                    - Mode handlers
src/EncoderMode/Manager/EncoderModeManager  - Mode switching
src/Hardware/LedController.h/cpp            - LED control (from Story 11.1)
src/Config/ConfigManager.h/cpp              - NVS persistence
src/Menu/Action/                            - Action classes
include/Enum/WheelModeEnum.h                - Mode enum (may need extension)
```

### Key Files to Create/Modify

| File | Change |
|------|--------|
| `include/Config/device_config.h` | Add `KEY_LED_BRIGHTNESS`, `DEFAULT_LED_BRIGHTNESS` |
| `include/Enum/WheelModeEnum.h` | Add `BRIGHTNESS_ADJUST` mode (if not exists) |
| `src/Hardware/LedController.h` | Add brightness member and methods |
| `src/Hardware/LedController.cpp` | Implement `setBrightness()` |
| `src/Config/ConfigManager.h` | Add getter/setter declarations |
| `src/Config/ConfigManager.cpp` | Implement NVS read/write |
| `src/EncoderMode/Handler/BrightnessAdjustModeHandler.h` | New mode handler header |
| `src/EncoderMode/Handler/BrightnessAdjustModeHandler.cpp` | New mode handler implementation |
| `src/Menu/Action/AdjustBrightnessAction.h` | New action class header |
| `src/Menu/Action/AdjustBrightnessAction.cpp` | New action class implementation |
| `src/Menu/Model/MenuTree.h` | Wire action to menu item |
| `src/main.cpp` | Register new mode handler |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Mode Entry Test**:
   - Navigate to LED → Brightness
   - Verify display shows current percentage
   - Verify wheel input is captured by brightness mode
3. **Live Preview Test**:
   - Rotate wheel clockwise
   - Verify LED gets brighter immediately
   - Verify display updates percentage
4. **Boundary Test**:
   - Rotate to 100%, verify can't go higher
   - Rotate to 0%, verify can't go lower
5. **Confirm Test**:
   - Adjust to 75%
   - Press wheel button
   - Verify saved to NVS
   - Verify menu returns to parent
6. **Cancel Test**:
   - Note current brightness (e.g., 50%)
   - Adjust to 90%
   - Long-press wheel button
   - Verify brightness reverts to 50%
   - Verify menu returns to parent
7. **Persistence Test**:
   - Set brightness to 30%
   - Reboot device
   - Verify LED is at 30%

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Blocking in rotation handler
void handleRotate(int delta) {
    ledController->setBrightness(newValue);
    delay(50);  // Never block!
}

// ❌ WRONG - No bounds checking
void handleRotate(int delta) {
    currentBrightness += delta;  // Can overflow/underflow
}

// ❌ WRONG - Save on every rotation
void handleRotate(int delta) {
    ledController->setBrightness(newValue);
    configManager->setLedBrightness(newValue);  // NVS writes are slow!
}

// ❌ WRONG - No revert capability
void handleLongClick() {
    // Just exit, user loses their original setting
}

// ✅ CORRECT - Non-blocking, bounded, save only on confirm
void handleRotate(int delta) {
    int newBrightness = currentBrightness + (delta * STEP_SIZE);
    newBrightness = constrain(newBrightness, 0, 100);
    currentBrightness = static_cast<uint8_t>(newBrightness);

    // Live preview only - no NVS write
    ledController->setBrightness(percentToBrightness(currentBrightness));
}

void handleShortClick() {
    // Save only on confirm
    configManager->setLedBrightness(currentBrightness);
    // Exit mode and return to menu
}

void handleLongClick() {
    // Revert to original
    ledController->setBrightness(percentToBrightness(originalBrightness));
    // Exit mode and return to menu
}
```

### Brightness Adjustment Flow

```
User selects LED → Brightness
    ↓
AdjustBrightnessAction::execute()
    ↓
EncoderModeManager::setActiveMode(BRIGHTNESS_ADJUST)
    ↓
BrightnessAdjustModeHandler::onEnter()
  - Store originalBrightness
  - Show current % on display
    ↓
[User rotates wheel]
    ↓
BrightnessAdjustModeHandler::handleRotate(delta)
  - Update currentBrightness
  - Live preview: ledController->setBrightness()
  - Update display with %
    ↓
[User short-clicks to confirm]
    ↓
BrightnessAdjustModeHandler::handleShortClick()
  - Save: configManager->setLedBrightness()
  - Exit mode, return to menu
```

### References

- [Source: architecture/core-architectural-decisions.md#Component Architecture] - Mode handler pattern
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - File placement
- [Source: project-context.md#Event Architecture Rules] - Non-blocking handlers
- [Source: project-context.md#Configuration Persistence Gotchas] - NVS write validation
- [Source: epics.md#Story 11.3] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

{{agent_model_name_version}}

### Completion Notes

### Files Modified
