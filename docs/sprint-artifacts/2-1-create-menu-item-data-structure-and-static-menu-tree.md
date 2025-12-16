# Story 2.1: Create Menu Item Data Structure and Static Menu Tree

Status: ready-for-dev

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

- [ ] Create `src/Menu/Action/MenuAction.h`
- [ ] Create `src/Menu/Model/MenuItem.h`
- [ ] Create `src/Menu/Model/MenuTree.h`

## Dev Notes

### Architecture Compliance

- **Constexpr:** Menu tree must be `constexpr` to reside in flash/RO data.
- **Forward Declaration:** `MenuAction` needs to be forward declared in `MenuItem.h` or included.
- **Nullptrs:** Leaf nodes have `children=nullptr`, `childCount=0`. Branch nodes have `action=nullptr`.

### References

- [Architecture: Component Architecture (Menu System)](docs/architecture.md#component-architecture-menu-system)
- [Epics: Story 2.1](docs/epics.md#story-21-create-menu-item-data-structure-and-static-menu-tree)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
