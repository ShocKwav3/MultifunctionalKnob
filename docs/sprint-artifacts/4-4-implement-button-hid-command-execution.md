# Story 4.4: Implement Button HID Command Execution

Status: ready-for-dev

## Story

As a **user**,
I want **buttons to send media commands when pressed**,
so that **I can control media playback and volume with physical buttons**.

## Acceptance Criteria

1. **Event Handling:**
   - **Current Implementation:** `ButtonManager` uses `ButtonEventDispatcher` → `buttonEventQueue` → `ButtonEventHandler` (currently logs only)
   - **This Story:** Modify `src/Event/Handler/ButtonEventHandler.cpp` to execute HID commands instead of just logging
   - Architecture is decoupled: Button events are separate from encoder events

2. **Command Execution:**
   - In handler:
     - Get `buttonIndex` from event
     - Call `ConfigManager::loadButtonAction(buttonIndex)`
     - Switch on action:
       - `MUTE`: `bleKeyboard.write(KEY_MEDIA_MUTE)`
       - `PLAY/PAUSE`: `bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE)`
       - `NEXT`: `bleKeyboard.write(KEY_MEDIA_NEXT_TRACK)`
       - `PREVIOUS`: `bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK)`
       - `NONE`: Do nothing

3. **BLE Check:**
   - Only send if `bleKeyboard.isConnected()`

## Tasks / Subtasks

- [ ] Modify `src/Event/Handler/ButtonEventHandler.cpp` to execute HID commands based on button actions

## Dev Notes

### Architecture Compliance

- **Event Flow:** `ButtonManager` -> `ButtonEventDispatcher` -> `buttonEventQueue` -> `ButtonEventHandler`
- **Logic:** Keep it simple - map button index to action, execute HID command

### References

- [Architecture: Event Flow](docs/architecture/index.md#event-flow)
- [Epics: Story 4.4](docs/epics/index.md#story-44-implement-button-hid-command-execution)

## Dev Agent Record

### Context Reference

- `docs/architecture/index.md`
- `docs/epics/index.md`

### Agent Model Used

- google/gemini-3-pro-preview
