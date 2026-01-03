# Epic 1: Display & Persistence Foundation

**Goal:** User can see device feedback via serial output and their settings are remembered across power cycles.

**FRs covered:** FR26, FR27, FR28, FR29, FR35, FR36, FR37

---

## Story 1.1: Create Foundation Enums and Configuration Headers

As a **developer**,
I want **standardized enums, error codes, and logging macros**,
So that **all subsequent components have consistent foundations to build upon**.

**Acceptance Criteria:**

**Given** the project needs standardized error handling
**When** I create `include/Enum/ErrorEnum.h`
**Then** it defines `enum class Error : uint8_t` with values: `OK`, `INVALID_PARAM`, `NVS_READ_FAIL`, `NVS_WRITE_FAIL`, `INVALID_STATE`
**And** all enum values use UPPER_SNAKE_CASE naming

**Given** the project needs wheel mode definitions
**When** I create `include/Enum/WheelModeEnum.h`
**Then** it defines `enum class WheelMode : uint8_t` with values: `SCROLL`, `VOLUME`, `ZOOM`
**And** includes a `WheelMode_MAX` constant for validation

**Given** the project needs button action definitions
**When** I create `include/Enum/ButtonActionEnum.h`
**Then** it defines `enum class ButtonAction : uint8_t` with values: `NONE`, `MUTE`, `PLAY`, `PAUSE`, `NEXT`, `PREVIOUS`
**And** includes a `ButtonAction_MAX` constant for validation

**Given** the project needs standardized logging
**When** I create `include/Config/log_config.h`
**Then** it defines `LOG_ERROR`, `LOG_INFO`, `LOG_DEBUG` macros
**And** log output format is `[LEVEL][Tag] Message`
**And** log level is controllable via build flag `LOG_LEVEL`

**Given** the project needs button GPIO configuration
**When** I create `include/Config/button_config.h`
**Then** it defines `struct ButtonConfig { uint8_t pin; const char* label; bool activeLow; }`
**And** defines `inline constexpr ButtonConfig BUTTONS[]` array with 4 default buttons
**And** defines `inline constexpr size_t BUTTON_COUNT`
**And** defines `inline constexpr uint32_t DEBOUNCE_MS = 50`

---

## Story 1.2: Create Display Interface and Serial Implementation

As a **user**,
I want **to see device output via serial monitor**,
So that **I can observe menu navigation, status messages, and configuration feedback**.

**Acceptance Criteria:**

**Given** the project needs display abstraction for future hardware display support
**When** I create `src/Display/Interface/DisplayInterface.h`
**Then** it defines an abstract class with pure virtual methods:
- `virtual void showMenu(const char* title, const char** items, uint8_t count, uint8_t selected) = 0`
- `virtual void showMessage(const char* message) = 0`
- `virtual void showConfirmation(const char* message) = 0`
- `virtual void showStatus(const char* key, const char* value) = 0`
- `virtual void clear() = 0`
**And** uses `#pragma once` header guard

**Given** the project needs serial output for MVP
**When** I create `src/Display/Impl/SerialDisplay.cpp` and `SerialDisplay.h`
**Then** `SerialDisplay` implements `DisplayInterface`
**And** `showMenu()` outputs formatted menu with current selection indicator
**And** `showMessage()` outputs text with `[MSG]` prefix
**And** `showConfirmation()` outputs text with `[OK]` prefix
**And** `showStatus()` outputs `key: value` format
**And** `clear()` outputs a separator line

**Given** the display needs initialization
**When** `SerialDisplay` is constructed
**Then** it accepts a `HardwareSerial*` reference (default `&Serial`)
**And** uses `LOG_INFO` for initialization confirmation

**Given** menu display needs clear formatting
**When** `showMenu()` is called with items
**Then** output shows title on first line
**And** each item is numbered (1, 2, 3...)
**And** selected item is marked with `>` indicator
**And** output is readable in serial monitor at 115200 baud

---

## Story 1.3: Create Configuration Manager with NVS Persistence

As a **user**,
I want **my wheel mode and button behavior settings saved to device storage**,
So that **my configuration persists across power cycles without needing to reconfigure**.

**Acceptance Criteria:**

**Given** the project needs configuration persistence
**When** I create `src/Config/ConfigManager.cpp` and `ConfigManager.h`
**Then** `ConfigManager` class accepts `Preferences*` via constructor (dependency injection for testability)
**And** uses NVS namespace `"knobkoky"`
**And** uses `#pragma once` header guard

**Given** the user changes wheel mode
**When** `saveWheelMode(WheelMode mode)` is called
**Then** mode is saved to NVS key `"wheel.mode"` as `uint8_t`
**And** function returns `Error::OK` on success
**And** function returns `Error::NVS_WRITE_FAIL` on failure
**And** `LOG_INFO` confirms the save operation

**Given** the system needs to read wheel mode
**When** `loadWheelMode()` is called
**Then** it reads from NVS key `"wheel.mode"`
**And** validates the value is within `WheelMode_MAX` range
**And** returns `WheelMode::SCROLL` as default if key doesn't exist or value is invalid
**And** `LOG_DEBUG` reports the loaded value

**Given** the user assigns a button behavior
**When** `saveButtonAction(uint8_t buttonIndex, ButtonAction action)` is called
**Then** action is saved to NVS key `"btnN.action"` (where N = buttonIndex)
**And** function returns `Error::INVALID_PARAM` if buttonIndex >= `BUTTON_COUNT`
**And** function returns `Error::OK` on success
**And** function returns `Error::NVS_WRITE_FAIL` on failure

**Given** the system needs to read button behavior
**When** `loadButtonAction(uint8_t buttonIndex)` is called
**Then** it reads from NVS key `"btnN.action"`
**And** validates the value is within `ButtonAction_MAX` range
**And** returns `ButtonAction::NONE` as default if key doesn't exist or value is invalid

**Given** configuration might become corrupted
**When** invalid enum values are read from NVS
**Then** the validation wrapper returns the default value
**And** `LOG_ERROR` reports the corruption with key name and invalid value

---

## Story 1.4: Implement Boot Configuration Restore and Factory Reset

As a **user**,
I want **my saved settings automatically restored when the device powers on**,
So that **I can immediately use my configured wheel mode and button behaviors without manual setup**.

**Acceptance Criteria:**

**Given** the device boots with saved configuration
**When** the system initializes
**Then** `ConfigManager::loadWheelMode()` is called to restore wheel mode
**And** `ConfigManager::loadButtonAction()` is called for each button (0 to BUTTON_COUNT-1)
**And** restored values are applied to the active mode manager
**And** `LOG_INFO` reports "Configuration restored" with mode name

**Given** the device boots with no saved configuration (first boot)
**When** no NVS keys exist
**Then** wheel mode defaults to `WheelMode::SCROLL`
**And** all button actions default to `ButtonAction::NONE`
**And** `LOG_INFO` reports "Using default configuration"
**And** device operates normally with defaults

**Given** the user wants to reset all settings
**When** I create `src/Config/FactoryReset.cpp` and `FactoryReset.h`
**Then** `FactoryReset::isResetRequested()` checks if encoder button is held during boot
**And** `FactoryReset::execute(ConfigManager&, DisplayInterface&)` clears all NVS config

**Given** the user holds encoder button during power-on
**When** button is held for 5+ seconds at boot
**Then** `isResetRequested()` returns `true`
**And** display shows "Factory Reset..." message
**And** all NVS keys in `"knobkoky"` namespace are cleared
**And** display shows "Reset Complete" confirmation
**And** `LOG_INFO` reports "Factory reset performed"
**And** device continues boot with default configuration

**Given** factory reset is checked in main.cpp
**When** `setup()` runs
**Then** `FactoryReset::isResetRequested()` is checked BEFORE normal ConfigManager initialization
**And** if reset requested, `FactoryReset::execute()` runs before loading config
**And** if not requested, normal config restore proceeds

**Given** the encoder button is briefly pressed during boot
**When** button is held for less than 5 seconds
**Then** `isResetRequested()` returns `false`
**And** normal boot proceeds without reset

---
