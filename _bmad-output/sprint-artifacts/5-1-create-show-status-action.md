# Story 5.1: Create Show Status Action

Status: ready-for-dev

## Story

As a **user**,
I want **to view current device configuration and connection status**,
so that **I can troubleshoot issues and verify my settings are correct**.

## Acceptance Criteria

1. **ShowStatusAction Class:**
   - Create `src/Menu/Action/ShowStatusAction.cpp` and `ShowStatusAction.h`
   - Inherit from `MenuAction`
   - Constructor accepts `ConfigManager*`, `EncoderModeManager*`, `BleKeyboard*`, `DisplayInterface*`

2. **Execution Logic:**
   - `execute()`:
     - Call `DisplayInterface::showStatus("Wheel Mode", currentModeName)`
     - Call `DisplayInterface::showStatus("BLE", isConnected ? "Connected" : "Disconnected")`
     - Loop buttons and show assignments
     - (Optional) Show RSSI if available

3. **Interaction:**
   - `getConfirmationMessage()` returns `nullptr` (keeps status on screen)
   - User exits via back button (handled by MenuController)

## Tasks / Subtasks

- [ ] Create `src/Menu/Action/ShowStatusAction.h`
- [ ] Create `src/Menu/Action/ShowStatusAction.cpp`

## Dev Notes

### Architecture Compliance

- **Display Interface:** Use `showStatus` method.
- **Dependencies:** Needs access to managers to read state.

### References

- [Architecture: Component Architecture](_bmad-output/architecture/index.md#component-architecture-menu-system)
- [Epics: Story 5.1](_bmad-output/epics/index.md#story-51-create-show-status-action)

## Dev Agent Record

### Context Reference

- `_bmad-output/architecture/index.md`
- `_bmad-output/epics/index.md`

### Agent Model Used

- google/gemini-3-pro-preview
