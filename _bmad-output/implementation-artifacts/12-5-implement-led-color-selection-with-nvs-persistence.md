# Story 12.5: Implement LED Color Selection with NVS Persistence

Status: ready-for-dev

## Story

As a **user wanting to customize LED color**,
I want **to select a color from a list**,
so that **the strip displays my preferred color**.

## Acceptance Criteria

1. **Given** I navigate to LED → Color
   **When** I see the list of available colors
   **Then** I can scroll through colors using the wheel
   **And** each color is clearly labeled

2. **Given** I select a color
   **When** I confirm the selection
   **Then** the LED strip applies the color immediately
   **And** the selection is saved to NVS

3. **Given** the setting is saved
   **When** I cycle power
   **Then** the color setting is restored from NVS
   **And** the LED strip displays the correct color

4. **Given** multiple colors are available
   **When** I navigate through them
   **Then** all colors are accessible
   **And** currently selected color is visually highlighted

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Define LED Colors** (AC: 1, 4)
  - [ ] Create `include/Enum/LedColor.h`:
    ```cpp
    enum class LedColor : uint8_t {
        RED = 0,
        GREEN = 1,
        BLUE = 2,
        YELLOW = 3,
        ORANGE = 4,
        PURPLE = 5,
        WHITE = 6,
        PINK = 7,
        CYAN = 8
    };
    ```
  - [ ] Document RGB values for each color
  - [ ] Add `LED_COLOR_COUNT` constant

- [ ] **Task 2: Add LED Color to ConfigManager** (AC: 2, 3)
  - [ ] Add `KEY_LED_COLOR` constant to `include/Config/device_config.h`:
    ```cpp
    constexpr const char* KEY_LED_COLOR = "led.color";
    ```
  - [ ] Add `DEFAULT_LED_COLOR` constant:
    ```cpp
    constexpr LedColor DEFAULT_LED_COLOR = LedColor::WHITE;
    ```
  - [ ] Add `getLedColor()` method to `ConfigManager.h`:
    ```cpp
    LedColor getLedColor() const;
    ```
  - [ ] Add `setLedColor()` method to `ConfigManager.h`:
    ```cpp
    Error setLedColor(LedColor color);
    ```
  - [ ] Implement methods in `ConfigManager.cpp`:
    - [ ] `getLedColor()`: Use `readEnumConfig` with validation
    - [ ] `setLedColor()`: Cast to `uint8_t`, save to NVS, return `Error`

- [ ] **Task 3: Extend LedController for Color Support** (AC: 1, 2)
  - [ ] Update `src/Hardware/LedController.h`:
    - [ ] Add `LedColor color` member
    - [ ] Add `void setColor(LedColor color)` method
  - [ ] Implement in `src/Hardware/LedController.cpp`:
    - [ ] `setColor()`: Update `color` member
    - [ ] Apply color to all LEDs based on current mode
    - [ ] Call `update()` to refresh display
    - [ ] Log color change with `LOG_INFO`

- [ ] **Task 4: Create SelectLedColorAction Class** (AC: 1, 2)
  - [ ] Create `src/Menu/Action/SelectLedColorAction.h`:
    - [ ] Inherit from `MenuAction`
    - [ ] Store `LedColor targetColor` member
    - [ ] Store `LedController* ledController` member
    - [ ] Store `ConfigManager* configManager` member
    - [ ] Constructor: `SelectLedColorAction(LedColor color, LedController* led, ConfigManager* config)`
    - [ ] Override `execute()` method
    - [ ] Override `getConfirmationMessage()` method
  - [ ] Create `src/Menu/Action/SelectLedColorAction.cpp`:
    - [ ] Implement `execute()`:
      - [ ] Call `ledController->setColor(targetColor)`
      - [ ] Save to NVS: `configManager->setLedColor(targetColor)`
      - [ ] Log action with `LOG_INFO`
    - [ ] Implement `getConfirmationMessage()`:
      - [ ] Return color name as confirmation

- [ ] **Task 5: Update MenuTree.h with Color Items** (AC: 1, 4)
  - [ ] Populate `ledSubmenu` "Color" item with sub-items:
    - [ ] "Red" with `SelectLedColorAction(LedColor::RED)`
    - [ ] "Green" with `SelectLedColorAction(LedColor::GREEN)`
    - [ ] "Blue" with `SelectLedColorAction(LedColor::BLUE)`
    - [ ] "Yellow" with `SelectLedColorAction(LedColor::YELLOW)`
    - [ ] "Orange" with `SelectLedColorAction(LedColor::ORANGE)`
    - [ ] "Purple" with `SelectLedColorAction(LedColor::PURPLE)`
    - [ ] "White" with `SelectLedColorAction(LedColor::WHITE)`
    - [ ] "Pink" with `SelectLedColorAction(LedColor::PINK)`
    - [ ] "Cyan" with `SelectLedColorAction(LedColor::CYAN)`
  - [ ] Initialize actions in `initLedActions()` function

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Navigate to LED → Color, verify all colors listed
  - [ ] Manual test: Select color, verify LED changes immediately
  - [ ] Manual test: Reboot device, verify color persists

## Dev Notes

### Architecture Compliance

- **Enum Storage**: Use `readEnumConfig` helper for safe enum retrieval with validation
- **Action Pattern**: Create dedicated `MenuAction` subclass following Command pattern
- **Controller Pattern**: `LedController` encapsulates all LED logic
- **NVS Persistence**: LED color should persist across power cycles

### LED Color Definitions

```cpp
// include/Enum/LedColor.h
enum class LedColor : uint8_t {
    RED = 0,      // RGB: 255, 0, 0
    GREEN = 1,    // RGB: 0, 255, 0
    BLUE = 2,     // RGB: 0, 0, 255
    YELLOW = 3,   // RGB: 255, 255, 0
    ORANGE = 4,  // RGB: 255, 128, 0
    PURPLE = 5,   // RGB: 128, 0, 128
    WHITE = 6,    // RGB: 255, 255, 255
    PINK = 7,     // RGB: 255, 0, 128
    CYAN = 8      // RGB: 0, 255, 255
};

constexpr uint8_t LED_COLOR_COUNT = 8;
```

### Color Application Pattern

```cpp
// src/Hardware/LedController.cpp
void LedController::setColor(LedColor newColor) {
    color = newColor;

    CRGB rgbColor;
    switch (newColor) {
        case LedColor::RED:    rgbColor = CRGB(255, 0, 0); break;
        case LedColor::GREEN:  rgbColor = CRGB(0, 255, 0); break;
        case LedColor::BLUE:   rgbColor = CRGB(0, 0, 255); break;
        case LedColor::YELLOW: rgbColor = CRGB(255, 255, 0); break;
        case LedColor::ORANGE: rgbColor = CRGB(255, 128, 0); break;
        case LedColor::PURPLE: rgbColor = CRGB(128, 0, 128); break;
        case LedColor::WHITE:  rgbColor = CRGB(255, 255, 255); break;
        case LedColor::PINK:   rgbColor = CRGB(255, 0, 128); break;
        case LedColor::CYAN:   rgbColor = CRGB(0, 255, 255); break;
    }

    // Apply color to all LEDs
    for (uint8_t i = 0; i < numLeds; i++) {
        leds[i] = rgbColor;
    }

    update();
    LOG_INFO("LedController", "Color set to: %d", static_cast<int>(newColor));
}
```

### Action Class Pattern

```cpp
// From architecture/core-architectural-decisions.md#Command Pattern for Actions
class SelectLedColorAction : public MenuAction {
    LedColor targetColor;
    LedController* ledController;
    ConfigManager* configManager;
public:
    SelectLedColorAction(LedColor color, LedController* led, ConfigManager* config)
        : targetColor(color), ledController(led), configManager(config) {}

    void execute() override {
        LOG_INFO("LedColor", "Setting color to: %d", static_cast<int>(targetColor));

        // Update LED controller
        ledController->setColor(targetColor);

        // Save to NVS
        Error result = configManager->setLedColor(targetColor);
        if (result != Error::OK) {
            LOG_ERROR("LedColor", "Failed to save color: %d", result);
        }
    }

    const char* getConfirmationMessage() const override {
        switch (targetColor) {
            case LedColor::RED:    return "Red";
            case LedColor::GREEN:  return "Green";
            case LedColor::BLUE:   return "Blue";
            case LedColor::YELLOW: return "Yellow";
            case LedColor::ORANGE: return "Orange";
            case LedColor::PURPLE: return "Purple";
            case LedColor::WHITE:  return "White";
            case LedColor::PINK:   return "Pink";
            case LedColor::CYAN:   return "Cyan";
            default: return "Unknown";
        }
    }
};
```

### ConfigManager Pattern

```cpp
// src/Config/ConfigManager.cpp
LedColor ConfigManager::getLedColor() const {
    return readEnumConfig<LedColor>(
        KEY_LED_COLOR,
        DEFAULT_LED_COLOR,
        static_cast<LedColor>(LED_COLOR_COUNT - 1)  // Max valid value
    );
}

Error ConfigManager::setLedColor(LedColor color) {
    if (static_cast<uint8_t>(color) >= LED_COLOR_COUNT) {
        return Error::INVALID_PARAM;
    }

    if (!preferences.putUChar(KEY_LED_COLOR, static_cast<uint8_t>(color))) {
        return Error::NVS_WRITE_FAIL;
    }
    return Error::OK;
}
```

### Existing Code Locations

```
include/Enum/LedColor.h                    - New enum definition
include/Config/device_config.h              - Add KEY_LED_COLOR and DEFAULT_LED_COLOR constants
src/Hardware/LedController.h/cpp             - Add color support
src/Config/ConfigManager.h/cpp             - Add getter/setter methods
src/Menu/Action/SelectLedColorAction.h/cpp  - New action class
src/Menu/Model/MenuTree.h                  - Update menu items
```

### Key Files to Modify

| File | Change |
|------|--------|
| `include/Enum/LedColor.h` | New enum definition |
| `include/Config/device_config.h` | Add `KEY_LED_COLOR` and `DEFAULT_LED_COLOR` constants |
| `src/Hardware/LedController.h` | Add `color` member and `setColor()` method |
| `src/Hardware/LedController.cpp` | Implement color application logic |
| `src/Config/ConfigManager.h` | Add getter/setter method declarations |
| `src/Config/ConfigManager.cpp` | Implement NVS read/write |
| `src/Menu/Action/SelectLedColorAction.h` | New action class header |
| `src/Menu/Action/SelectLedColorAction.cpp` | New action class implementation |
| `src/Menu/Model/MenuTree.h` | Update menu items and action initialization |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Color List Test**:
   - Navigate to LED → Color
   - Verify all 8 colors are listed
   - Verify currently selected color is highlighted
3. **Color Selection Test**:
   - Select each color
   - Verify LED changes immediately
   - Verify confirmation message displays
4. **Persistence Test**:
   - Set color to Red
   - Reboot device
   - Verify Red color is restored
5. **Mode Interaction Test**:
   - Set color to Blue
   - Change mode to Rainbow
   - Verify Rainbow effect uses Blue color

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No enum validation
LedColor getLedColor() const {
    uint8_t stored = preferences.getUChar(KEY_LED_COLOR, 0);
    return static_cast<LedColor>(stored);  // Could return invalid enum
}

// ❌ WRONG - No error handling
void setLedColor(LedColor color) {
    preferences.putUChar(KEY_LED_COLOR, static_cast<uint8_t>(color));
    // No return value, can't detect failure
}

// ❌ WRONG - Wrong NVS key
preferences.putUChar("led_color", ...);  // Wrong key format

// ❌ WRONG - No color application
void setColor(LedColor newColor) {
    color = newColor;
    // LEDs don't actually change color
}

// ❌ WRONG - Dynamic allocation
LedColor* colors = new LedColor[8];  // Never do this

// ✅ CORRECT - With validation, error handling, proper application
LedColor ConfigManager::getLedColor() const {
    return readEnumConfig<LedColor>(
        KEY_LED_COLOR,
        DEFAULT_LED_COLOR,
        static_cast<LedColor>(LED_COLOR_COUNT - 1)
    );
}

Error ConfigManager::setLedColor(LedColor color) {
    if (static_cast<uint8_t>(color) >= LED_COLOR_COUNT) {
        return Error::INVALID_PARAM;
    }

    if (!preferences.putUChar(KEY_LED_COLOR, static_cast<uint8_t>(color))) {
        return Error::NVS_WRITE_FAIL;
    }
    return Error::OK;
}

void LedController::setColor(LedColor newColor) {
    color = newColor;

    CRGB rgbColor;
    switch (newColor) {
        case LedColor::RED:    rgbColor = CRGB(255, 0, 0); break;
        // ... other cases
    }

    for (uint8_t i = 0; i < numLeds; i++) {
        leds[i] = rgbColor;
    }

    update();
    LOG_INFO("LedController", "Color set to: %d", static_cast<int>(newColor));
}
```

### Color Reference Table

| Color | RGB Value | Description |
|-------|-----------|-------------|
| Red | 255, 0, 0 | Primary red |
| Green | 0, 255, 0 | Primary green |
| Blue | 0, 0, 255 | Primary blue |
| Yellow | 255, 255, 0 | Primary yellow |
| Orange | 255, 128, 0 | Orange |
| Purple | 128, 0, 128 | Purple |
| White | 255, 255, 255 | White (all on) |
| Pink | 255, 0, 128 | Pink |
| Cyan | 0, 255, 255 | Cyan |

### References

- [Source: architecture/core-architectural-decisions.md#Data Architecture] - NVS namespace and enum storage pattern
- [Source: architecture/core-architectural-decisions.md#Command Pattern for Actions] - Action class structure
- [Source: architecture/implementation-patterns-consistency-rules.md#Format Patterns] - Error return pattern
- [Source: project-context.md#Configuration Persistence Gotchas] - NVS write validation
- [Source: epics.md#Story 12.5] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
