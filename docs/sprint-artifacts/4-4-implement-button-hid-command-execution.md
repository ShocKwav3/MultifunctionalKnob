# Story 4.4: Implement Button HID Command Execution

Status: ready-for-dev

## Story

As a **user**,
I want **buttons to send media commands when pressed**,
so that **I can control media playback and volume with physical buttons**.

## Acceptance Criteria

1. **Event Handling:**
   - Modify `src/Event/Handler/EncoderEventHandler.cpp` (or create `ButtonEventHandler`?)
   - Architecture suggests `EncoderEventHandler` handles input, or maybe `AppEventHandler`?
   - Actually, `ButtonManager` dispatches `BUTTON_PRESSED`. We need a handler for this.
   - Let's use `AppEventHandler` or a dedicated `ButtonEventHandler`. Architecture mentions `EncoderEventHandler` for input routing. Let's stick to `EncoderEventHandler` or add logic to `AppEventHandler` if it's application logic.
   - Wait, `ButtonManager` dispatches to `EncoderEventDispatcher`. So `EncoderEventHandler` receives it.

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

- [ ] Modify `src/Event/Handler/EncoderEventHandler.cpp` to handle `BUTTON_PRESSED`

## Dev Notes

### Architecture Compliance

- **Event Flow:** `ButtonManager` -> `EncoderEventDispatcher` -> `EncoderEventHandler`.
- **Logic:** Keep it simple.

### References

- [Architecture: Event Flow](docs/architecture.md#event-flow)
- [Epics: Story 4.4](docs/epics.md#story-44-implement-button-hid-command-execution)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
