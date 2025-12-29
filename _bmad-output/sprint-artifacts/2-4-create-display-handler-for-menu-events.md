# Story 2.4: Create Display Handler for Menu Events

Status: ready-for-dev

## Story

As a **user**,
I want **to see the current menu state and confirmation messages on the display**,
so that **I know which option is selected and when my actions are confirmed**.

## Acceptance Criteria

1. **DisplayHandler Class:**
   - Create `src/Display/Handler/DisplayHandler.cpp` and `DisplayHandler.h`
   - Holds `DisplayInterface*`
   - Subscribes to `AppEventDispatcher`

2. **Event Handling:**
   - `MENU_ACTIVATED`: `display->clear()`, `display->showMenu(root)`
   - `MENU_NAVIGATION_CHANGED`: `display->showMenu(currentMenu, index)`
   - `MENU_ITEM_SELECTED`: `display->showConfirmation(msg)`
   - `MENU_DEACTIVATED`: `display->clear()`, `display->showMessage("Ready")`

3. **AppEvent Payload:**
   - Update `include/Type/AppEvent.h` to include union for menu data (`MenuItem*`, `index`, `message`)

4. **Initialization:**
   - Register listener in `main.cpp` (or init logic)

## Tasks / Subtasks

- [ ] Update `include/Type/AppEvent.h`
- [ ] Create `src/Display/Handler/DisplayHandler.h`
- [ ] Create `src/Display/Handler/DisplayHandler.cpp`

## Dev Notes

### Architecture Compliance

- **Observer Pattern:** DisplayHandler observes AppEvents.
- **Decoupling:** MenuController doesn't know about DisplayHandler; they communicate via events.

### References

- [Architecture: Communication Patterns](_bmad-output/architecture/index.md#communication-patterns)
- [Epics: Story 2.4](_bmad-output/epics/index.md#story-24-create-display-handler-for-menu-events)

## Dev Agent Record

### Context Reference

- `_bmad-output/architecture/index.md`
- `_bmad-output/epics/index.md`

### Agent Model Used

- google/gemini-3-pro-preview
