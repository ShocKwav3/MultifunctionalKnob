---
stepsCompleted: [1, 2, 3, 4]
status: complete
completedAt: '2025-12-16'
inputDocuments:
  - docs/prd.md
  - docs/architecture.md
project_name: UtilityButtonsWithKnobUSB
user_name: Feroj
date: '2025-12-16'
---

# UtilityButtonsWithKnobUSB - Epic Breakdown

## Overview

This document provides the complete epic and story breakdown for UtilityButtonsWithKnobUSB, decomposing the requirements from the PRD and Architecture into implementable stories.

## Requirements Inventory

### Functional Requirements

**Menu System (FR1-FR8):**
- FR1: User can enter the main menu by long-pressing the encoder button
- FR2: User can navigate menu items by rotating the encoder wheel
- FR3: User can select a menu item by short-pressing the encoder button
- FR4: User can return to the previous menu level by long-pressing the encoder button
- FR5: User can exit the menu system by long-pressing from the main menu level
- FR6: System displays the current menu item and available options via the display interface
- FR7: System displays confirmation feedback when a selection is made
- FR8: System automatically exits menu and returns to normal operation after configuration change

**Wheel Behavior Configuration (FR9-FR14):**
- FR9: User can view available wheel behavior modes (Scroll, Volume, Zoom)
- FR10: User can select and activate a wheel behavior mode
- FR11: System applies the selected wheel behavior immediately after selection
- FR12: User can use Scroll mode to send mouse scroll events (vertical)
- FR13: User can use Volume mode to send media volume up/down events
- FR14: User can use Zoom mode to send zoom in/out events

**Button Configuration (FR15-FR19):**
- FR15: User can view all configured buttons and their current behavior assignments
- FR16: User can select a button to configure its behavior
- FR17: User can assign a predefined behavior to a button (Mute, Play, Pause, Next, Previous)
- FR18: System applies button behavior assignment immediately after selection
- FR19: System sends the configured HID command when user presses a configured button

**Device Status (FR20-FR23):**
- FR20: User can view current wheel behavior mode
- FR21: User can view current button behavior assignments
- FR22: User can view Bluetooth connection status
- FR23: User can view Bluetooth connection strength (when available)

**About Screen (FR24-FR25):**
- FR24: User can view device name and firmware version
- FR25: User can view manufacturer information

**Configuration Persistence (FR26-FR29):**
- FR26: System saves wheel behavior mode to non-volatile storage when changed
- FR27: System saves button behavior assignments to non-volatile storage when changed
- FR28: System restores saved configuration automatically on boot
- FR29: System operates with default configuration if no saved configuration exists

**Bluetooth Connectivity (FR30-FR34):**
- FR30: System advertises as BLE HID device on boot
- FR31: System automatically reconnects to last paired host device
- FR32: System supports standard BLE pairing flow
- FR33: System sends HID keyboard/media key events for button actions
- FR34: System sends HID mouse scroll events for wheel actions

**Display Interface (FR35-FR37):**
- FR35: System outputs menu and status information to serial interface
- FR36: System provides abstract display interface for future hardware display support
- FR37: Display interface supports text output for menu items and status messages

**Input Handling (FR38-FR42):**
- FR38: System detects encoder rotation direction and magnitude
- FR39: System detects encoder button short press (< configurable threshold)
- FR40: System detects encoder button long press (>= configurable threshold)
- FR41: System detects GPIO button presses for configured buttons
- FR42: System processes inputs via event queue (non-blocking)

### Non-Functional Requirements

**Performance:**
- NFR1: Menu navigation shall feel responsive with no perceptible lag between input and display update

**Reliability:**
- NFR2: Saved configuration shall persist across power cycles with zero data loss
- NFR3: System shall handle invalid/corrupted configuration gracefully by falling back to defaults
- NFR4: Input events shall not be dropped under normal operation (queue-based processing)

**Maintainability:**
- NFR5: Code shall follow consistent naming conventions throughout the codebase
- NFR6: Architecture shall follow established patterns (event-driven, handler pattern, singleton where appropriate)
- NFR7: Display abstraction shall allow new display implementations without modifying menu logic
- NFR8: New wheel modes shall be addable by implementing a defined handler interface
- NFR9: New button behaviors shall be addable by extending the behavior registry
- NFR10: Code shall include header documentation for public interfaces
- NFR11: Build process shall be documented and reproducible via PlatformIO

**Compatibility:**
- NFR12: Device shall work with ESP32-C3 Super Mini board as reference hardware

### Additional Requirements

**From Architecture - Technical Constraints:**
- Static menu tree with parent pointers (no dynamic allocation, compile-time menu definition)
- Event interceptor pattern for menu activation (menu intercepts events at higher level)
- Command pattern for action execution (MenuAction base class with execute() method)
- NVS persistence using single namespace "knobkoky" with prefixed keys (wheel.mode, btn0.action, etc.)
- Enum storage with validation wrapper for graceful fallback on corruption
- 5-phase build order with specific component dependencies

**From Architecture - Implementation Requirements:**
- Display abstraction (DisplayInterface) with SerialDisplay implementation for MVP
- Button configuration via constexpr array of ButtonConfig structs
- Factory reset capability (encoder button held 5+ seconds during boot clears NVS)
- BLE disconnect handling - exit menu to root and show "Disconnected" status
- DI-enabled ConfigManager accepting Preferences* for testability
- LOG_* macros (LOG_ERROR, LOG_INFO, LOG_DEBUG) for all logging - no direct Serial.print
- Error enum return from all fallible operations (OK, INVALID_PARAM, NVS_READ_FAIL, NVS_WRITE_FAIL, INVALID_STATE)

**From Architecture - Directory Structure:**
- New directories: Menu/, Display/, Config/, Button/
- Subdirectory pattern: Controller/, Handler/, Interface/, Impl/, Model/, Action/
- Header files in include/Config/, include/Enum/, include/Type/

### FR Coverage Map

| FR | Epic | Description |
|----|------|-------------|
| FR1 | Epic 2 | Long-press enters menu |
| FR2 | Epic 2 | Wheel navigates menu |
| FR3 | Epic 2 | Short-press selects item |
| FR4 | Epic 2 | Long-press returns to previous level |
| FR5 | Epic 2 | Long-press from main exits menu |
| FR6 | Epic 2 | Display shows current menu/options |
| FR7 | Epic 2 | Confirmation feedback on selection |
| FR8 | Epic 2 | Auto-exit after config change |
| FR9 | Epic 3 | View available wheel modes |
| FR10 | Epic 3 | Select wheel mode |
| FR11 | Epic 3 | Immediate mode application |
| FR12 | Epic 3 | Scroll mode functionality |
| FR13 | Epic 3 | Volume mode functionality |
| FR14 | Epic 3 | Zoom mode functionality |
| FR15 | Epic 4 | View button assignments |
| FR16 | Epic 4 | Select button to configure |
| FR17 | Epic 4 | Assign predefined behavior |
| FR18 | Epic 4 | Immediate behavior application |
| FR19 | Epic 4 | HID command on button press |
| FR20 | Epic 5 | View current wheel mode |
| FR21 | Epic 5 | View button assignments |
| FR22 | Epic 5 | View BLE connection status |
| FR23 | Epic 5 | View BLE connection strength |
| FR24 | Epic 5 | View device name/version |
| FR25 | Epic 5 | View manufacturer info |
| FR26 | Epic 1 | Save wheel mode to NVS |
| FR27 | Epic 1 | Save button behaviors to NVS |
| FR28 | Epic 1 | Restore config on boot |
| FR29 | Epic 1 | Default config if none exists |
| FR30 | Existing | BLE advertising |
| FR31 | Existing | Auto-reconnect |
| FR32 | Existing | BLE pairing |
| FR33 | Epic 4 | HID keyboard/media events |
| FR34 | Existing | HID scroll events |
| FR35 | Epic 1 | Serial output |
| FR36 | Epic 1 | Abstract display interface |
| FR37 | Epic 1 | Text output support |
| FR38 | Existing | Encoder rotation detection |
| FR39 | Existing | Short press detection |
| FR40 | Existing | Long press detection |
| FR41 | Epic 4 | GPIO button detection |
| FR42 | Existing | Event queue processing |

---

## Epic 1: Display & Persistence Foundation

**Goal:** User can see device feedback via serial output and their settings are remembered across power cycles.

**FRs covered:** FR26, FR27, FR28, FR29, FR35, FR36, FR37

---

### Story 1.1: Create Foundation Enums and Configuration Headers

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
**And** defines `constexpr ButtonConfig BUTTONS[]` array with 4 default buttons
**And** defines `constexpr size_t BUTTON_COUNT`

---

### Story 1.2: Create Display Interface and Serial Implementation

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

### Story 1.3: Create Configuration Manager with NVS Persistence

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

### Story 1.4: Implement Boot Configuration Restore and Factory Reset

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

## Epic 2: Menu Navigation System

**Goal:** User can enter, navigate, select items, and exit the hierarchical menu using the encoder's two-input model (wheel rotation + button press).

**FRs covered:** FR1, FR2, FR3, FR4, FR5, FR6, FR7, FR8

---

### Story 2.1: Create Menu Item Data Structure and Static Menu Tree

As a **developer**,
I want **a compile-time menu tree structure with parent-child relationships**,
So that **menu navigation can traverse hierarchically without dynamic memory allocation**.

**Acceptance Criteria:**

**Given** the project needs a menu item data structure
**When** I create `src/Menu/Model/MenuItem.h`
**Then** it defines:
```cpp
struct MenuItem {
    const char* label;
    MenuItem* parent;
    MenuItem* const* children;
    uint8_t childCount;
    MenuAction* action;
};
```
**And** uses `#pragma once` header guard

**Given** the project needs an abstract action interface
**When** I create `src/Menu/Action/MenuAction.h`
**Then** it defines:
```cpp
class MenuAction {
public:
    virtual ~MenuAction() = default;
    virtual void execute() = 0;
    virtual const char* getConfirmationMessage() const { return nullptr; }
};
```

**Given** the menu system needs a static tree definition
**When** I create `src/Menu/Model/MenuTree.h`
**Then** it defines the 4-item main menu as `constexpr`:
- "Wheel Behavior" (has children for modes)
- "Button Config" (has children for buttons)
- "Device Status" (leaf with action)
- "About" (leaf with action)
**And** wheel behavior submenu has children: "Scroll", "Volume", "Zoom"
**And** parent pointers enable back navigation
**And** leaf items have non-null `action` pointers

**Given** a menu item is a branch (has children)
**When** the item is selected
**Then** `childCount > 0` and `children != nullptr`
**And** `action == nullptr` (branches don't execute actions)

**Given** a menu item is a leaf (no children)
**When** the item is selected
**Then** `childCount == 0` and `children == nullptr`
**And** `action != nullptr` (leaves execute actions)

---

### Story 2.2: Create Menu Controller State Machine

As a **user**,
I want **to navigate through menu levels using wheel rotation and button presses**,
So that **I can browse options and make selections intuitively**.

**Acceptance Criteria:**

**Given** the project needs menu state management
**When** I create `src/Menu/Controller/MenuController.cpp` and `MenuController.h`
**Then** `MenuController` class tracks:
- `bool menuActive` - whether menu is currently displayed
- `MenuItem* currentMenu` - pointer to current menu/submenu
- `uint8_t selectedIndex` - currently highlighted item index
**And** uses `#pragma once` header guard

**Given** the menu is inactive
**When** `isActive()` is called
**Then** it returns `false`
**And** encoder events pass through to normal mode handlers

**Given** the menu is active and user rotates encoder clockwise
**When** `handleRotation(int delta)` is called with positive delta
**Then** `selectedIndex` increments (wraps at `currentMenu->childCount`)
**And** display update event is dispatched via AppEventDispatcher
**And** `LOG_DEBUG` reports new selection index

**Given** the menu is active and user rotates encoder counter-clockwise
**When** `handleRotation(int delta)` is called with negative delta
**Then** `selectedIndex` decrements (wraps at 0 to `childCount - 1`)
**And** display update event is dispatched

**Given** the menu is active and user short-presses on a branch item
**When** `handleSelect()` is called and selected item has children
**Then** `currentMenu` changes to the selected child's children array
**And** `selectedIndex` resets to 0
**And** display update event is dispatched
**And** `LOG_INFO` reports "Entered submenu: {label}"

**Given** the menu is active and user short-presses on a leaf item
**When** `handleSelect()` is called and selected item has an action
**Then** `action->execute()` is called
**And** confirmation message is dispatched if `getConfirmationMessage()` returns non-null
**And** menu exits automatically (FR8)
**And** `LOG_INFO` reports "Executed: {label}"

**Given** the menu is active at a submenu level and user long-presses
**When** `handleBack()` is called and `currentMenu->parent != nullptr`
**Then** `currentMenu` changes to parent menu
**And** `selectedIndex` resets to 0
**And** display update event is dispatched
**And** `LOG_DEBUG` reports "Back to: {parent label}"

**Given** the menu is active at main menu level and user long-presses
**When** `handleBack()` is called and `currentMenu->parent == nullptr`
**Then** `menuActive` is set to `false`
**And** menu deactivation event is dispatched
**And** `LOG_INFO` reports "Menu closed"

---

### Story 2.3: Integrate Menu with Encoder Event System

As a **user**,
I want **to activate the menu by long-pressing the encoder button**,
So that **I can access configuration options without interrupting normal operation**.

**Acceptance Criteria:**

**Given** the menu system needs to intercept encoder events
**When** I modify `src/Event/Handler/EncoderEventHandler.cpp`
**Then** it checks `MenuController::isActive()` before dispatching to mode handlers
**And** if menu is active, events route to `MenuController` instead

**Given** the device is in normal operation mode
**When** user long-presses the encoder button
**Then** `MenuController::activate()` is called
**And** `menuActive` is set to `true`
**And** `currentMenu` is set to main menu root
**And** `selectedIndex` is set to 0
**And** `MENU_ACTIVATED` event is dispatched via AppEventDispatcher
**And** `LOG_INFO` reports "Menu activated"

**Given** the menu is active
**When** encoder rotation event is received
**Then** `MenuController::handleRotation(delta)` is called
**And** event is consumed (not passed to mode handlers)

**Given** the menu is active
**When** encoder short-press event is received
**Then** `MenuController::handleSelect()` is called
**And** event is consumed

**Given** the menu is active
**When** encoder long-press event is received
**Then** `MenuController::handleBack()` is called
**And** event is consumed

**Given** the menu exits (via back from main or action execution)
**When** `menuActive` becomes `false`
**Then** `MENU_DEACTIVATED` event is dispatched
**And** normal encoder event routing resumes
**And** current wheel mode handler receives subsequent events

**Given** new event types are needed
**When** I modify `include/Enum/EventEnum.h`
**Then** it includes: `MENU_ACTIVATED`, `MENU_DEACTIVATED`, `MENU_ITEM_SELECTED`, `MENU_NAVIGATION_CHANGED`

---

### Story 2.4: Create Display Handler for Menu Events

As a **user**,
I want **to see the current menu state and confirmation messages on the display**,
So that **I know which option is selected and when my actions are confirmed**.

**Acceptance Criteria:**

**Given** the project needs to route display events
**When** I create `src/Display/Handler/DisplayHandler.cpp` and `DisplayHandler.h`
**Then** `DisplayHandler` class:
- Holds reference to `DisplayInterface*`
- Subscribes to AppEvent queue
- Routes events to appropriate display methods
**And** uses `#pragma once` header guard

**Given** menu is activated
**When** `MENU_ACTIVATED` event is received
**Then** `DisplayHandler` calls `display->clear()`
**And** calls `display->showMenu()` with main menu items and selection index 0
**And** `LOG_DEBUG` reports "Display: Menu activated"

**Given** user navigates within menu
**When** `MENU_NAVIGATION_CHANGED` event is received
**Then** event payload contains: `MenuItem* currentMenu`, `uint8_t selectedIndex`
**And** `DisplayHandler` calls `display->showMenu()` with current items and selection
**And** display updates to show new selection indicator

**Given** user selects an action item
**When** `MENU_ITEM_SELECTED` event is received with confirmation message
**Then** event payload contains: `const char* confirmationMessage`
**And** `DisplayHandler` calls `display->showConfirmation(message)`
**And** confirmation is visible for user feedback (FR7)

**Given** menu is deactivated
**When** `MENU_DEACTIVATED` event is received
**Then** `DisplayHandler` calls `display->clear()`
**And** `display->showMessage("Ready")` indicates return to normal mode
**And** `LOG_DEBUG` reports "Display: Menu closed"

**Given** AppEvent needs to carry menu data
**When** I modify `include/Type/AppEvent.h`
**Then** it includes union-based payload:
```cpp
union {
    struct { MenuItem* menu; uint8_t index; } navigation;
    struct { const char* message; } confirmation;
} data;
```

**Given** DisplayHandler needs initialization
**When** `DisplayHandler` is constructed in `main.cpp`
**Then** it receives `DisplayInterface*` and `AppEventDispatcher*` references
**And** registers itself as event listener for menu-related events

---

## Epic 3: Wheel Behavior Configuration

**Goal:** User can view available wheel modes and switch between Scroll, Volume, and Zoom at runtime through the menu.

**FRs covered:** FR9, FR10, FR11, FR12, FR13, FR14

---

### Story 3.1: Create Set Wheel Mode Action

As a **user**,
I want **to select a wheel mode from the menu and have it applied immediately**,
So that **my wheel behavior changes without needing to restart the device**.

**Acceptance Criteria:**

**Given** the project needs wheel mode actions
**When** I create `src/Menu/Action/SetWheelModeAction.cpp` and `SetWheelModeAction.h`
**Then** `SetWheelModeAction` extends `MenuAction`
**And** constructor accepts `WheelMode targetMode` and `ConfigManager*` and `EncoderModeManager*`
**And** uses `#pragma once` header guard

**Given** user selects a wheel mode from menu
**When** `execute()` is called
**Then** `ConfigManager::saveWheelMode(targetMode)` is called to persist
**And** `EncoderModeManager::setMode(targetMode)` is called to apply immediately
**And** `LOG_INFO` reports "Wheel mode set to: {mode name}"

**Given** the action needs to confirm success
**When** `getConfirmationMessage()` is called
**Then** it returns mode-specific message:
- `WheelMode::SCROLL` → "Scroll Mode Active"
- `WheelMode::VOLUME` → "Volume Mode Active"
- `WheelMode::ZOOM` → "Zoom Mode Active"

**Given** NVS save fails
**When** `ConfigManager::saveWheelMode()` returns error
**Then** mode is still applied to EncoderModeManager (runtime change works)
**And** `LOG_ERROR` reports "Failed to persist wheel mode"
**And** confirmation message still displays (user sees mode changed)

**Given** three action instances are needed for the menu
**When** MenuTree.h defines wheel behavior submenu
**Then** it creates:
- `SetWheelModeAction scrollAction(WheelMode::SCROLL, ...)`
- `SetWheelModeAction volumeAction(WheelMode::VOLUME, ...)`
- `SetWheelModeAction zoomAction(WheelMode::ZOOM, ...)`

---

### Story 3.2: Create Zoom Mode Handler

As a **user**,
I want **to use Zoom mode to send zoom in/out commands when rotating the wheel**,
So that **I can zoom in applications like browsers, image editors, and IDEs**.

**Acceptance Criteria:**

**Given** the project needs a zoom mode handler
**When** I create `src/EncoderMode/Handler/EncoderModeHandlerZoom.cpp` and `EncoderModeHandlerZoom.h`
**Then** `EncoderModeHandlerZoom` extends `EncoderModeHandlerAbstract`
**And** follows the same pattern as existing Scroll and Volume handlers
**And** uses `#pragma once` header guard

**Given** user rotates encoder clockwise in Zoom mode
**When** `handleRotation(positive delta)` is called
**Then** handler sends `Ctrl + Plus` (zoom in) via BleKeyboard
**And** command is sent `abs(delta)` times for larger rotations
**And** `LOG_DEBUG` reports "Zoom in: {delta}"

**Given** user rotates encoder counter-clockwise in Zoom mode
**When** `handleRotation(negative delta)` is called
**Then** handler sends `Ctrl + Minus` (zoom out) via BleKeyboard
**And** command is sent `abs(delta)` times for larger rotations
**And** `LOG_DEBUG` reports "Zoom out: {delta}"

**Given** the handler needs BLE keyboard access
**When** `EncoderModeHandlerZoom` is constructed
**Then** it receives `BleKeyboard*` reference (same as other handlers)

**Given** zoom commands need correct key codes
**When** sending zoom commands
**Then** uses `KEY_LEFT_CTRL` modifier with `+` or `-` keys
**And** releases modifier after key press

---

### Story 3.3: Wire Wheel Behavior Menu to Mode Manager

As a **user**,
I want **the wheel behavior menu to show all available modes and switch between them**,
So that **I can easily change how my wheel operates at runtime**.

**Acceptance Criteria:**

**Given** the menu system needs wheel behavior options
**When** MenuTree.h is updated
**Then** "Wheel Behavior" main menu item has 3 children:
- "Scroll" with `SetWheelModeAction(SCROLL)`
- "Volume" with `SetWheelModeAction(VOLUME)`
- "Zoom" with `SetWheelModeAction(ZOOM)`

**Given** EncoderModeManager needs zoom mode registration
**When** I modify `EncoderModeManager` initialization
**Then** it registers `EncoderModeHandlerZoom` alongside Scroll and Volume
**And** `setMode(WheelMode)` correctly maps enum to handler

**Given** user opens Wheel Behavior submenu
**When** menu displays the options
**Then** all three modes are visible: "Scroll", "Volume", "Zoom" (FR9)
**And** user can navigate between them with wheel rotation

**Given** user selects a wheel mode
**When** short-press on mode option
**Then** `SetWheelModeAction::execute()` runs (FR10)
**And** mode changes immediately without restart (FR11)
**And** confirmation message displays
**And** menu exits automatically

**Given** user switched to Scroll mode
**When** wheel is rotated outside menu
**Then** mouse scroll events are sent (FR12)

**Given** user switched to Volume mode
**When** wheel is rotated outside menu
**Then** volume up/down media keys are sent (FR13)

**Given** user switched to Zoom mode
**When** wheel is rotated outside menu
**Then** Ctrl+Plus/Minus zoom commands are sent (FR14)

---

## Epic 4: Button System & Configuration

**Goal:** User can view, configure, and use hardware buttons with assigned media control behaviors.

**FRs covered:** FR15, FR16, FR17, FR18, FR19, FR33, FR41

---

### Story 4.1: Create Button Manager for GPIO Input

As a **user**,
I want **the device to detect when I press hardware buttons**,
So that **I can trigger actions with physical button presses**.

**Acceptance Criteria:**

**Given** the project needs button input handling
**When** I create `src/Button/ButtonManager.cpp` and `ButtonManager.h`
**Then** `ButtonManager` class:
- Reads button configuration from `BUTTONS[]` array in button_config.h
- Sets up GPIO pins with appropriate pull-up/pull-down based on `activeLow`
- Polls button states and detects press/release transitions
**And** uses `#pragma once` header guard

**Given** buttons need GPIO initialization
**When** `ButtonManager::init()` is called during setup
**Then** each pin in `BUTTONS[]` is configured as `INPUT_PULLUP` (if activeLow) or `INPUT_PULLDOWN`
**And** initial button states are captured
**And** `LOG_INFO` reports "ButtonManager: {BUTTON_COUNT} buttons initialized"

**Given** a button is pressed
**When** `ButtonManager::update()` detects state change from released to pressed
**Then** a `BUTTON_PRESSED` event is dispatched via EncoderEventDispatcher
**And** event payload includes `buttonIndex` (0 to BUTTON_COUNT-1)
**And** debounce logic prevents duplicate events (50ms minimum between events)
**And** `LOG_DEBUG` reports "Button {index} pressed"

**Given** a button is released
**When** `ButtonManager::update()` detects state change from pressed to released
**Then** a `BUTTON_RELEASED` event is dispatched (optional, for future use)
**And** debounce timer resets

**Given** ButtonManager needs regular polling
**When** main loop runs
**Then** `ButtonManager::update()` is called each iteration
**And** polling is non-blocking (no delays)

**Given** new event types are needed
**When** I modify `include/Enum/EventEnum.h`
**Then** it includes: `BUTTON_PRESSED`, `BUTTON_RELEASED`

---

### Story 4.2: Create Set Button Action Menu Action

As a **user**,
I want **to assign a behavior to a button from the menu**,
So that **I can customize what each button does**.

**Acceptance Criteria:**

**Given** the project needs button action assignment
**When** I create `src/Menu/Action/SetButtonActionAction.cpp` and `SetButtonActionAction.h`
**Then** `SetButtonActionAction` extends `MenuAction`
**And** constructor accepts `uint8_t buttonIndex`, `ButtonAction action`, `ConfigManager*`
**And** uses `#pragma once` header guard

**Given** user assigns a behavior to a button
**When** `execute()` is called
**Then** `ConfigManager::saveButtonAction(buttonIndex, action)` is called to persist
**And** runtime button-to-action mapping is updated immediately
**And** `LOG_INFO` reports "Button {index} set to: {action name}"

**Given** the action needs to confirm success
**When** `getConfirmationMessage()` is called
**Then** it returns action-specific message:
- `ButtonAction::MUTE` → "Mute Assigned"
- `ButtonAction::PLAY` → "Play Assigned"
- `ButtonAction::PAUSE` → "Pause Assigned"
- `ButtonAction::NEXT` → "Next Assigned"
- `ButtonAction::PREVIOUS` → "Previous Assigned"
- `ButtonAction::NONE` → "Button Cleared"

**Given** NVS save fails
**When** `ConfigManager::saveButtonAction()` returns error
**Then** runtime mapping is still updated (works until reboot)
**And** `LOG_ERROR` reports "Failed to persist button action"

---

### Story 4.3: Wire Button Configuration Menu

As a **user**,
I want **the Button Config menu to show all my buttons and let me assign behaviors**,
So that **I can see and change button configurations easily**.

**Acceptance Criteria:**

**Given** the menu needs to show available buttons
**When** MenuTree.h defines "Button Config" submenu
**Then** it dynamically creates menu items for each button in `BUTTONS[]` array
**And** each button shows its label from `ButtonConfig::label` (e.g., "Button 1", "Button 2")
**And** buttons are listed in order (index 0, 1, 2, 3...)

**Given** user selects a button to configure
**When** short-press on a button menu item
**Then** submenu displays available behaviors:
- "None" (clears assignment)
- "Mute"
- "Play"
- "Pause"
- "Next"
- "Previous"
**And** user can navigate behaviors with wheel rotation (FR16)

**Given** user selects a behavior for the button
**When** short-press on behavior option
**Then** `SetButtonActionAction::execute()` runs (FR17)
**And** behavior is applied immediately (FR18)
**And** confirmation message displays
**And** menu exits automatically

**Given** user opens Button Config menu
**When** menu displays button list
**Then** all configured buttons are visible (FR15)
**And** current assignment could be shown (e.g., "Button 1 [Mute]") - optional enhancement

**Given** the menu needs action instances for each button-behavior combination
**When** MenuTree.h is constructed
**Then** it creates `SetButtonActionAction` instances for each button × behavior pair
**And** action instances are organized in behavior submenus per button

---

### Story 4.4: Implement Button HID Command Execution

As a **user**,
I want **buttons to send media commands when pressed**,
So that **I can control media playback and volume with physical buttons**.

**Acceptance Criteria:**

**Given** button actions need to send HID commands
**When** I create or modify button action handler in event system
**Then** `BUTTON_PRESSED` events are handled by looking up assigned `ButtonAction`
**And** corresponding HID command is sent via BleKeyboard

**Given** button has `ButtonAction::MUTE` assigned
**When** button is pressed
**Then** `bleKeyboard.write(KEY_MEDIA_MUTE)` is called
**And** `LOG_DEBUG` reports "Button {index}: Mute"

**Given** button has `ButtonAction::PLAY` assigned
**When** button is pressed
**Then** `bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE)` is called
**And** `LOG_DEBUG` reports "Button {index}: Play"

**Given** button has `ButtonAction::PAUSE` assigned
**When** button is pressed
**Then** `bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE)` is called (same as Play)
**And** `LOG_DEBUG` reports "Button {index}: Pause"

**Given** button has `ButtonAction::NEXT` assigned
**When** button is pressed
**Then** `bleKeyboard.write(KEY_MEDIA_NEXT_TRACK)` is called
**And** `LOG_DEBUG` reports "Button {index}: Next"

**Given** button has `ButtonAction::PREVIOUS` assigned
**When** button is pressed
**Then** `bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK)` is called
**And** `LOG_DEBUG` reports "Button {index}: Previous"

**Given** button has `ButtonAction::NONE` assigned
**When** button is pressed
**Then** no HID command is sent
**And** `LOG_DEBUG` reports "Button {index}: No action"

**Given** BLE is not connected
**When** button is pressed with assigned action
**Then** no HID command is sent (BleKeyboard handles gracefully)
**And** no error is thrown

---

## Epic 5: Device Status & About Screens

**Goal:** User can view device status for troubleshooting and device information.

**FRs covered:** FR20, FR21, FR22, FR23, FR24, FR25

---

### Story 5.1: Create Show Status Action

As a **user**,
I want **to view current device configuration and connection status**,
So that **I can troubleshoot issues and verify my settings are correct**.

**Acceptance Criteria:**

**Given** the project needs a status display action
**When** I create `src/Menu/Action/ShowStatusAction.cpp` and `ShowStatusAction.h`
**Then** `ShowStatusAction` extends `MenuAction`
**And** constructor accepts `ConfigManager*`, `EncoderModeManager*`, `BleKeyboard*`
**And** uses `#pragma once` header guard

**Given** user selects "Device Status" from menu
**When** `execute()` is called
**Then** display shows multiple status lines via `DisplayInterface::showStatus()`

**Given** status needs to show current wheel mode (FR20)
**When** status is displayed
**Then** shows "Wheel Mode: {current mode name}" (Scroll/Volume/Zoom)
**And** value is read from `EncoderModeManager::getCurrentMode()`

**Given** status needs to show button assignments (FR21)
**When** status is displayed
**Then** shows each button's assignment:
- "Button 1: {action}" (e.g., "Mute", "Play", "None")
- "Button 2: {action}"
- (continues for all BUTTON_COUNT buttons)
**And** values are read from `ConfigManager::loadButtonAction()`

**Given** status needs to show BLE connection status (FR22)
**When** status is displayed
**Then** shows "BLE: Connected" or "BLE: Disconnected"
**And** value is read from `BleKeyboard::isConnected()`

**Given** status needs to show BLE signal strength (FR23)
**When** status is displayed and BLE is connected
**Then** shows "Signal: {RSSI} dBm" if RSSI is available
**And** shows "Signal: N/A" if RSSI not available from BLE stack
**And** `LOG_DEBUG` reports status values displayed

**Given** status display should not exit menu automatically
**When** `execute()` completes
**Then** `getConfirmationMessage()` returns `nullptr` (no auto-exit)
**And** user must long-press to return to menu
**And** status remains visible until user navigates away

---

### Story 5.2: Create Show About Action

As a **user**,
I want **to view device information and firmware version**,
So that **I know what version I'm running and can reference it for support**.

**Acceptance Criteria:**

**Given** the project needs an about display action
**When** I create `src/Menu/Action/ShowAboutAction.cpp` and `ShowAboutAction.h`
**Then** `ShowAboutAction` extends `MenuAction`
**And** uses `#pragma once` header guard

**Given** user selects "About" from menu
**When** `execute()` is called
**Then** display shows device information via `DisplayInterface::showStatus()`

**Given** about screen needs device name and version (FR24)
**When** about is displayed
**Then** shows "Device: KnobKoKy" (from device_config.h `BLE_DEVICE_NAME`)
**And** shows "Version: {version}" (from version.h `FIRMWARE_VERSION`)
**And** version format follows semver (e.g., "1.0.0")

**Given** about screen needs manufacturer info (FR25)
**When** about is displayed
**Then** shows "By: {manufacturer}" (from device_config.h `BLE_MANUFACTURER`)
**And** optionally shows build date if available

**Given** about display should not exit menu automatically
**When** `execute()` completes
**Then** `getConfirmationMessage()` returns `nullptr` (no auto-exit)
**And** user must long-press to return to menu

**Given** version info needs to be centralized
**When** I verify `include/version.h` exists
**Then** it defines:
- `#define FIRMWARE_VERSION "1.0.0"`
- `#define FIRMWARE_BUILD_DATE __DATE__` (optional)

## Epic List

### Epic 1: Display & Persistence Foundation
User can see device feedback via serial output and their settings are remembered across power cycles.

**FRs covered:** FR26, FR27, FR28, FR29, FR35, FR36, FR37

**What this delivers:**
- Serial display output for all menu and status messages
- Abstract display interface (ready for future OLED)
- NVS-based configuration persistence
- Automatic config restore on boot with sensible defaults
- Factory reset capability

---

### Epic 2: Menu Navigation System
User can enter, navigate, select items, and exit the hierarchical menu using the encoder's two-input model (wheel rotation + button press).

**FRs covered:** FR1, FR2, FR3, FR4, FR5, FR6, FR7, FR8

**What this delivers:**
- Long-press to enter/exit menu
- Wheel rotation to browse options
- Short-press to select
- Hierarchical navigation with back capability
- Confirmation feedback on selection
- Auto-exit after configuration changes

---

### Epic 3: Wheel Behavior Configuration
User can view available wheel modes and switch between Scroll, Volume, and Zoom at runtime through the menu.

**FRs covered:** FR9, FR10, FR11, FR12, FR13, FR14

**What this delivers:**
- Wheel Behavior submenu with mode options
- Mode selection and immediate application
- Scroll mode (vertical mouse scroll)
- Volume mode (media volume up/down)
- Zoom mode (zoom in/out)
- Persisted mode selection

---

### Epic 4: Button System & Configuration
User can view, configure, and use hardware buttons with assigned media control behaviors.

**FRs covered:** FR15, FR16, FR17, FR18, FR19, FR33, FR41

**What this delivers:**
- Button GPIO setup and event detection
- Button Configurator submenu showing all buttons
- Behavior assignment (Mute, Play, Pause, Next, Previous)
- Immediate application of button behaviors
- HID command sending when buttons pressed
- Persisted button assignments

---

### Epic 5: Device Status & About Screens
User can view device status for troubleshooting and device information.

**FRs covered:** FR20, FR21, FR22, FR23, FR24, FR25

**What this delivers:**
- Device Status submenu showing:
  - Current wheel mode
  - Current button assignments
  - BLE connection status
  - BLE connection strength (when available)
- About screen showing:
  - Device name and firmware version
  - Manufacturer information

---

### Existing Functionality (No New Implementation Required)

The following FRs are already implemented in the existing codebase and will be integrated with the new menu system:

| FR | Description |
|----|-------------|
| FR30 | BLE HID advertising on boot |
| FR31 | Auto-reconnect to last paired device |
| FR32 | Standard BLE pairing flow |
| FR34 | Mouse scroll events for wheel |
| FR38 | Encoder rotation detection |
| FR39 | Short press detection |
| FR40 | Long press detection |
| FR42 | Event queue processing |
