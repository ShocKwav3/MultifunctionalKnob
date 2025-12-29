# Epic 4: Button System & Configuration

**Goal:** User can view, configure, and use hardware buttons with assigned media control behaviors.

**FRs covered:** FR15, FR16, FR17, FR18, FR19, FR33, FR41

---

## Story 4.1: Create Button Manager for GPIO Input

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
**When** `ButtonManager::poll()` detects state change from released to pressed
**Then** a `BUTTON_PRESSED` event is dispatched via ButtonEventDispatcher
**And** event payload includes `buttonIndex` (0 to BUTTON_COUNT-1)
**And** debounce logic prevents duplicate events (DEBOUNCE_MS from button_config.h)
**And** `LOG_DEBUG` reports "Button {index} pressed"

**Given** a button is released
**When** `ButtonManager::poll()` detects state change from pressed to released
**Then** a `BUTTON_RELEASED` event is dispatched (optional, for future use)
**And** debounce timer resets

**Given** ButtonManager needs regular polling
**When** main loop runs
**Then** `ButtonManager::poll()` is called each iteration
**And** polling is non-blocking (no delays)

**Given** new event types are needed
**When** I modify `include/Enum/EventEnum.h`
**Then** it includes: `BUTTON_PRESSED`, `BUTTON_RELEASED`

---

## Story 4.2: Create Set Button Action Menu Action

As a **user**,
I want **to assign a behavior to a button from the menu**,
So that **I can customize what each button does**.

**Acceptance Criteria:**

**Given** the project needs button action assignment
**When** I create `src/Menu/Action/SetButtonBehaviorAction.cpp` and `SetButtonBehaviorAction.h`
**Then** `SetButtonBehaviorAction` extends `MenuAction`
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

## Story 4.3: Wire Button Configuration Menu

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
**Then** `SetButtonBehaviorAction::execute()` runs (FR17)
**And** behavior is applied immediately (FR18)
**And** confirmation message displays
**And** menu exits automatically

**Given** user opens Button Config menu
**When** menu displays button list
**Then** all configured buttons are visible (FR15)
**And** current assignment could be shown (e.g., "Button 1 [Mute]") - optional enhancement

**Given** the menu needs action instances for each button-behavior combination
**When** MenuTree.h is constructed
**Then** it creates `SetButtonBehaviorAction` instances for each button × behavior pair
**And** action instances are organized in behavior submenus per button

---

## Story 4.4: Implement Button HID Command Execution

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
