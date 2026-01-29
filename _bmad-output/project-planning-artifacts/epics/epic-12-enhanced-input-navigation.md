---
epicNumber: 12
title: "Enhanced Input & Navigation"
complexity: Low
dependencies: "None (uses existing menu, encoder driver, button event handler)"
frsCovered: "FR-20, FR-21"
nfrsCovered: "NFR-4"
stories: 2
sourceDocuments:
  - "Requirements gathered in conversation with user"
---

# Epic 12: Enhanced Input & Navigation

**Goal:** Improve device usability by adding intuitive navigation shortcuts and utilizing unused button inputs for quick mode switching.

**User Outcome:** Users can quickly exit menus with a double-press gesture and cycle through wheel modes using the macro button, making the device faster and more convenient to use during daily workflows.

**Story Status:**
- Story 12.1: 📋 READY-FOR-DEV (double-press exit)
- Story 12.2: 📋 READY-FOR-DEV (macro button mode cycling)

---

## Functional Requirements

- FR-20: Double-press encoder button exits menu from any level, returning to home screen
- FR-21: Short-press macro button cycles through available wheel modes (Scroll → Volume → Zoom)

## Non-Functional Requirements

- NFR-4: No additional latency on normal input processing; gestures handled by existing encoder driver

---

## Story 12.1: Double-Press Encoder Exit

As a **user navigating through menus**,
I want **to quickly exit by double-pressing the encoder button**,
So that **I can return to normal operation without multiple long-presses**.

**Scope:**
- `lib/EncoderDriver/` - Encoder driver already detects double-press events
- `src/Menu/Controller/MenuController.cpp/.h` - Add double-press exit handling
- `src/Event/Handler/EncoderEventHandler.cpp` - Route double-press to menu when active

**Acceptance Criteria:**

**Given** I am in any menu level (top-level, submenu, or deep nested menu)
**When** I double-press the encoder button
**Then** the menu system exits completely
**And** the display returns to the home/idle screen
**And** menu state is cleared

**Given** I am at the home screen (menu not active)
**When** I double-press the encoder button
**Then** no action is taken (double-press only works in menu context)

**Given** the encoder driver detects a double-press
**When** the event is processed by EncoderEventHandler
**Then** if MenuController is active, it receives the double-press event
**And** MenuController.exit() or equivalent method is called
**And** menu closes gracefully

**Given** I double-press to exit a menu
**When** the exit completes
**Then** encoder rotation and button press return to normal mode behavior
**And** no menu artifacts remain on display

**Technical Notes:**
- EncoderDriver likely already emits double-press events (verify in event dispatcher)
- Add `handleDoublePress()` method to MenuController
- Double-press timing configured in encoder driver (no changes needed)
- Priority: Menu handles double-press before normal mode handlers

---

## Story 12.2: Macro Button Mode Cycling

As a **user wanting to switch wheel modes quickly**,
I want **to short-press the macro button to cycle through modes**,
So that **I can switch from Scroll to Volume to Zoom without opening the menu**.

**Scope:**
- `src/Event/Handler/ButtonEventHandler.cpp` - Detect short-press on macro button (GPIO 10)
- `src/EncoderMode/Manager/EncoderModeManager.cpp/.h` - Add cycle method
- `src/Config/ConfigManager.cpp/.h` - Save mode change to NVS
- `include/Enum/EventEnum.h` - Ensure short-press event type exists

**Acceptance Criteria:**

**Given** I am using the device in any wheel mode
**When** I short-press the macro button (GPIO 10)
**Then** the wheel mode cycles to the next mode in sequence
**And** the display updates to show the new mode name
**And** the new mode is saved to NVS immediately

**Given** the current mode is Scroll
**When** I short-press the macro button
**Then** the mode changes to Volume

**Given** the current mode is Volume
**When** I short-press the macro button
**Then** the mode changes to Zoom

**Given** the current mode is Zoom
**When** I short-press the macro button
**Then** the mode wraps back to Scroll

**Given** I cycle to a new mode
**When** I use the encoder (rotate or press)
**Then** the new mode handler processes the input correctly
**And** behavior matches the newly selected mode

**Given** I cycle to a new mode and reboot the device
**When** the device starts up
**Then** the last selected mode is loaded from NVS
**And** the device starts in that mode

**Given** the menu is active
**When** I short-press the macro button
**Then** the menu takes priority (no mode cycling during menu navigation)

**Technical Implementation Notes:**
- Macro button GPIO 10 already defined in `button_config.h`
- ButtonEventHandler already processes button events
- Add `EncoderModeManager::cycleToNextMode()` method
- Cycle order: SCROLL → VOLUME → ZOOM → SCROLL (use modulo arithmetic on enum)
- After cycle, call `ConfigManager::saveWheelMode()` for persistence
- Dispatch `AppEvent` to update display with new mode name
- Priority order: Menu > Mode cycling > Normal input handling

---

## Implementation Checklist

### Story 12.1: Double-Press Exit
- [ ] Verify EncoderDriver emits double-press events (check event dispatcher)
- [ ] Add `handleDoublePress()` method to MenuController
- [ ] Update EncoderEventHandler to route double-press to menu when active
- [ ] Implement menu exit logic (clear state, return to home screen)
- [ ] Test double-press from top-level menu
- [ ] Test double-press from nested submenus
- [ ] Test double-press when menu is not active (should do nothing)
- [ ] Verify display clears menu artifacts after exit
- [ ] Verify encoder inputs return to normal mode behavior after exit

### Story 12.2: Macro Button Mode Cycling
- [ ] Verify macro button GPIO 10 is configured in ButtonEventHandler
- [ ] Implement `EncoderModeManager::cycleToNextMode()` method
- [ ] Add short-press detection for macro button in ButtonEventHandler
- [ ] Implement cycle logic with modulo arithmetic (3 modes)
- [ ] Dispatch AppEvent to update display with new mode name
- [ ] Call `ConfigManager::saveWheelMode()` after each cycle
- [ ] Test cycle sequence: Scroll → Volume → Zoom → Scroll
- [ ] Test NVS persistence across reboot
- [ ] Verify menu priority (no cycling during menu navigation)
- [ ] Test each mode's behavior after cycling to it

---

## Technical Notes

**Existing Infrastructure:**
- EncoderDriver handles button press detection (single, long, double)
- EncoderModeManager tracks active mode and routes to handlers
- ButtonEventHandler processes GPIO button events
- ConfigManager provides NVS persistence for wheel mode
- AppEventDispatcher can trigger display updates

**GPIO Assignments:**
- Encoder button: GPIO 2
- Macro button: GPIO 10

**Mode Enum Values:**
- Verify `WheelModeEnum.h` for mode ordering (Scroll, Volume, Zoom)

**Event Priority Order:**
1. Menu active → MenuController handles all inputs
2. Macro button short-press → Mode cycling
3. Normal mode handlers

**Display Updates:**
- Use existing AppEvent mechanism to show mode name on OLED
- Display format: "Mode: [Scroll/Volume/Zoom]" (2-second notification or persistent indicator)

---

## Related Files

**Story 12.1:**
- `lib/EncoderDriver/EncoderDriver.cpp/.h` (verify double-press events)
- `src/Event/Dispatcher/EncoderEventDispatcher.cpp` (verify event emission)
- `src/Event/Handler/EncoderEventHandler.cpp` (route to menu)
- `src/Menu/Controller/MenuController.cpp/.h` (add exit method)

**Story 12.2:**
- `include/Config/button_config.h` (macro button GPIO)
- `include/Enum/WheelModeEnum.h` (mode enum values)
- `src/Event/Handler/ButtonEventHandler.cpp` (short-press detection)
- `src/EncoderMode/Manager/EncoderModeManager.cpp/.h` (cycle method)
- `src/Config/ConfigManager.cpp/.h` (save mode)
- `src/Event/Dispatcher/AppEventDispatcher.cpp` (display update)

---

## Embedded Systems Considerations

**Memory Impact:**
- No dynamic allocation required
- Reuses existing event handling infrastructure
- Minimal code size increase (~100-200 bytes per story)

**Performance:**
- No additional latency on input path
- Mode cycling is instantaneous (enum update + NVS write)
- Double-press detection handled by encoder driver ISR

**Power Consumption:**
- No impact (no new peripherals or timers)
- NVS writes are infrequent (only on user action)

---
