# Epic 5: Device Status & About Screens

**Goal:** User can view device status for troubleshooting and device information.

**FRs covered:** FR20, FR21, FR22, FR23, FR24, FR25

---

## Story 5.1: Create Show Status Action

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
**Then** display shows multiple status lines via `SystemEventDispatcher`

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

## Story 5.2: Create Show About Action

As a **user**,
I want **to view device information and firmware version**,
So that **I can customize what each button does**.

**Acceptance Criteria:**

**Given** the project needs an about display action
**When** I create `src/Menu/Action/ShowAboutAction.cpp` and `ShowAboutAction.h`
**Then** `ShowAboutAction` extends `MenuAction`
**And** uses `#pragma once` header guard

**Given** user selects "About" from menu
**When** `execute()` is called
**Then** display shows device information via `SystemEventDispatcher`

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
