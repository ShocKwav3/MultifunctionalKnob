# Epic 3: Wheel Behavior Configuration

**Goal:** User can view available wheel modes and switch between Scroll, Volume, and Zoom at runtime through the menu.

**FRs covered:** FR9, FR10, FR11, FR12, FR13, FR14

---

## Story 3.1: Create Set Wheel Mode Action

As a **user**,
I want **to select a wheel mode from the menu and have it applied immediately**,
So that **my wheel behavior changes without needing to restart the device**.

**Acceptance Criteria:**

**Given** the project needs wheel mode actions
**When** I create `src/Menu/Action/SelectWheelModeAction.cpp` and `SelectWheelModeAction.h`
**Then** `SelectWheelModeAction` extends `MenuAction`
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
- `SelectWheelModeAction scrollAction(WheelMode::SCROLL, ...)`
- `SelectWheelModeAction volumeAction(WheelMode::VOLUME, ...)`
- `SelectWheelModeAction zoomAction(WheelMode::ZOOM, ...)`

---

## Story 3.2: Create Zoom Mode Handler

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

## Story 3.3: Wire Wheel Behavior Menu to Mode Manager

As a **user**,
I want **the wheel behavior menu to show all available modes and switch between them**,
So that **I can easily change how my wheel operates at runtime**.

**Acceptance Criteria:**

**Given** the menu system needs wheel behavior options
**When** MenuTree.h is updated
**Then** "Wheel Behavior" main menu item has 3 children:
- "Scroll" with `SelectWheelModeAction(SCROLL)`
- "Volume" with `SelectWheelModeAction(VOLUME)`
- "Zoom" with `SelectWheelModeAction(ZOOM)`

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
**Then** `SelectWheelModeAction::execute()` runs (FR10)
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
