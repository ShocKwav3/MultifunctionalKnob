# Sprint Artifact: Create Menu Controller & Distributed Event Architecture

**Story:** 2.2
**Epic:** 2 - Menu Navigation System
**Status:** ready-for-dev

## Description
Implement the core Menu Controller state machine logic AND the foundational distributed event architecture required for it to function. This includes the Menu Event Pipeline and the Display Arbitration system.

This story consolidates the Logic (Controller) and the Output Infrastructure (Event+Display) to ensure a complete, testable vertical slice.

## Goals
1.  **Menu Logic:** Implement `MenuController` state machine (Navigation, Selection, Back).
2.  **Event Pipeline:** Implement `MenuEvent`, `MenuEventDispatcher`.
3.  **Display Arbitration:** Implement `DisplayRequest`, `DisplayRequestQueue`, and `DisplayTask`.
4.  **Menu Output:** Implement `MenuEventHandler` to translate Events → DisplayRequests.

## Acceptance Criteria

### 1. Menu Controller (Logic)
- [ ] `MenuController` tracks active state, current menu item, and selection index.
- [ ] `handleRotation()` updates index and emits `MENU_NAVIGATION_CHANGED`.
- [ ] `handleSelect()` enters submenu or executes action (emitting `MENU_ITEM_SELECTED`).
- [ ] `handleBack()` traverses up tree or exits (emitting `MENU_DEACTIVATED`).
- [ ] `activate()` resets state and emits `MENU_ACTIVATED`.

### 2. Distributed Event Infrastructure
- [ ] Define `MenuEventType` enum and `MenuEvent` struct (No `AppEvent`).
- [ ] Implement `MenuEventDispatcher` (Singleton/Static).
- [ ] Implement `DisplayRequest` struct (Union of Menu/Status data).
- [ ] Implement `DisplayTask` that consumes `DisplayRequestQueue` and owns `DisplayInterface`.

### 3. Menu Event Handling
- [ ] `MenuEventHandler` subscribes to `MenuEventDispatcher`.
- [ ] On `MENU_NAVIGATION_CHANGED`: Formats menu items and pushes `DRAW_MENU` request to DisplayQueue.
- [ ] On `MENU_ACTIVATED`: Pushes `DRAW_MENU` request.
- [ ] On `MENU_DEACTIVATED`: Pushes `CLEAR` request.

## Technical Implementation Notes
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