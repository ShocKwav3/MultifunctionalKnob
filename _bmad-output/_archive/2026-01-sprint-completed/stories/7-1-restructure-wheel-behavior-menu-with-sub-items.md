# Story 7.1: Restructure Wheel Behavior Menu with Sub-Items

Status: done

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

- [x] **Task 1: Audit existing menu structure** (AC: 5)
  - [x] Review `src/Menu/Model/MenuTree.h` for current `wheelBehaviorSubmenu` structure
  - [x] Document existing `MenuItem` array layout
  - [x] Identify `initMenuTree()` function and parent pointer initialization logic
  - [x] Identify `initWheelBehaviorActions()` function for action wiring

- [x] **Task 2: Create new submenu arrays** (AC: 1, 2, 3)
  - [x] Rename existing `wheelBehaviorSubmenu` to `wheelModeSubmenu`
  - [x] Create new `wheelDirectionSubmenu` array with placeholder items:
    - "Normal" (action = nullptr initially)
    - "Reversed" (action = nullptr initially)
  - [x] Create new `wheelBehaviorSubmenu` array containing:
    - "Wheel Mode" (children = wheelModeSubmenu)
    - "Wheel Direction" (children = wheelDirectionSubmenu)

- [x] **Task 3: Update parent pointers in initMenuTree()** (AC: 4, 5)
  - [x] Update parent of `wheelModeSubmenu` items to be the "Wheel Mode" item
  - [x] Update parent of `wheelDirectionSubmenu` items to be the "Wheel Direction" item
  - [x] Update parent of `wheelBehaviorSubmenu` items to be the "Wheel Behavior" main menu item
  - [x] Verify no circular references in parent pointers

- [x] **Task 4: Refactor action initialization** (AC: 2)
  - [x] Update `initWheelBehaviorActions()` to target the correct items in `wheelModeSubmenu`
  - [x] Ensure Scroll, Volume, Zoom actions are wired to the new submenu items
  - [x] Leave Wheel Direction actions as nullptr (to be filled in Story 7.2)

- [x] **Task 5: Update main menu reference** (AC: 1)
  - [x] Verify main menu "Wheel Behavior" item points to new `wheelBehaviorSubmenu`
  - [x] Update childCount if needed

- [x] **Task 6: Build and Verify** (AC: all)
  - [x] Build with `pio run -e use_nimble`
  - [x] Verify no compile errors
  - [x] Manual test: Navigate to Wheel Behavior → Wheel Mode → verify options
  - [x] Manual test: Navigate to Wheel Behavior → Wheel Direction → verify placeholders
  - [x] Manual test: Test back navigation from submenus

### Review Follow-ups (AI)

- [x] [AI-Review][Medium] Refactor `initWheelBehaviorActions` in `src/Menu/Model/MenuTree.h` to use `WheelMode` enum values or named constants instead of magic numbers (0, 1, 2) to ensure alignment between actions and menu labels. [src/Menu/Model/MenuTree.h:237]

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
Claude 3.7 Sonnet (implementation)

### Implementation Plan

**Approach:**
- Restructured menu hierarchy to add intermediate "Wheel Behavior" parent menu
- Created three separate MenuItem arrays: wheelModeSubmenu, wheelDirectionSubmenu, and wheelBehaviorSubmenu
- Updated initMenuTree() to correctly wire parent pointers for new 3-level hierarchy
- Renamed setWheelBehaviorAction() to setWheelModeAction() for clarity
- Updated initWheelBehaviorActions() to target wheelModeSubmenu items
- Left Wheel Direction items with nullptr actions (placeholder for Story 7.2)

**Design Decisions:**
- Maintained static array pattern (no dynamic allocation)
- Preserved constexpr where possible for compile-time initialization
- Updated WHEEL_BEHAVIOR_COUNT from 3 to 2 (now contains "Wheel Mode" and "Wheel Direction" branches)
- Added WHEEL_MODE_COUNT = 3 and WHEEL_DIRECTION_COUNT = 2
- Parent pointer initialization follows established pattern in initMenuTree()

### Completion Notes

✅ **Implementation Complete (2026-01-06)**
- Restructured menu tree with new intermediate layer
- All menu arrays created with correct structure
- Parent pointers correctly initialized in initMenuTree()
- Action wiring updated to target wheelModeSubmenu
- Build successful with no compile errors
- Manual testing required for navigation verification

**Technical Changes:**
- `wheelBehaviorSubmenu` repurposed as parent menu (2 items: Wheel Mode, Wheel Direction)
- `wheelModeSubmenu` created with existing actions (Scroll, Volume, Zoom)
- `wheelDirectionSubmenu` created with placeholders (Normal, Reversed)
- Menu structure now supports 3 levels: Main → Wheel Behavior → Wheel Mode/Direction → Options

**Menu Hierarchy Verification:**
```
Main Menu
└── Wheel Behavior (branch)
    ├── Wheel Mode (branch)
    │   ├── Scroll (leaf, action wired)
    │   ├── Volume (leaf, action wired)
    │   └── Zoom (leaf, action wired)
    └── Wheel Direction (branch)
        ├── Normal (leaf, placeholder)
        └── Reversed (leaf, placeholder)
```

✅ **Resolved review finding [Medium]: Refactored initWheelBehaviorActions to use WheelMode enum values (2026-01-07)**
- Replaced magic numbers (0, 1, 2) with `static_cast<uint8_t>(WheelMode::SCROLL/VOLUME/ZOOM)`
- Ensures type-safe alignment between menu indices and enum values
- Added inline comment explaining the enum-based approach
- Build verified successfully with no errors

✅ **Additional code quality improvements (2026-01-07)**
- Added explicit `#include "Enum/WheelModeEnum.h"` to MenuTree.h (eliminates implicit dependency through SelectWheelModeAction.h)
- Created `wheelModeToDisplayString()` helper function in WheelModeEnum.h for menu label generation
- Refactored `wheelModeSubmenu[]` to use `wheelModeToDisplayString(WheelMode::*)` instead of magic strings
- Ensures menu labels are derived from enum values (single source of truth, compiler-enforced alignment)
- Eliminates "magic string" problem where menu labels could drift from enum definitions

✅ **Manual testing completed (2026-01-07)**
- Navigation to Wheel Behavior → Wheel Mode verified - all options (Scroll, Volume, Zoom) display correctly
- Navigation to Wheel Behavior → Wheel Direction verified - placeholder options (Normal, Reversed) display correctly
- Back navigation from submenus verified - returns to correct parent menu level
- All acceptance criteria satisfied

### Files Modified

- src/Menu/Model/MenuTree.h
- include/Enum/WheelModeEnum.h

## Change Log

- **2026-01-06**: Restructured Wheel Behavior menu to add intermediate menu layer with Wheel Mode and Wheel Direction submenus. Build verified successfully.
- **2026-01-07**: Addressed code review finding - Refactored `initWheelBehaviorActions` to use `WheelMode` enum values instead of magic numbers for type-safe menu action assignment. Added explicit WheelModeEnum.h include and `wheelModeToDisplayString()` helper to eliminate magic strings in menu labels.
