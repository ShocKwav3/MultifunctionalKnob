# Epic 2: Menu Navigation System

**Goal:** User can enter, navigate, select items, and exit the hierarchical menu using the encoder's two-input model (wheel rotation + button press).

**FRs covered:** FR1, FR2, FR3, FR4, FR5, FR6, FR7, FR8

---

## Story 2.1: Create Menu Item Data Structure and Static Menu Tree

As a **developer**,
I want **a compile-time menu tree structure with parent-child relationships**,
So that **menu navigation can traverse hierarchically without dynamic memory allocation**.

**Acceptance Criteria:**

**Given** the project needs a menu item data structure
**When** I create `src/Menu/Model/MenuItem.h`
**Then** it defines:
```cpp
struct MenuItem {
    const char* label;
    MenuItem* parent;
    MenuItem* const* children;
    uint8_t childCount;
    MenuAction* action;
};
```
**And** uses `#pragma once` header guard

**Given** the project needs an abstract action interface
**When** I create `src/Menu/Action/MenuAction.h`
**Then** it defines:
```cpp
class MenuAction {
public:
    virtual ~MenuAction() = default;
    virtual void execute() = 0;
    virtual const char* getConfirmationMessage() const { return nullptr; }
};
```

**Given** the menu system needs a static tree definition
**When** I create `src/Menu/Model/MenuTree.h`
**Then** it defines the 4-item main menu as `constexpr`:
- "Wheel Behavior" (has children for modes)
- "Button Config" (has children for buttons)
- "Device Status" (leaf with action)
- "About" (leaf with action)
**And** wheel behavior submenu has children: "Scroll", "Volume", "Zoom"
**And** parent pointers enable back navigation
**And** leaf items have non-null `action` pointers

**Given** a menu item is a branch (has children)
**When** the item is selected
**Then** `childCount > 0` and `children != nullptr`
**And** `action == nullptr` (branches don't execute actions)

**Given** a menu item is a leaf (no children)
**When** the item is selected
**Then** `childCount == 0` and `children == nullptr`
**And** `action != nullptr` (leaves execute actions)

---

## Story 2.2: Create Menu Controller State Machine

As a **user**,
I want **to navigate through menu levels using wheel rotation and button presses**,
So that **I can browse options and make selections intuitively**.

**Acceptance Criteria:**

**Given** the project needs menu state management
**When** I create `src/Menu/Controller/MenuController.cpp` and `MenuController.h`
**Then** `MenuController` class tracks:
- `bool menuActive` - whether menu is currently displayed
- `MenuItem* currentMenu` - pointer to current menu/submenu
- `uint8_t selectedIndex` - currently highlighted item index
**And** uses `#pragma once` header guard

**Given** the menu is inactive
**When** `isActive()` is called
**Then** it returns `false`
**And** encoder events pass through to normal mode handlers

**Given** the menu is active and user rotates encoder clockwise
**When** `handleRotation(int delta)` is called with positive delta
**Then** `selectedIndex` increments (wraps at `currentMenu->childCount`)
**And** `MENU_NAVIGATION_CHANGED` event is dispatched via `MenuEventDispatcher`
**And** `LOG_DEBUG` reports new selection index

**Given** the menu is active and user rotates encoder counter-clockwise
**When** `handleRotation(int delta)` is called with negative delta
**Then** `selectedIndex` decrements (wraps at 0 to `childCount - 1`)
**And** `MENU_NAVIGATION_CHANGED` event is dispatched

**Given** the menu is active and user short-presses on a branch item
**When** `handleSelect()` is called and selected item has children
**Then** `currentMenu` changes to the selected child's children array
**And** `selectedIndex` resets to 0
**And** `MENU_NAVIGATION_CHANGED` event is dispatched
**And** `LOG_INFO` reports "Entered submenu: {label}"

**Given** the menu is active and user short-presses on a leaf item
**When** `handleSelect()` is called and selected item has an action
**Then** `action->execute()` is called
**And** confirmation event is dispatched if `getConfirmationMessage()` returns non-null
**And** menu exits automatically (FR8)
**And** `LOG_INFO` reports "Executed: {label}"

**Given** the menu is active at a submenu level and user long-presses
**When** `handleBack()` is called and `currentMenu->parent != nullptr`
**Then** `currentMenu` changes to parent menu
**And** `selectedIndex` resets to 0
**And** `MENU_NAVIGATION_CHANGED` event is dispatched
**And** `LOG_DEBUG` reports "Back to: {parent label}"

**Given** the menu is active at main menu level and user long-presses
**When** `handleBack()` is called and `currentMenu->parent == nullptr`
**Then** `menuActive` is set to `false`
**And** `MENU_DEACTIVATED` event is dispatched
**And** `LOG_INFO` reports "Menu closed"

---

## Story 2.3: Integrate Menu with Encoder Event System

As a **user**,
I want **to activate the menu by long-pressing the encoder button**,
So that **I can access configuration options without interrupting normal operation**.

**Acceptance Criteria:**

**Given** the menu system needs to intercept encoder events
**When** I modify `src/Event/Handler/EncoderEventHandler.cpp`
**Then** it checks `MenuController::isActive()` before dispatching to mode handlers
**And** if menu is active, events route to `MenuController` instead

**Given** the device is in normal operation mode
**When** user long-presses the encoder button
**Then** `MenuController::activate()` is called
**And** `menuActive` is set to `true`
**And** `currentMenu` is set to main menu root
**And** `selectedIndex` is set to 0
**And** `MENU_ACTIVATED` event is dispatched via `MenuEventDispatcher`
**And** `LOG_INFO` reports "Menu activated"

**Given** the menu is active
**When** encoder rotation event is received
**Then** `MenuController::handleRotation(delta)` is called
**And** event is consumed (not passed to mode handlers)

**Given** the menu is active
**When** encoder short-press event is received
**Then** `MenuController::handleSelect()` is called
**And** event is consumed

**Given** the menu is active
**When** encoder long-press event is received
**Then** `MenuController::handleBack()` is called
**And** event is consumed

**Given** the menu exits (via back from main or action execution)
**When** `menuActive` becomes `false`
**Then** `MENU_DEACTIVATED` event is dispatched
**And** normal encoder event routing resumes
**And** current wheel mode handler receives subsequent events

**Given** the event system is transitioning to a distributed architecture
**When** `AppEventDispatcher` is no longer used for menu events
**Then** `MenuEventDispatcher` is utilized for all menu-related event dispatches

**Given** new event types are needed
**When** I create `include/Type/MenuEvent.h`
**Then** it defines `enum class MenuEventType` with values: `ACTIVATED`, `DEACTIVATED`, `ITEM_SELECTED`, `NAVIGATION_CHANGED`
**And** defines `struct MenuEvent` payload

---

## Story 2.4: Create Menu Event Handler and Display Arbitrator

As a **user**,
I want **to see the current menu state and confirmation messages on the display without corruption**,
So that **I can clearly read the screen even if other system events occur**.

**Acceptance Criteria:**

**Given** the project needs display arbitration
**When** I create `src/Display/Task/DisplayTask.cpp` and `DisplayTask.h`
**Then** `DisplayTask` is the ONLY component that holds a `DisplayInterface*`
**And** it consumes a `DisplayRequestQueue`
**And** it processes one `DisplayRequest` at a time to prevent hardware conflicts

**Given** the project needs to format menu events for display
**When** I create `src/Event/Handler/MenuEventHandler.cpp` and `MenuEventHandler.h`
**Then** `MenuEventHandler` subscribes to `MenuEventDispatcher`
**And** for each `MenuEvent`, it formats a `DisplayRequest` (type `DRAW_MENU` or `SHOW_CONFIRMATION`)
**And** sends the request to `DisplayRequestQueue`

**Given** menu is activated
**When** `MENU_ACTIVATED` event is received by Handler
**Then** Handler creates `DisplayRequest` with main menu items
**And** sends to queue
**And** `DisplayTask` pops request and calls `display->showMenu()`

**Given** user navigates within menu
**When** `MENU_NAVIGATION_CHANGED` event is received
**Then** Handler formats new `DisplayRequest` with updated selection
**And** `DisplayTask` updates the screen

**Given** user selects an action item
**When** `MENU_ITEM_SELECTED` event is received with confirmation message
**Then** Handler formats `DisplayRequest` (type `SHOW_CONFIRMATION`)
**And** `DisplayTask` calls `display->showConfirmation()`

**Given** menu is deactivated
**When** `MENU_DEACTIVATED` event is received
**Then** Handler formats `DisplayRequest` (type `CLEAR` or `SHOW_STATUS`)
**And** `DisplayTask` clears the screen

**Given** DisplayTask needs initialization
**When** `main.cpp` runs setup
**Then** `DisplayTask` is created as a FreeRTOS task
**And** `DisplayRequestQueue` is initialized

---
