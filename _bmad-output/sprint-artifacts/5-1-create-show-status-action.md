# Story 5.1: Create Show Status Action

Status: in-review

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

- [x] Create `src/Menu/Action/ShowStatusAction.h`
- [x] Create `src/Menu/Action/ShowStatusAction.cpp`

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

- claude-opus-4-5-20251101

### Implementation Plan

Following red-green-refactor cycle:
1. Create ShowStatusAction class inheriting from MenuAction
2. Implement constructor with dependency injection (ConfigManager, BleKeyboard, DisplayInterface)
3. Implement execute() to read and display status information
4. Implement getConfirmationMessage() returning nullptr to keep status on screen

### Completion Notes

**Implementation Summary:**

1. **ShowStatusAction.h**: Created menu action header with:
   - Proper inheritance from MenuAction base class
   - Dependency injection via constructor (ConfigManager*, BleKeyboard*, DisplayInterface*)
   - Forward declarations for minimal compile dependencies
   - BleKeyboard included (typedef, cannot be forward declared)

2. **ShowStatusAction.cpp**: Implemented status display logic:
   - `execute()` method displays current device configuration:
     - Wheel mode (SCROLL/VOLUME/ZOOM) loaded from ConfigManager
     - BLE connection status checked via bleKeyboard->isConnected()
     - All 4 button action assignments displayed using BUTTONS[] config array
   - Clear display before showing status for clean presentation
   - Comprehensive null pointer checking for defensive programming
   - `getConfirmationMessage()` returns nullptr to keep status visible until user navigates back

**Architecture Compliance:**
- Uses DisplayInterface::showStatus() as specified
- Reads state from ConfigManager (single source of truth for persisted config)
- BLE status checked directly from BleKeyboard instance
- Follows project naming conventions (PascalCase class, camelCase methods)
- Proper include order: matching header, C std lib, Arduino, third-party, project headers

**Build Status:** Project compiles successfully with no errors or warnings.

## File List

- `src/Menu/Action/ShowStatusAction.h` (new)
- `src/Menu/Action/ShowStatusAction.cpp` (new)

## Change Log

- 2025-12-30: Implemented ShowStatusAction menu action for device status display (Story 5.1)
