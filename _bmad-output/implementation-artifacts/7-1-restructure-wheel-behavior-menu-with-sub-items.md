# Story 7.1: Restructure Wheel Behavior Menu with Sub-Items

Status: ready-for-dev

## Story

As a **user accessing wheel settings**,
I want **Wheel Behavior to be a parent menu with Wheel Mode and Wheel Direction as sub-items**,
so that **I can access both mode and direction settings in an organized hierarchy**.

## Acceptance Criteria

1. **Given** I am in the main menu
   **When** I select "Wheel Behavior"
   **Then** I see two sub-items: "Wheel Mode" and "Wheel Direction"
   **And** the menu navigation shows these as selectable items

2. **Given** I am in the new "Wheel Behavior" submenu
   **When** I select "Wheel Mode"
   **Then** I see the existing options (Scroll, Volume, Zoom)
   **And** the currently selected mode is visually highlighted

3. **Given** I am in the "Wheel Behavior" submenu
   **When** I select "Wheel Direction"
   **Then** I see placeholder options (Normal, Reversed) for the next story
   **And** the currently selected direction is visually highlighted

4. **Given** I am in a submenu (Wheel Mode or Wheel Direction)
   **When** I select "Back" or use the back gesture (long-press wheel button)
   **Then** I navigate back to the Wheel Behavior parent menu
   **And** the parent menu items are displayed correctly

5. **Given** the menu structure is implemented
   **When** I compile the code
   **Then** the static menu tree is valid
   **And** all parent pointers are correctly linked
   **And** no memory allocation occurs at runtime

## Tasks

- [ ] **Task 1: Audit existing menu structure** (AC: 5)
  - [ ] Review `src/Menu/Model/MenuTree.h` for current `wheelBehaviorSubmenu` structure
  - [ ] Document existing `MenuItem` array layout
  - [ ] Identify `initMenuTree()` function and parent pointer initialization logic
  - [ ] Identify `initWheelBehaviorActions()` function for action wiring

- [ ] **Task 2: Create new submenu arrays** (AC: 1, 2, 3)
  - [ ] Rename existing `wheelBehaviorSubmenu` to `wheelModeSubmenu`
  - [ ] Create new `wheelDirectionSubmenu` array with placeholder items:
    - "Normal" (action = nullptr initially)
    - "Reversed" (action = nullptr initially)
  - [ ] Create new `wheelBehaviorSubmenu` array containing:
    - "Wheel Mode" (children = wheelModeSubmenu)
    - "Wheel Direction" (children = wheelDirectionSubmenu)

- [ ] **Task 3: Update parent pointers in initMenuTree()** (AC: 4, 5)
  - [ ] Update parent of `wheelModeSubmenu` items to be the "Wheel Mode" item
  - [ ] Update parent of `wheelDirectionSubmenu` items to be the "Wheel Direction" item
  - [ ] Update parent of `wheelBehaviorSubmenu` items to be the "Wheel Behavior" main menu item
  - [ ] Verify no circular references in parent pointers

- [ ] **Task 4: Refactor action initialization** (AC: 2)
  - [ ] Update `initWheelBehaviorActions()` to target the correct items in `wheelModeSubmenu`
  - [ ] Ensure Scroll, Volume, Zoom actions are wired to the new submenu items
  - [ ] Leave Wheel Direction actions as nullptr (to be filled in Story 7.2)

- [ ] **Task 5: Update main menu reference** (AC: 1)
  - [ ] Verify main menu "Wheel Behavior" item points to new `wheelBehaviorSubmenu`
  - [ ] Update childCount if needed

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Navigate to Wheel Behavior → Wheel Mode → verify options
  - [ ] Manual test: Navigate to Wheel Behavior → Wheel Direction → verify placeholders
  - [ ] Manual test: Test back navigation from submenus

## Dev Notes

### Architecture Compliance

- **Static Menu Structure**: Maintain the `MenuItem` array pattern in `MenuTree.h` - no dynamic allocation
- **Parent Pointers**: Ensure `initMenuTree()` correctly links the new hierarchy
- **Menu Tree Pattern**: Follow the established pattern from `core-architectural-decisions.md#Component Architecture (Menu System)`

### Existing Code Locations

```
src/Menu/Model/MenuTree.h  - Primary location for menu structure
src/Menu/Controller/MenuController.cpp  - Menu navigation logic
src/Menu/Action/SelectWheelModeAction.h  - Existing action for wheel mode
```

### Menu Structure Changes

**Old Structure:**
```
Main Menu
└── Wheel Behavior
    ├── Scroll
    ├── Volume
    └── Zoom
```

**New Structure:**
```
Main Menu
└── Wheel Behavior
    ├── Wheel Mode
    │   ├── Scroll
    │   ├── Volume
    │   └── Zoom
    └── Wheel Direction
        ├── Normal (placeholder)
        └── Reversed (placeholder)
```

### MenuItem Structure Reference

```cpp
// From architecture/core-architectural-decisions.md
struct MenuItem {
    const char* label;
    MenuItem* parent;
    MenuItem* const* children;
    uint8_t childCount;
    MenuAction* action;
};
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Menu/Model/MenuTree.h` | Restructure arrays to add intermediate layer |
| `src/Menu/Model/MenuTree.cpp` | Update `initMenuTree()` and `initWheelBehaviorActions()` |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Navigation Test**: Verify menu hierarchy navigation works correctly
3. **Back Navigation Test**: Verify back gesture returns to correct parent
4. **Visual Test**: Verify menu items display correctly on SerialDisplay (and OLEDDisplay when implemented)

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Dynamic allocation
MenuItem* submenu = new MenuItem[3];  // Never do this

// ❌ WRONG - Hardcoded parent in struct definition
MenuItem wheelModeItem = {"Wheel Mode", &wheelBehaviorItem, ...};  // Parent set in initMenuTree()

// ✅ CORRECT - Static arrays with runtime parent linking
inline constexpr MenuItem wheelModeSubmenu[] = {
    {"Scroll", nullptr, nullptr, 0, &scrollAction},
    {"Volume", nullptr, nullptr, 0, &volumeAction},
    {"Zoom", nullptr, nullptr, 0, &zoomAction}
};

// In initMenuTree():
for (size_t i = 0; i < 3; i++) {
    wheelModeSubmenu[i].parent = &wheelModeItem;
}
```

### References

- [Source: architecture/core-architectural-decisions.md#Component Architecture (Menu System)] - Menu tree structure and parent pointer pattern
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization and file placement
- [Source: project-context.md#Menu System Gotchas] - Menu intercept order and exit behavior
- [Source: epics.md#Story 7.1] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
