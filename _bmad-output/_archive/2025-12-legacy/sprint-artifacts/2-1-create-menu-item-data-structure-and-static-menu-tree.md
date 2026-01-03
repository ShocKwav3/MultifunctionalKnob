# Story 2.1: Create Menu Item Data Structure and Static Menu Tree

Status: done

## Story

As a **developer**,
I want **a compile-time menu tree structure with parent-child relationships**,
so that **menu navigation can traverse hierarchically without dynamic memory allocation**.

## Acceptance Criteria

1. **MenuItem Struct:**
   - Create `src/Menu/Model/MenuItem.h`
   - Struct members: `label`, `parent`, `children`, `childCount`, `action`
   - `action` is pointer to `MenuAction` (abstract base)

2. **MenuAction Interface:**
   - Create `src/Menu/Action/MenuAction.h`
   - Pure virtual `execute()`
   - Virtual `getConfirmationMessage()`

3. **Static Menu Tree:**
   - Create `src/Menu/Model/MenuTree.h` (Header-only)
   - Define `constexpr` arrays for submenus and main menu
   - Note: Since `MenuAction` instances may require runtime dependencies (DI), `MenuTree` might need to be initialized at runtime or use a Context pattern. If using `constexpr`, ensure actions are stateless or globally accessible.
   - Main Menu: "Wheel Behavior", "Button Config", "Device Status", "About"
   - Wheel Behavior Submenu: "Scroll", "Volume", "Zoom"
   - Link parents and children correctly

## Tasks / Subtasks

- [x] Create `src/Menu/Action/MenuAction.h`
- [x] Create `src/Menu/Model/MenuItem.h`
- [x] Create `src/Menu/Model/MenuTree.h`

### Review Follow-ups (AI)
- [x] [AI-Review][HIGH] `MenuTree.h` initializes leaf `action` pointers to `nullptr`, contradicting AC #5. This requires runtime injection and compromises the static nature of the tree. [src/Menu/Model/MenuTree.h]
- [x] [AI-Review][HIGH] `MenuTree.h` defines "Button Config" as a leaf node, contradicting AC #3, which states it should have children. The tree is incomplete. [src/Menu/Model/MenuTree.h]
- [x] [AI-Review][MEDIUM] `MenuTree.h` uses `inline MenuItem[]` instead of `constexpr` arrays, deviating from AC #3. [src/Menu/Model/MenuTree.h]

## Dev Notes

### Architecture Compliance

- **Constexpr:** Menu tree must be `constexpr` to reside in flash/RO data.
- **Forward Declaration:** `MenuAction` needs to be forward declared in `MenuItem.h` or included.
- **Nullptrs:** Leaf nodes have `children=nullptr`, `childCount=0`. Branch nodes have `action=nullptr`.

### References

- [Architecture: Component Architecture (Menu System)](_bmad-output/architecture/index.md#component-architecture-menu-system)
- [Epics: Story 2.1](_bmad-output/epics/index.md#story-21-create-menu-item-data-structure-and-static-menu-tree)

## Dev Agent Record

### Context Reference

- `_bmad-output/architecture/index.md`
- `_bmad-output/epics/index.md`
- `_bmad-output/project_context.md`

### Agent Model Used

- claude-opus-4-5-20251101

### Completion Notes

**Implementation Summary:**

1. **MenuAction.h**: Created abstract interface with:
   - Virtual destructor (per project standards)
   - Pure virtual `execute()` method
   - Virtual `getConfirmationMessage()` with default nullptr return

2. **MenuItem.h**: Created struct with:
   - Forward declaration of MenuAction class
   - All required members: `label`, `parent`, `children`, `childCount`, `action`
   - Proper const qualifiers for navigation pointers

3. **MenuTree.h**: Created header-only namespace with:
   - Static menu arrays using `inline` for header-only linkage
   - Complete menu structure with all 4 main items and both submenus
   - `initMenuTree()` function to set parent pointers at runtime
   - Action setter functions: `setWheelBehaviorAction()`, `setButtonConfigAction()`, `setDeviceStatusAction()`, `setAboutAction()`
   - Helper functions: `getRoot()`, `getMainMenu()`, `getMainMenuCount()`
   - Index namespace for safe menu item references

**Review Follow-up Resolutions:**

1. **[HIGH] Action pointers nullptr** - RESOLVED
   - Per AC note: "MenuAction instances may require runtime dependencies (DI), MenuTree might need to be initialized at runtime"
   - Added explicit action setter functions for runtime DI injection
   - Documentation clarifies initialization sequence: `initMenuTree()` then action setters

2. **[HIGH] Button Config as leaf** - RESOLVED
   - Added `buttonConfigSubmenu[]` with 4 placeholder button items
   - Button Config is now a branch node with `childCount=4`
   - Placeholder children will be wired to actual actions in Story 4.3

3. **[MEDIUM] inline vs constexpr** - RESOLVED (Technical Limitation)
   - C++ constexpr cannot support circular references (parent pointers)
   - C++ constexpr cannot support runtime DI objects (action pointers)
   - `inline` is the correct C++ mechanism for header-only static data
   - Constants (counts, indices) use `constexpr` where possible
   - Documentation explains technical rationale

**Build Verification:** Full project build succeeded with no warnings.

## File List

- `src/Menu/Action/MenuAction.h` (new)
- `src/Menu/Model/MenuItem.h` (new)
- `src/Menu/Model/MenuTree.h` (new, updated)

## Change Log

- 2025-12-20: Addressed code review findings - added Button Config submenu, action setter functions, documentation
- 2025-12-20: Implemented menu item data structure and static menu tree (Story 2.1)
