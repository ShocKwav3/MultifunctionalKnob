# Story 4.2: Create Set Button Action Menu Action

Status: ready-for-dev

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

- [ ] Create `src/Menu/Action/SetButtonActionAction.h`
- [ ] Create `src/Menu/Action/SetButtonActionAction.cpp`

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
