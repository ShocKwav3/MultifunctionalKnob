# Story 2.3: Integrate Menu with Encoder Event System

Status: ready-for-dev

## Story

As a **user**,
I want **to activate the menu by long-pressing the encoder button**,
so that **I can access configuration options without interrupting normal operation**.

## Acceptance Criteria

1. **Event Interception:**
   - Modify `src/Event/Handler/EncoderEventHandler.cpp`
   - Check `MenuController::isActive()`
   - If active, route events to `MenuController` and return (consume event)
   - If inactive, proceed to normal mode handlers

2. **Menu Activation:**
   - Long-press (when menu inactive) -> `MenuController::activate()`
   - Dispatch `MENU_ACTIVATED`

3. **Menu Input Handling:**
   - Rotation -> `MenuController::handleRotation()`
   - Short Press -> `MenuController::handleSelect()`
   - Long Press -> `MenuController::handleBack()`

4. **Event Enums:**
   - Update `include/Enum/EventEnum.h` with `MENU_ACTIVATED`, `MENU_DEACTIVATED`, `MENU_ITEM_SELECTED`, `MENU_NAVIGATION_CHANGED`

## Tasks / Subtasks

- [ ] Update `include/Enum/EventEnum.h`
- [ ] Modify `src/Event/Handler/EncoderEventHandler.cpp` to integrate `MenuController`

## Dev Notes

### Architecture Compliance

- **Interceptor Pattern:** Menu takes precedence over normal modes.
- **Separation:** `EncoderEventHandler` is the router; `MenuController` is the logic.

### References

- [Architecture: Event Interceptor Pattern](docs/architecture.md#event-interceptor-pattern)
- [Epics: Story 2.3](docs/epics.md#story-23-integrate-menu-with-encoder-event-system)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
