# Story 5.2: Create Show About Action

Status: done

## Story

As a **user**,
I want **to view device information and firmware version**,
so that **I know what version I'm running and can reference it for support**.

## Acceptance Criteria

1. **ShowAboutAction Class:**
   - Create `src/Menu/Action/ShowAboutAction.cpp` and `ShowAboutAction.h`
   - Inherit from `MenuAction`
   - Constructor accepts `DisplayInterface*`

2. **Execution Logic:**
   - `execute()`:
     - Call `DisplayInterface::showStatus("Device", BLE_DEVICE_NAME)`
     - Call `DisplayInterface::showStatus("Version", FIRMWARE_VERSION)`
     - Call `DisplayInterface::showStatus("By", BLE_MANUFACTURER)`

3. **Version Header:**
   - Ensure `include/version.h` exists with `FIRMWARE_VERSION`

## Tasks / Subtasks

- [x] Create `include/version.h` (if not exists)
- [x] Create `src/Menu/Action/ShowAboutAction.h`
- [x] Create `src/Menu/Action/ShowAboutAction.cpp`

## Dev Notes

### Architecture Compliance

- **Constants:** Use constants from `device_config.h` and `version.h`.

### References

- [Architecture: Component Architecture](_bmad-output/architecture/index.md#component-architecture-menu-system)
- [Epics: Story 5.2](_bmad-output/epics/index.md#story-52-create-show-about-action)

## Dev Agent Record

### Context Reference

- `_bmad-output/architecture/index.md`
- `_bmad-output/epics/index.md`

### Agent Model Used

- google/gemini-3-pro-preview

### Implementation Plan

Task 1: Verified `include/version.h` already exists with `FIRMWARE_VERSION` constant defined.

Tasks 2-3: Created `ShowAboutAction` class following established patterns:
- Header file with forward declarations for DisplayInterface
- Implementation with constructor accepting DisplayInterface pointer
- `execute()` method displays device info, version, and manufacturer using `DisplayInterface::showStatus()`
- Uses constants from `device_config.h` (BLUETOOTH_DEVICE_NAME, BLUETOOTH_DEVICE_MANUFACTURER) and `version.h` (FIRMWARE_VERSION)
- Returns `nullptr` from `getConfirmationMessage()` to keep about screen visible until user navigates back

### Completion Notes

✅ All tasks completed successfully:
- Task 1: `include/version.h` already exists with FIRMWARE_VERSION
- Task 2: Created `src/Menu/Action/ShowAboutAction.h` - inherits from MenuAction
- Task 3: Created `src/Menu/Action/ShowAboutAction.cpp` - displays device, version, manufacturer

Implementation follows architecture patterns:
- Follows MenuAction interface contract
- Uses dependency injection (DisplayInterface pointer in constructor)
- Follows established naming conventions (PascalCase class, camelCase methods)
- Uses LOG_INFO for diagnostic output
- Null-checks DisplayInterface before use
- Clear display before showing content

Build verified successful with PlatformIO.

## File List

### New Files
- `src/Menu/Action/ShowAboutAction.h`
- `src/Menu/Action/ShowAboutAction.cpp`

### Modified Files
- None

### Deleted Files
- None

## Change Log

- **2025-12-30**: Initial implementation of ShowAboutAction menu action
  - Created ShowAboutAction class to display device information
  - Displays device name, firmware version, and manufacturer on About screen
  - Follows MenuAction interface pattern established in the codebase
