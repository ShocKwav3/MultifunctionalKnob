# Story 4.2: Create Set Button Action Menu Action

Status: done

## Story

As a **user**,
I want **to assign a behavior to a button from the menu**,
so that **I can customize what each button does**.

## Acceptance Criteria

1. **SetButtonBehaviorAction Class:**
   - Create `src/Menu/Action/SetButtonBehaviorAction.cpp` and `SetButtonBehaviorAction.h`
   - Inherit from `MenuAction`
   - Constructor: `SetButtonBehaviorAction(uint8_t buttonIndex, ButtonAction action, ConfigManager* config)`

2. **Execution Logic:**
   - `execute()`:
     - Call `ConfigManager::saveButtonAction(buttonIndex, action)`
     - Update runtime mapping (if needed, or rely on ConfigManager/ButtonManager to sync)
     - Log "Button {index} set to: {action}"

3. **Confirmation:**
   - `getConfirmationMessage()`: Return "Mute Assigned", "Play Assigned", etc.

## Tasks / Subtasks

- [x] Create `src/Menu/Action/SetButtonBehaviorAction.h`
- [x] Create `src/Menu/Action/SetButtonBehaviorAction.cpp`
- [x] Verify build success (Tests deferred per Strategic Pivot)

## Dev Notes

### Architecture Compliance

- **Command Pattern:** Encapsulate action logic.
- **Persistence:** Use `ConfigManager`.
- **Testability:** Injected `ConfigManager` dependency ensures future testability (tests deferred per Strategic Pivot).

### References

- [Architecture: Command Pattern](_bmad-output/architecture/index.md#command-pattern-for-actions)
- [Epics: Story 4.2](_bmad-output/epics/index.md#story-42-create-set-button-action-menu-action)
- [Strategic Pivot](_bmad-output/epics/strategic-pivot-speed-to-market-2025-12-18.md)

## Dev Agent Record

### Context Reference

- `_bmad-output/architecture/index.md`
- `_bmad-output/epics/index.md`

### Agent Model Used

- google/gemini-3-pro-preview
- claude-sonnet-4-5 (implementation)

### Implementation Plan

Created SetButtonBehaviorAction following Command Pattern:
- Extends MenuAction base class
- Constructor accepts buttonIndex, action, ConfigManager*
- execute() persists via ConfigManager::saveButtonAction()
- getConfirmationMessage() returns action-specific strings
- Follows existing SelectWheelModeAction pattern

### Completion Notes

✅ Implemented SetButtonBehaviorAction class per AC 1-3
- src/Menu/Action/SetButtonBehaviorAction.h: Header with MenuAction inheritance
- src/Menu/Action/SetButtonBehaviorAction.cpp: Implementation with NVS persistence
- Constructor signature matches AC (buttonIndex, action, ConfigManager*)
- execute() calls ConfigManager::saveButtonAction() and logs action
- getConfirmationMessage() returns all required messages (Mute/Play/Pause/Next/Previous/None)
- Build validated successfully (use_nimble env)

## File List

- src/Menu/Action/SetButtonBehaviorAction.h (new)
- src/Menu/Action/SetButtonBehaviorAction.cpp (new)

## Change Log

- 2025-12-29: Created SetButtonBehaviorAction class for button action assignment via menu
- 2025-12-29: Renamed from SetButtonActionAction to SetButtonBehaviorAction (removed redundant "Action")
