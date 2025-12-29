# Story 5.2: Create Show About Action

Status: ready-for-dev

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

- [ ] Create `include/version.h` (if not exists)
- [ ] Create `src/Menu/Action/ShowAboutAction.h`
- [ ] Create `src/Menu/Action/ShowAboutAction.cpp`

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
