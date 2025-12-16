# Story 1.3: Create Configuration Manager with NVS Persistence

Status: ready-for-dev

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

- [ ] Create `src/Config/ConfigManager.h`
- [ ] Create `src/Config/ConfigManager.cpp`
- [ ] Implement `saveWheelMode` / `loadWheelMode`
- [ ] Implement `saveButtonAction` / `loadButtonAction`

## Dev Notes

### Architecture Compliance

- **Dependency Injection:** Must accept `Preferences*` in constructor for testability.
- **Validation:** Use the validation pattern from architecture (check against MAX enum value).
- **Keys:** Use `"wheel.mode"`, `"btn0.action"`, etc.

### References

- [Architecture: Data Architecture](docs/architecture.md#data-architecture)
- [Epics: Story 1.3](docs/epics.md#story-13-create-configuration-manager-with-nvs-persistence)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
