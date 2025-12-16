# Story 1.4: Implement Boot Configuration Restore and Factory Reset

Status: ready-for-dev

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

- [ ] Create `src/Config/FactoryReset.h`
- [ ] Create `src/Config/FactoryReset.cpp`
- [ ] Update `src/main.cpp` to integrate reset check and config restore (Note: main.cpp update might be part of Integration phase, but logic definition belongs here)

## Dev Notes

### Architecture Compliance

- **Static Methods:** `FactoryReset` should use static methods as it runs before full system init.
- **Feedback:** Use `DisplayInterface` to show "Factory Reset..." and "Complete".

### References

- [Architecture: Factory Reset Trigger](docs/architecture.md#factory-reset-trigger)
- [Epics: Story 1.4](docs/epics.md#story-14-implement-boot-configuration-restore-and-factory-reset)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
