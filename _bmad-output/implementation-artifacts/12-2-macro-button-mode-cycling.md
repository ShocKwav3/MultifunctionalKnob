# Story 12.2: Macro Button Mode Cycling

**Epic:** 12 - Enhanced Input & Navigation
**Status:** ready-for-dev
**Complexity:** Low
**Created:** 2026-01-29

---

## User Story

As a **user wanting to switch wheel modes quickly**,
I want **to short-press the macro button to cycle through modes**,
So that **I can switch from Scroll to Volume to Zoom without opening the menu**.

---

## Scope

- `src/Event/Handler/ButtonEventHandler.cpp` - Detect short-press on macro button (GPIO 10)
- `src/EncoderMode/Manager/EncoderModeManager.cpp/.h` - Add cycle method
- `src/Config/ConfigManager.cpp/.h` - Save mode change to NVS
- `include/Enum/EventEnum.h` - Ensure short-press event type exists

---

## Current Behavior

Short-pressing the macro button (GPIO 10) does nothing. The button is defined in `button_config.h` but has no short-press functionality.

---

## Desired Behavior

Short-press the macro button to cycle through wheel modes: Scroll → Volume → Zoom → Scroll (repeat). The display shows the new mode briefly, and the mode is saved to NVS.

---

## Acceptance Criteria

### AC1: Basic Mode Cycling
**Given** I am using the device in any wheel mode
**When** I short-press the macro button (GPIO 10)
**Then** the wheel mode cycles to the next mode in sequence
**And** the display updates to show the new mode name
**And** the new mode is saved to NVS immediately

### AC2: Scroll to Volume
**Given** the current mode is Scroll
**When** I short-press the macro button
**Then** the mode changes to Volume

### AC3: Volume to Zoom
**Given** the current mode is Volume
**When** I short-press the macro button
**Then** the mode changes to Zoom

### AC4: Zoom to Scroll (Wrap Around)
**Given** the current mode is Zoom
**When** I short-press the macro button
**Then** the mode wraps back to Scroll

### AC5: Mode Handler Behavior
**Given** I cycle to a new mode
**When** I use the encoder (rotate or press)
**Then** the new mode handler processes the input correctly
**And** behavior matches the newly selected mode

### AC6: Persistence
**Given** I cycle to a new mode and reboot the device
**When** the device starts up
**Then** the last selected mode is loaded from NVS
**And** the device starts in that mode

### AC7: Menu Priority
**Given** the menu is active
**When** I short-press the macro button
**Then** the menu takes priority (no mode cycling during menu navigation)

---

## Technical Implementation Notes

### Button Configuration
- Macro button GPIO 10 already defined in `include/Config/button_config.h`
- ButtonEventHandler already processes button events for this GPIO
- Verify short-press event type exists in ButtonEventHandler

### Mode Manager
- Add `EncoderModeManager::cycleToNextMode()` method
- Cycle logic: Use modulo arithmetic on mode enum
  ```cpp
  currentMode = (currentMode + 1) % 3; // 3 modes: Scroll, Volume, Zoom
  ```
- Get mode order from `WheelModeEnum.h`
- Call existing `setMode()` or equivalent to activate new mode

### Persistence
- Use existing `ConfigManager::saveWheelMode()` method
- Call after mode change to persist immediately
- NVS key likely: `"wheel_mode"` or similar (check existing code)

### Display Update
- Dispatch `AppEvent` to update display
- Show mode name: "Mode: Scroll", "Mode: Volume", "Mode: Zoom"
- Display for 2-3 seconds as notification
- Use existing AppEventDispatcher mechanism

### Event Priority
1. **Menu active** → MenuController handles (ignore mode cycling)
2. **Macro button short-press** → Mode cycling
3. **Normal handlers** → Default behavior

---

## Implementation Checklist

- [ ] Verify macro button GPIO 10 configured in ButtonEventHandler
  - Check `button_config.h` for GPIO definition
  - Verify ButtonEventHandler processes GPIO 10
- [ ] Implement `EncoderModeManager::cycleToNextMode()` method
  - Get current mode
  - Calculate next mode with modulo arithmetic
  - Set new mode using existing mode change logic
- [ ] Add short-press detection for macro button in ButtonEventHandler
  - Check for short-press event type
  - Call `encoderModeManager.cycleToNextMode()` on short-press
  - Respect menu priority (check if menu active first)
- [ ] Implement cycle logic with proper enum handling
  - Verify mode order in `WheelModeEnum.h`
  - Ensure all 3 modes cycle correctly (Scroll → Volume → Zoom → Scroll)
- [ ] Dispatch AppEvent for display update
  - Create event with new mode name
  - Send to AppEventDispatcher
  - Display should show "Mode: [name]" notification
- [ ] Call `ConfigManager::saveWheelMode()` after cycle
  - Save new mode to NVS immediately
  - Handle any error returns gracefully
- [ ] Test cycle sequence: Scroll → Volume → Zoom → Scroll
  - Verify each transition
  - Verify display updates correctly
  - Verify mode behavior changes correctly
- [ ] Test NVS persistence across reboot
  - Cycle to Volume
  - Reboot device
  - Verify starts in Volume mode
- [ ] Test menu priority
  - Open menu
  - Short-press macro button
  - Verify menu still active (no mode change)
- [ ] Test each mode's behavior after cycling
  - Rotate encoder in each mode
  - Verify Scroll sends scroll keys
  - Verify Volume sends volume keys
  - Verify Zoom sends zoom keys

---

## Files to Modify

- `include/Config/button_config.h` (verify GPIO 10 definition)
- `include/Enum/WheelModeEnum.h` (verify mode enum order)
- `src/Event/Handler/ButtonEventHandler.cpp` (add short-press handling)
- `src/EncoderMode/Manager/EncoderModeManager.cpp/.h` (add cycleToNextMode)
- `src/Config/ConfigManager.cpp/.h` (verify saveWheelMode exists)
- `src/Event/Dispatcher/AppEventDispatcher.cpp` (dispatch display update)

---

## Testing Notes

**Manual Testing:**
1. Start in Scroll mode → short-press macro button → verify Volume mode
2. In Volume mode → short-press → verify Zoom mode
3. In Zoom mode → short-press → verify Scroll mode (wrap)
4. Cycle to Volume → rotate encoder → verify volume up/down keys sent
5. Cycle to Zoom → rotate encoder → verify zoom in/out keys sent
6. Cycle to new mode → reboot → verify mode persists
7. Open menu → short-press macro button → verify menu unaffected
8. Check display → verify mode name shows briefly after cycle

**Edge Cases:**
- Rapid multiple short-presses (mode should cycle quickly)
- Short-press during mode transition (should handle gracefully)
- Short-press while BLE disconnected (mode should still cycle)

---

## Definition of Done

- [ ] All acceptance criteria passing
- [ ] All checklist items completed
- [ ] Manual testing completed successfully
- [ ] NVS persistence verified across reboot
- [ ] No new compiler warnings
- [ ] Code follows project coding standards
- [ ] Ready for code review

---

## Related Epic

See `_bmad-output/project-planning-artifacts/epics/epic-12-enhanced-input-navigation.md` for full epic context.

---
