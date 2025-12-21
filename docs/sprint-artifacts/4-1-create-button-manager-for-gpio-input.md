# Story 4.1: Create Button Manager for GPIO Input

Status: ready-for-dev

## Story

As a **user**,
I want **the device to detect when I press hardware buttons**,
so that **I can trigger actions with physical button presses**.

## Acceptance Criteria

1. **ButtonManager Class:**
   - Create `src/Button/ButtonManager.cpp` and `ButtonManager.h`
   - `init()`: Configure GPIOs from `BUTTONS[]` (INPUT_PULLUP/DOWN)
   - `update()`: Poll buttons, detect state changes (debounce 50ms)

2. **Event Dispatch:**
   - On press: Dispatch `BUTTON_PRESSED` with `buttonIndex`
   - On release: Dispatch `BUTTON_RELEASED` (optional)

3. **Integration:**
   - Call `init()` in setup
   - Call `update()` in loop

4. **Event Enums:**
   - Update `include/Enum/EventEnum.h` with `BUTTON_PRESSED`, `BUTTON_RELEASED`

## Tasks / Subtasks

- [ ] Update `include/Enum/EventEnum.h`
- [ ] Create `src/Button/ButtonManager.h`
- [ ] Create `src/Button/ButtonManager.cpp`

## Dev Notes

### Architecture Compliance

- **Config Driven:** Use `BUTTONS[]` from `button_config.h`.
- **Non-Blocking:** `update()` must be non-blocking.
- **Event System:** Use `EncoderEventDispatcher` (or `AppEventDispatcher`? Architecture says `EncoderEventDispatcher` for input).

### References

- [Architecture: Component Architecture](docs/architecture.md#component-architecture-menu-system)
- [Epics: Story 4.1](docs/epics.md#story-41-create-button-manager-for-gpio-input)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
