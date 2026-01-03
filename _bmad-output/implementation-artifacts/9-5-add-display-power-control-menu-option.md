# Story 9.5: Add Display Power Control Menu Option

Status: ready-for-dev

## Story

As a **user wanting to save power or reduce distraction**,
I want **to turn off the display from the menu**,
so that **the screen goes dark when I don't need it**.

## Acceptance Criteria

1. **Given** I am in the menu
   **When** I navigate to "Display Off" and select it
   **Then** OLED display turns off (blank/power-save mode)
   **And** menu exits or returns to parent

2. **Given** display is off
   **When** device continues to run
   **Then** functions (buttons, encoder) still work normally
   **And** device remains responsive to inputs

3. **Given** display is off
   **When** I long-press the wheel button (Menu activation)
   **Then** display wakes (covered in Story 9.6)
   **And** menu opens

4. **Given** display power state changes
   **When** I toggle display on/off
   **Then** state persists across power cycles
   **And** menu shows correct state on next boot

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Add setPower() to DisplayInterface** (AC: 1, 2)
  - [ ] Update `src/Display/Interface/DisplayInterface.h`:
    - [ ] Add virtual method: `virtual void setPower(bool on) = 0;`
  - [ ] Document that implementations should use hardware power commands

- [ ] **Task 2: Implement setPower() in OLEDDisplay** (AC: 1)
  - [ ] Update `src/Display/Impl/OLEDDisplay.h`:
    - [ ] Override `setPower(bool on)` method
    - [ ] Add `bool displayOn` member to track state
  - [ ] Implement in `OLEDDisplay.cpp`:
    - [ ] If `on` is true:
      - [ ] Call `display.ssd1306_command(SSD1306_DISPLAYON)`
      - [ ] Set `displayOn = true`
      - [ ] Log with `LOG_INFO`
    - [ ] If `on` is false:
      - [ ] Call `display.ssd1306_command(SSD1306_DISPLAYOFF)`
      - [ ] Set `displayOn = false`
      - [ ] Log with `LOG_INFO`

- [ ] **Task 3: Add Display Power State to ConfigManager** (AC: 4)
  - [ ] Add `KEY_DISPLAY_POWER` constant to `device_config.h`:
    ```cpp
    constexpr const char* KEY_DISPLAY_POWER = "display.power";
    ```
  - [ ] Add `getDisplayPower()` method to `ConfigManager.h`:
    ```cpp
    bool getDisplayPower() const;
    ```
  - [ ] Add `setDisplayPower()` method to `ConfigManager.h`:
    ```cpp
    Error setDisplayPower(bool on);
    ```
  - [ ] Implement methods in `ConfigManager.cpp`:
    - [ ] Use `preferences.getBool()` with default `true`
    - [ ] Use `preferences.putBool()` to save state

- [ ] **Task 4: Create DisplayPowerAction Class** (AC: 1)
  - [ ] Create `src/Menu/Action/DisplayPowerAction.h`:
    - [ ] Inherit from `MenuAction`
    - [ ] Store `DisplayInterface* display` member
    - [ ] Store `ConfigManager* configManager` member
    - [ ] Constructor: `DisplayPowerAction(DisplayInterface* disp, ConfigManager* config)`
    - [ ] Override `execute()` method
    - [ ] Override `getConfirmationMessage()` method
  - [ ] Create `src/Menu/Action/DisplayPowerAction.cpp`:
    - [ ] Implement `execute()`:
      - [ ] Toggle display power: `display->setPower(!currentPower)`
      - [ ] Save new state to NVS: `configManager->setDisplayPower(newPower)`
      - [ ] Log action with `LOG_INFO`
    - [ ] Implement `getConfirmationMessage()`:
      - [ ] Return "Display Off" or "Display On" based on new state

- [ ] **Task 5: Add Menu Item to MenuTree** (AC: 1)
  - [ ] Add "Display Off" item to `mainMenu` array
  - [ ] Initialize `DisplayPowerAction` in init function
  - [ ] Assign action to menu item
  - [ ] Ensure parent pointers are set correctly

- [ ] **Task 6: Initialize Display Power on Boot** (AC: 4)
  - [ ] Update `main.cpp`:
    - [ ] After OLEDDisplay initialization, read power state from ConfigManager
    - [ ] Call `display->setPower(configManager->getDisplayPower())`
    - [ ] Ensure display starts in correct state

- [ ] **Task 7: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Select "Display Off", verify screen goes dark
  - [ ] Manual test: Verify buttons/encoder still work when display is off
  - [ ] Manual test: Reboot device, verify display state persists

## Dev Notes

### Architecture Compliance

- **Interface Segregation**: `DisplayInterface` needs power control method
- **Action Pattern**: Create dedicated `MenuAction` subclass following Command pattern
- **NVS Persistence**: Display power state should persist across power cycles
- **Non-Blocking**: Power control should be immediate, no blocking waits

### SSD1306 Power Commands

```cpp
// Adafruit SSD1306 library commands
display.ssd1306_command(SSD1306_DISPLAYON);   // Turn on display
display.ssd1306_command(SSD1306_DISPLAYOFF);  // Turn off display (power save)
```

### Display Power State Machine

```
ON ──[select "Display Off"]──> OFF
 │                              │
 │                              └─[wake action]──> ON (Story 9.6)
 │
 └─[select "Display On"]─────────┘
```

### Action Class Pattern

```cpp
// From architecture/core-architectural-decisions.md#Command Pattern for Actions
class DisplayPowerAction : public MenuAction {
    DisplayInterface* display;
    ConfigManager* configManager;
public:
    DisplayPowerAction(DisplayInterface* disp, ConfigManager* config)
        : display(disp), configManager(config) {}

    void execute() override {
        bool currentPower = configManager->getDisplayPower();
        bool newPower = !currentPower;

        LOG_INFO("DisplayPower", "Toggling display: %d", newPower);

        // Update display hardware
        display->setPower(newPower);

        // Save to NVS
        Error result = configManager->setDisplayPower(newPower);
        if (result != Error::OK) {
            LOG_ERROR("DisplayPower", "Failed to save state: %d", result);
        }
    }

    const char* getConfirmationMessage() const override {
        return configManager->getDisplayPower() ? "Display Off" : "Display On";
    }
};
```

### ConfigManager Pattern

```cpp
// src/Config/ConfigManager.cpp
bool ConfigManager::getDisplayPower() const {
    return preferences.getBool(KEY_DISPLAY_POWER, true);  // Default: on
}

Error ConfigManager::setDisplayPower(bool on) {
    if (!preferences.putBool(KEY_DISPLAY_POWER, on)) {
        return Error::NVS_WRITE_FAIL;
    }
    return Error::OK;
}
```

### Existing Code Locations

```
src/Display/Interface/DisplayInterface.h  - Add setPower() method
src/Display/Impl/OLEDDisplay.h/cpp       - Implement power control
src/Config/ConfigManager.h/cpp             - Add power state persistence
src/Menu/Action/DisplayPowerAction.h/cpp   - New action class
src/Menu/Model/MenuTree.h                - Add menu item
src/main.cpp                                 - Initialize display power on boot
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Display/Interface/DisplayInterface.h` | Add `setPower()` method declaration |
| `src/Display/Impl/OLEDDisplay.h` | Override `setPower()` method |
| `src/Display/Impl/OLEDDisplay.cpp` | Implement SSD1306 power commands |
| `include/Config/device_config.h` | Add `KEY_DISPLAY_POWER` constant |
| `src/Config/ConfigManager.h` | Add getter/setter methods |
| `src/Config/ConfigManager.cpp` | Implement NVS read/write |
| `src/Menu/Action/DisplayPowerAction.h` | New action class header |
| `src/Menu/Action/DisplayPowerAction.cpp` | New action class implementation |
| `src/Menu/Model/MenuTree.h` | Add menu item |
| `src/main.cpp` | Initialize display power on boot |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Power Off Test**:
   - Navigate to "Display Off"
   - Select it
   - Verify screen goes dark
   - Verify menu exits
3. **Functionality Test**:
   - With display off, press buttons
   - Verify device still responds
   - Verify encoder still works
4. **Persistence Test**:
   - Turn display off
   - Reboot device
   - Verify display stays off
   - Turn display on
   - Reboot device
   - Verify display stays on
5. **Wake Test** (for Story 9.6):
   - Turn display off
   - Long-press wheel button
   - Verify display wakes and menu opens

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No NVS persistence
void DisplayPowerAction::execute() {
    display->setPower(false);
    // State lost on reboot
}

// ❌ WRONG - No confirmation message
const char* getConfirmationMessage() const override {
    return nullptr;  // User has no feedback
}

// ❌ WRONG - Wrong SSD1306 command
display.ssd1306_command(0xAE);  // Magic number, unclear intent
// Use: SSD1306_DISPLAYOFF

// ❌ WRONG - Blocking wait
void setPower(bool on) {
    display.ssd1306_command(on ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
    delay(100);  // Unnecessary blocking
}

// ❌ WRONG - No state tracking
void setPower(bool on) {
    display.ssd1306_command(on ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
    // Can't query current state
}

// ✅ CORRECT - With persistence, feedback, state tracking
void DisplayPowerAction::execute() {
    bool currentPower = configManager->getDisplayPower();
    bool newPower = !currentPower;

    LOG_INFO("DisplayPower", "Toggling display: %d", newPower);

    // Update display hardware
    display->setPower(newPower);

    // Save to NVS
    Error result = configManager->setDisplayPower(newPower);
    if (result != Error::OK) {
        LOG_ERROR("DisplayPower", "Failed to save state: %d", result);
    }
}

void OLEDDisplay::setPower(bool on) {
    if (on) {
        display.ssd1306_command(SSD1306_DISPLAYON);
        displayOn = true;
        LOG_INFO("OLED", "Display ON");
    } else {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        displayOn = false;
        LOG_INFO("OLED", "Display OFF");
    }
}
```

### Display Power Flow

```
User Action: Select "Display Off"
    ↓
DisplayPowerAction::execute()
    ↓
Toggle power state
    ↓
display->setPower(false)
    ↓
SSD1306_DISPLAYOFF command
    ↓
Display goes dark
    ↓
Save to NVS
    ↓
Menu exits
```

### References

- [Source: architecture/core-architectural-decisions.md#Command Pattern for Actions] - Action class structure
- [Source: architecture/implementation-patterns-consistency-rules.md#Format Patterns] - Error return pattern
- [Source: project-context.md#Configuration Persistence Gotchas] - NVS write validation
- [Source: epics.md#Story 9.5] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
