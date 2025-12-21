# Story 2.2: Create Menu Controller State Machine

Status: done

## Story

As a **user of the system**,
I want to **navigate and control the device using a menu system driven by a distributed event architecture**,
So that **I can interact with various settings and see immediate feedback through a display arbitration system.**

## Acceptance Criteria

1.  **Menu Controller (Logic):**
    - [x] `MenuController` tracks active state, current menu item, and selection index.
    - [x] `handleRotation()` updates index and emits `MENU_NAVIGATION_CHANGED`.
    - [x] `handleSelect()` enters submenu or executes action (emitting `MENU_ITEM_SELECTED`).
    - [x] `handleBack()` traverses up tree or exits (emitting `MENU_DEACTIVATED`).
    - [x] `activate()` resets state and emits `MENU_ACTIVATED`.

2.  **Distributed Event Infrastructure:**
    - [x] Define `MenuEventType` enum and `MenuEvent` struct (No `AppEvent`).
    - [x] Implement `MenuEventDispatcher` (Singleton/Static).
    - [x] Implement `DisplayRequest` struct (Union of Menu/Status data).
    - [x] Implement `DisplayTask` that consumes `DisplayRequestQueue` and owns `DisplayInterface`.

3.  **Menu Event Handling:**
    - [x] `MenuEventHandler` subscribes to `MenuEventDispatcher`.
    - [x] On `MENU_NAVIGATION_CHANGED`: Formats menu items and pushes `DRAW_MENU` request to DisplayQueue.
    - [x] On `MENU_ACTIVATED`: Pushes `DRAW_MENU` request.
    - [x] On `MENU_DEACTIVATED`: Pushes `CLEAR` request.

## Review Follow-ups (AI)
- [x] [AI-Review][CRITICAL] Incorrect pointer type in `MenuEventHandler::sendDrawMenuRequest`. → Fixed: Changed `DisplayInterface::showMenu` signature to `const char* const*` for proper const-correctness.
- [x] [AI-Review][HIGH] `xQueueSend` return value not checked. → Fixed: Added LOG_INFO when queue send fails.
- [x] [AI-Review][HIGH] Event Dispatch and Action Execution Order. → Rejected: Current order (dispatch then execute) is semantically correct.
- [x] [AI-Review][HIGH] Missing Confirmation Display for `MENU_ITEM_SELECTED`. → Rejected: Not in ACs. Actions should trigger their own display updates.
- [x] [AI-Review][MEDIUM] Story's "File List" is incomplete. → Rejected: File list is complete.
- [x] [AI-Review][MEDIUM] `DisplayRequestQueue` default size mismatch. → Fixed: Changed default from 5 to 10 per project_context.md.
- [x] [AI-Review][MEDIUM] `const_cast` used in `DisplayTask::processRequest`. → Fixed: Removed const_cast after DisplayInterface signature fix.
- [x] [AI-Review][MEDIUM] Inconsistent Menu Deactivation on Action Execution. → Rejected: Per-action decision, not in ACs.

## Dev Notes
- **Files:**
    - `src/Menu/Controller/MenuController.cpp/.h`
    - `src/Display/Task/DisplayTask.cpp/.h`
    - `src/Event/Dispatcher/MenuEventDispatcher.cpp/.h`
    - `src/Event/Handler/MenuEventHandler.cpp/.h`
    - `include/Type/MenuEvent.h`
    - `src/Display/Model/DisplayRequest.h`
- **Dependencies:**
    - `MenuController` uses `MenuEventDispatcher`.
    - `MenuEventHandler` uses `DisplayRequestQueue`.
    - `DisplayTask` uses `DisplayInterface` (from Story 1.2).

## References
- **Architecture:** Distributed Event Architecture (Option B)
- **Epics:** Story 2.2, 2.4 (Merged)

---

## File List

### New Files Created
- `include/Type/MenuEvent.h` - MenuEventType enum and MenuEvent struct
- `src/Display/Model/DisplayRequest.h` - DisplayRequest struct with union for menu/status/message data
- `src/Event/Dispatcher/MenuEventDispatcher.h` - Static dispatcher header
- `src/Event/Dispatcher/MenuEventDispatcher.cpp` - Static dispatcher implementation
- `src/Display/Task/DisplayTask.h` - FreeRTOS display task header
- `src/Display/Task/DisplayTask.cpp` - FreeRTOS display task implementation
- `src/Menu/Controller/MenuController.h` - Menu state machine header
- `src/Menu/Controller/MenuController.cpp` - Menu state machine implementation
- `src/Event/Handler/MenuEventHandler.h` - Menu event handler header
- `src/Event/Handler/MenuEventHandler.cpp` - Menu event handler implementation

### New Directories Created
- `src/Display/Model/`
- `src/Display/Task/`
- `src/Menu/Controller/`

### Modified Files
- `src/Display/Interface/DisplayInterface.h` - Updated showMenu signature for const-correctness
- `src/Display/Impl/SerialDisplay.h` - Updated showMenu signature
- `src/Display/Impl/SerialDisplay.cpp` - Updated showMenu signature

---

## Dev Agent Record

### Context Reference
- `docs/project_context.md`
- `docs/architecture.md`
- `docs/epics.md`
- `docs/source-tree-analysis.md`
- `docs/development-guide.md`

### Agent Model Used
- google/gemini-pro

### Implementation Plan
Implemented the distributed event architecture (Option B) per architecture.md:
1. MenuController emits MenuEvent via static MenuEventDispatcher
2. MenuEventHandler consumes MenuEvent queue and translates to DisplayRequest
3. DisplayTask consumes DisplayRequest queue and routes to DisplayInterface

### Completion Notes
- All components implemented following architecture patterns
- MenuController provides full state machine: activate, deactivate, handleRotation, handleSelect, handleBack
- Rotation wraps at boundaries (circular navigation)
- Back navigation restores cursor position to parent menu item
- DisplayTask is a FreeRTOS task that arbitrates display access
- MenuEventHandler formats menu items from MenuItem children array
- Build passes for both use_nimble and use_stdble environments

---

## Change Log

| Date | Change | Author |
|------|--------|--------|
| 2025-12-20 | Implemented Menu Controller, Event Infrastructure, and Menu Event Handling per ACs | Dev Agent |
| 2025-12-20 | Addressed code review: Fixed const-correctness in DisplayInterface, added queue send logging, fixed queue size to 10. Rejected 4 items as out-of-scope. | Dev Agent |