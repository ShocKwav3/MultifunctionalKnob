# Story 1.4: Implement Boot Configuration Restore and Factory Reset

Status: Done

## Story

As a **user**,
I want **my saved settings automatically restored when the device powers on**,
so that **I can immediately use my configured wheel mode and button behaviors without manual setup**.

## Acceptance Criteria

1. **Factory Reset Logic:**
   - Create `src/Config/FactoryReset.cpp` and `FactoryReset.h`
   - `isResetRequested(uint8_t buttonPin)`: Check if encoder button is held for 5+ seconds at boot
   - `execute(ConfigManager&, DisplayInterface&)`: Clear NVS, show feedback

2. **Boot Restore:**
   - In `main.cpp` (or equivalent init logic):
     - Check `FactoryReset::isResetRequested()`
     - If yes, `FactoryReset::execute()`
     - If no, `ConfigManager::loadWheelMode()` and `loadButtonAction()`
     - Apply loaded values to `EncoderModeManager` and runtime state

3. **Defaults:**
   - If no config exists, system defaults to `SCROLL` and `NONE` (handled by ConfigManager load logic)

## Tasks / Subtasks

- [x] Create `src/Config/FactoryReset.h`
- [x] Create `src/Config/FactoryReset.cpp`
- [x] Update `src/main.cpp` to integrate reset check and config restore (Note: main.cpp update might be part of Integration phase, but logic definition belongs here)

## Dev Notes

### Architecture Compliance

- **Static Methods:** `FactoryReset` should use static methods as it runs before full system init.
- **Feedback:** Use `DisplayInterface` to show "Factory Reset..." and "Complete".

### References

- [Architecture: Factory Reset Trigger](docs/architecture/index.md#factory-reset-trigger)
- [Epics: Story 1.4](docs/epics/index.md#story-14-implement-boot-configuration-restore-and-factory-reset)

## Dev Agent Record

### Context Reference

- `docs/architecture/index.md`
- `docs/epics/index.md`

### Agent Model Used

- Claude Opus 4.5 (claude-opus-4-5-20251101)

### Implementation Plan

1. Created `FactoryReset.h` with static methods `isResetRequested()` and `execute()`
2. Created `FactoryReset.cpp` implementing 5-second button hold detection and NVS clear
3. Added `clearAll()` method to `ConfigManager` to support factory reset
4. Added `fromWheelMode()` helper to `EncoderModeHelper` for WheelMode to EncoderModeEventTypes conversion
5. Updated `main.cpp` to check for factory reset at boot and load/apply saved wheel mode

### Completion Notes

- Factory reset checks if encoder button (GPIO 2) is held for 5+ seconds at boot
- Uses polling at 100ms intervals to detect sustained button press
- Displays "Factory Reset..." and "Complete" messages via SerialDisplay
- Config restore loads saved wheel mode from NVS and applies to EncoderModeManager
- ZOOM mode falls back to SCROLL since ZOOM handler not yet implemented
- Default values (SCROLL, NONE) are handled by ConfigManager load logic
- Build verified successful with PlatformIO

## File List

### New Files
- `src/Config/FactoryReset.h`
- `src/Config/FactoryReset.cpp`

### Modified Files
- `src/Config/ConfigManager.h` - Added `clearAll()` method declaration
- `src/Config/ConfigManager.cpp` - Added `clearAll()` method implementation
- `src/Helper/EncoderModeHelper.h` - Added `fromWheelMode()` helper declaration
- `src/Helper/EncoderModeHelper.cpp` - Added `fromWheelMode()` helper implementation
- `src/main.cpp` - Added factory reset check and config restore at boot

## Change Log

- 2025-12-18: Implemented factory reset and boot configuration restore (Story 1.4)
