# Story 1.3: Create Configuration Manager with NVS Persistence

Status: done

## Story

As a **user**,
I want **my wheel mode and button behavior settings saved to device storage**,
so that **my configuration persists across power cycles without needing to reconfigure**.

## Acceptance Criteria

1. **ConfigManager Class:**
   - Create `src/Config/ConfigManager.cpp` and `ConfigManager.h`
   - Include `<Preferences.h>`
   - Constructor accepts `Preferences*` (Dependency Injection)
   - Use NVS namespace `"knobkoky"`

2. **Wheel Mode Persistence:**
   - `saveWheelMode(WheelMode mode)`: Save to `"wheel.mode"`, return `Error`
   - `loadWheelMode()`: Read `"wheel.mode"`, validate against `WheelMode_MAX`, default to `SCROLL`

3. **Button Action Persistence:**
   - `saveButtonAction(uint8_t index, ButtonAction action)`: Save to `"btnN.action"`, return `Error`
   - `loadButtonAction(uint8_t index)`: Read `"btnN.action"`, validate, default to `NONE`

4. **Error Handling:**
   - Return `Error::OK`, `Error::NVS_WRITE_FAIL`, `Error::INVALID_PARAM`
   - Log errors using `LOG_ERROR`

## Tasks / Subtasks

- [x] Create `src/Config/ConfigManager.h`
- [x] Create `src/Config/ConfigManager.cpp`
- [x] Implement `saveWheelMode` / `loadWheelMode`
- [x] Implement `saveButtonAction` / `loadButtonAction`

## Dev Notes

### Architecture Compliance

- **Dependency Injection:** Must accept `Preferences*` in constructor for testability.
- **Validation:** Use the validation pattern from architecture (check against MAX enum value).
- **Keys:** Use `"wheel.mode"`, `"btn0.action"`, etc.

### References

- [Architecture: Data Architecture](_bmad-output/architecture/index.md#data-architecture)
- [Epics: Story 1.3](_bmad-output/epics/index.md#story-13-create-configuration-manager-with-nvs-persistence)

## Dev Agent Record

### Context Reference

- `_bmad-output/architecture/index.md`
- `_bmad-output/epics/index.md`

### Agent Model Used

- google/gemini-3-pro-preview
- claude-opus-4-5-20251101 (implementation)

### Implementation Plan

1. Created ConfigManager header with DI-enabled constructor accepting `Preferences*`
2. Implemented lazy initialization pattern via `ensureInitialized()` for NVS namespace
3. Used architecture validation pattern: check enum values against `*_MAX` constants
4. Applied standard key naming: `"wheel.mode"`, `"btn0.action"`, `"btn1.action"`, etc.
5. Error handling returns `Error::OK`, `Error::INVALID_PARAM`, `Error::NVS_WRITE_FAIL`
6. Logging via `LOG_ERROR`, `LOG_INFO`, `LOG_DEBUG` macros from `log_config.h`

### Completion Notes

- All acceptance criteria satisfied
- Build passes with no errors
- ConfigManager follows architecture patterns:
  - DI via `Preferences*` constructor parameter
  - Validation against `WheelMode_MAX` and `ButtonAction_MAX`
  - Uses `BUTTON_COUNT` from `button_config.h` for index validation
  - NVS namespace `"knobkoky"` as specified
  - Keys: `"wheel.mode"`, `"btn0.action"` through `"btn3.action"`

## File List

- `src/Config/ConfigManager.h` (new)
- `src/Config/ConfigManager.cpp` (new)

## Change Log

- 2025-12-18: Implemented ConfigManager with wheel mode and button action persistence (claude-opus-4-5-20251101)
