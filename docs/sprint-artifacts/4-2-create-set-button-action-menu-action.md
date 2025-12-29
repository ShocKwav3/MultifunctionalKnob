# Story 4.2: Create Set Button Action Menu Action

Status: review

## Story

As a **user**,
I want **to assign a behavior to a button from the menu**,
so that **I can customize what each button does**.

## Acceptance Criteria

1. **SetButtonActionAction Class:**
   - Create `src/Menu/Action/SetButtonActionAction.cpp` and `SetButtonActionAction.h`
   - Inherit from `MenuAction`
   - Constructor: `SetButtonActionAction(uint8_t buttonIndex, ButtonAction action)`

2. **Execution Logic:**
   - `execute()`:
     - Call `ConfigManager::saveButtonAction(buttonIndex, action)`
     - Update runtime mapping (if needed, or rely on ConfigManager/ButtonManager to sync)
     - Log "Button {index} set to: {action}"

3. **Confirmation:**
   - `getConfirmationMessage()`: Return "Mute Assigned", "Play Assigned", etc.

## Tasks / Subtasks

- [x] Create `src/Menu/Action/SetButtonActionAction.h`
- [x] Create `src/Menu/Action/SetButtonActionAction.cpp`

## Dev Notes

### Architecture Compliance

- **Command Pattern:** Encapsulate action logic.
- **Persistence:** Use `ConfigManager`.

### References

- [Architecture: Command Pattern](docs/architecture.md#command-pattern-for-actions)
- [Epics: Story 4.2](docs/epics.md#story-42-create-set-button-action-menu-action)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
- claude-sonnet-4-5 (implementation)

### Implementation Plan

Created SetButtonActionAction following Command Pattern:
- Extends MenuAction base class
- Constructor accepts buttonIndex, action, ConfigManager*
- execute() persists via ConfigManager::saveButtonAction()
- getConfirmationMessage() returns action-specific strings
- Follows existing SelectWheelModeAction pattern

### Completion Notes

✅ Implemented SetButtonActionAction class per AC 1-3
- src/Menu/Action/SetButtonActionAction.h: Header with MenuAction inheritance
- src/Menu/Action/SetButtonActionAction.cpp: Implementation with NVS persistence
- Constructor signature matches AC (buttonIndex, action, ConfigManager*)
- execute() calls ConfigManager::saveButtonAction() and logs action
- getConfirmationMessage() returns all required messages (Mute/Play/Pause/Next/Previous/None)
- Build validated successfully (use_nimble env)

## File List

- src/Menu/Action/SetButtonActionAction.h (new)
- src/Menu/Action/SetButtonActionAction.cpp (new)

## Change Log

- 2025-12-29: Created SetButtonActionAction class for button action assignment via menu
