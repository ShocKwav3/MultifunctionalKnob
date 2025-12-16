# Story 3.1: Create Set Wheel Mode Action

Status: ready-for-dev

## Story

As a **user**,
I want **to select a wheel mode from the menu and have it applied immediately**,
so that **my wheel behavior changes without needing to restart the device**.

## Acceptance Criteria

1. **SetWheelModeAction Class:**
   - Create `src/Menu/Action/SetWheelModeAction.cpp` and `SetWheelModeAction.h`
   - Inherit from `MenuAction`
   - Constructor: `SetWheelModeAction(WheelMode mode, ConfigManager* config, EncoderModeManager* modeMgr)`

2. **Execution Logic:**
   - `execute()`:
     - Call `ConfigManager::saveWheelMode(mode)`
     - Call `EncoderModeManager::setMode(mode)`
     - Log "Wheel mode set to: {mode}"

3. **Confirmation:**
   - `getConfirmationMessage()`: Return "Scroll Mode Active", "Volume Mode Active", etc.

4. **Menu Integration:**
   - Update `MenuTree.h` to use `SetWheelModeAction` for Scroll, Volume, Zoom items

## Tasks / Subtasks

- [ ] Create `src/Menu/Action/SetWheelModeAction.h`
- [ ] Create `src/Menu/Action/SetWheelModeAction.cpp`
- [ ] Update `src/Menu/Model/MenuTree.h` (add action instances)

## Dev Notes

### Architecture Compliance

- **Command Pattern:** Encapsulate action logic.
- **Immediate Application:** Must update runtime state (`EncoderModeManager`) AND persistence (`ConfigManager`).

### References

- [Architecture: Command Pattern for Actions](docs/architecture.md#command-pattern-for-actions)
- [Epics: Story 3.1](docs/epics.md#story-31-create-set-wheel-mode-action)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
