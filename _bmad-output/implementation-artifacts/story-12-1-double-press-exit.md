# Story 12.1: Double-Press Encoder Exit

**Epic:** 12 - Enhanced Input & Navigation
**Status:** ready-for-dev
**Complexity:** Low
**Created:** 2026-01-29

---

## User Story

As a **user navigating through menus**,
I want **to quickly exit by double-pressing the encoder button**,
So that **I can return to normal operation without multiple long-presses**.

---

## Scope

- `lib/EncoderDriver/` - Encoder driver already detects double-press events
- `src/Menu/Controller/MenuController.cpp/.h` - Add double-press exit handling
- `src/Event/Handler/EncoderEventHandler.cpp` - Route double-press to menu when active

---

## Current Behavior

Users exit menus by performing "few long-presses" (unclear mechanism). This is cumbersome and not intuitive.

---

## Desired Behavior

Double-press the encoder button to instantly exit from any menu level, returning to the home/idle screen.

---

## Acceptance Criteria

### AC1: Exit from Any Menu Level
**Given** I am in any menu level (top-level, submenu, or deep nested menu)
**When** I double-press the encoder button
**Then** the menu system exits completely
**And** the display returns to the home/idle screen
**And** menu state is cleared

### AC2: No Action When Menu Not Active
**Given** I am at the home screen (menu not active)
**When** I double-press the encoder button
**Then** no action is taken (double-press only works in menu context)

### AC3: Event Routing
**Given** the encoder driver detects a double-press
**When** the event is processed by EncoderEventHandler
**Then** if MenuController is active, it receives the double-press event
**And** MenuController.exit() or equivalent method is called
**And** menu closes gracefully

### AC4: Clean Exit
**Given** I double-press to exit a menu
**When** the exit completes
**Then** encoder rotation and button press return to normal mode behavior
**And** no menu artifacts remain on display

---

## Technical Implementation Notes

### Event Detection
- **Verify:** EncoderDriver likely already emits double-press events
- Check `EncoderEventDispatcher` to confirm double-press event type exists
- No timing changes needed (encoder driver handles double-press detection)

### Menu Controller
- Add `handleDoublePress()` method to `MenuController`
- Method should call existing exit logic (clear state, return to home)
- Ensure menu stack is properly cleaned up

### Event Handler Priority
- EncoderEventHandler should check if menu is active
- If active, route double-press to MenuController
- If not active, ignore double-press (no handler)
- Priority: Menu > Normal mode handlers

### Display Update
- Ensure display clears menu content
- Return to home/idle screen display
- No transition animation needed (instant exit)

---

## Implementation Checklist

- [ ] Verify EncoderDriver emits double-press events
  - Check `EncoderEventDispatcher.cpp` for event emission
  - Confirm event type in `EventEnum.h`
- [ ] Add `handleDoublePress()` method to MenuController
  - Implement exit logic (clear menu state)
  - Return to home screen
- [ ] Update EncoderEventHandler
  - Check if MenuController is active
  - Route double-press event to menu when active
  - Ignore when menu not active
- [ ] Test: Double-press from top-level menu
  - Verify exits to home screen
  - Verify menu state cleared
- [ ] Test: Double-press from nested submenu
  - Verify exits completely (not just one level)
  - Verify returns to home screen
- [ ] Test: Double-press when menu not active
  - Verify no action taken
  - Verify no errors or side effects
- [ ] Verify display cleanup
  - No menu artifacts remain
  - Home screen displays correctly
- [ ] Verify encoder behavior after exit
  - Rotation works in normal mode
  - Button press works in normal mode

---

## Files to Modify

- `lib/EncoderDriver/EncoderDriver.cpp/.h` (verify only)
- `src/Event/Dispatcher/EncoderEventDispatcher.cpp` (verify event emission)
- `src/Event/Handler/EncoderEventHandler.cpp` (add routing logic)
- `src/Menu/Controller/MenuController.cpp/.h` (add handleDoublePress method)
- `include/Enum/EventEnum.h` (verify event type exists)

---

## Testing Notes

**Manual Testing:**
1. Navigate to Settings menu → double-press → verify exit
2. Navigate to Settings → Wheel Mode → double-press → verify exit
3. Navigate deep into menu tree → double-press → verify complete exit
4. At home screen → double-press → verify no action
5. After exit → rotate encoder → verify scroll mode works
6. After exit → press encoder → verify normal behavior

**Edge Cases:**
- Double-press during menu transition (should complete transition first)
- Rapid multiple double-presses (should handle gracefully)

---

## Definition of Done

- [ ] All acceptance criteria passing
- [ ] All checklist items completed
- [ ] Manual testing completed successfully
- [ ] No new compiler warnings
- [ ] Code follows project coding standards
- [ ] Ready for code review

---

## Related Epic

See `_bmad-output/project-planning-artifacts/epics/epic-12-enhanced-input-navigation.md` for full epic context.

---
