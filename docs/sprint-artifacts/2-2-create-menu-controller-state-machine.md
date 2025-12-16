# Story 2.2: Create Menu Controller State Machine

Status: ready-for-dev

## Story

As a **user**,
I want **to navigate through menu levels using wheel rotation and button presses**,
so that **I can browse options and make selections intuitively**.

## Acceptance Criteria

1. **MenuController Class:**
   - Create `src/Menu/Controller/MenuController.cpp` and `MenuController.h`
   - Track state: `menuActive`, `currentMenu` (MenuItem*), `selectedIndex`

2. **Navigation Logic:**
   - `handleRotation(int delta)`: Increment/decrement `selectedIndex`, wrap around `childCount`
   - `handleSelect()`:
     - If branch: Enter submenu (update `currentMenu`, reset index)
     - If leaf: Execute action (`action->execute()`), exit menu
   - `handleBack()`:
     - If submenu: Go to parent (update `currentMenu`, reset index)
     - If root: Exit menu (`menuActive = false`)

3. **Event Dispatch:**
   - Dispatch `MENU_NAVIGATION_CHANGED` on move/enter/back
   - Dispatch `MENU_DEACTIVATED` on exit
   - Dispatch `MENU_ITEM_SELECTED` on action execution

## Tasks / Subtasks

- [ ] Create `src/Menu/Controller/MenuController.h`
- [ ] Create `src/Menu/Controller/MenuController.cpp`
- [ ] Implement navigation logic
- [ ] Implement event dispatching

## Dev Notes

### Architecture Compliance

- **State Machine:** Explicit state tracking.
- **Event System:** Use `AppEventDispatcher` to notify display.
- **Logging:** Log transitions (`LOG_INFO` for enter/exit, `LOG_DEBUG` for nav).

### References

- [Architecture: Component Architecture (Menu System)](docs/architecture.md#component-architecture-menu-system)
- [Epics: Story 2.2](docs/epics.md#story-22-create-menu-controller-state-machine)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
