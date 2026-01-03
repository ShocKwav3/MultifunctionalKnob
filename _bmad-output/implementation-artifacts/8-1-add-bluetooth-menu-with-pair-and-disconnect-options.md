# Story 8.1: Add Bluetooth Menu with Pair and Disconnect Options

Status: ready-for-dev

## Story

As a **user managing Bluetooth connections**,
I want **a Bluetooth menu item with Pair and Disconnect sub-options**,
so that **I can control BLE connectivity directly from the device**.

## Acceptance Criteria

1. **Given** I am in the main menu
   **When** I select "Bluetooth"
   **Then** I see two sub-items: "Pair" and "Disconnect"
   **And** the menu navigation shows these as selectable items

2. **Given** I am in the "Bluetooth" submenu
   **When** I select "Back" or use the back gesture (long-press wheel button)
   **Then** I navigate back to the main menu
   **And** the main menu items are displayed correctly

3. **Given** the menu structure is implemented
   **When** I compile the code
   **Then** the static menu tree is valid
   **And** all parent pointers are correctly linked
   **And** no memory allocation occurs at runtime

4. **Given** the Bluetooth menu is displayed
   **When** I view the menu items
   **Then** "Pair" and "Disconnect" items are clearly labeled
   **And** the currently selected item is visually highlighted

5. **Given** the implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Audit existing menu structure** (AC: 3)
  - [ ] Review `src/Menu/Model/MenuTree.h` for current main menu structure
  - [ ] Document existing `MenuItem` array layout
  - [ ] Identify `initMenuTree()` function and parent pointer initialization logic
  - [ ] Identify available slots in main menu for new "Bluetooth" item

- [ ] **Task 2: Create Bluetooth submenu array** (AC: 1, 4)
  - [ ] Define `bluetoothSubmenu` array with two items:
    - [ ] "Pair" (action = nullptr initially, to be filled in Story 8.2)
    - [ ] "Disconnect" (action = nullptr initially, to be filled in Story 8.3)
  - [ ] Ensure array is `static constexpr` or `inline constexpr` for compile-time initialization

- [ ] **Task 3: Add Bluetooth item to main menu** (AC: 1)
  - [ ] Add "Bluetooth" item to `mainMenu` array
  - [ ] Set `children` pointer to `bluetoothSubmenu`
  - [ ] Set `childCount` to 2
  - [ ] Set `action` to nullptr (parent menu item has no action)

- [ ] **Task 4: Initialize parent pointers in initMenuTree()** (AC: 2, 3)
  - [ ] Update `initMenuTree()` to set parent of `bluetoothSubmenu` items
  - [ ] Set parent of "Pair" item to be the "Bluetooth" main menu item
  - [ ] Set parent of "Disconnect" item to be the "Bluetooth" main menu item
  - [ ] Set parent of "Bluetooth" main menu item to be the main menu root
  - [ ] Verify no circular references in parent pointers

- [ ] **Task 5: Create placeholder action initialization** (AC: 3)
  - [ ] Create `initBluetoothActions()` function (or add to existing init function)
  - [ ] Initialize `PairAction` and `DisconnectAction` as nullptr for now
  - [ ] These will be filled in Stories 8.2 and 8.3

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Navigate to Bluetooth → Verify "Pair" and "Disconnect" items
  - [ ] Manual test: Test back navigation from Bluetooth submenu to main menu

## Dev Notes

### Architecture Compliance

- **Static Menu Structure**: Maintain the `MenuItem` array pattern in `MenuTree.h` - no dynamic allocation
- **Parent Pointers**: Ensure `initMenuTree()` correctly links the new hierarchy
- **Menu Tree Pattern**: Follow established pattern from `core-architectural-decisions.md#Component Architecture (Menu System)`
- **No Dynamic Allocation**: Use `static constexpr` or `inline constexpr` arrays

### Menu Structure Changes

```
Main Menu
├── Wheel Behavior
├── Buttons
├── Bluetooth (NEW)
│   ├── Pair (NEW)
│   └── Disconnect (NEW)
└── [other existing items]
```

### MenuItem Structure Reference

```cpp
// From architecture/core-architectural-decisions.md#Component Architecture (Menu System)
struct MenuItem {
    const char* label;
    MenuItem* parent;
    MenuItem* const* children;
    uint8_t childCount;
    MenuAction* action;
};
```

### Static Array Pattern

```cpp
// src/Menu/Model/MenuTree.h
inline constexpr MenuItem bluetoothSubmenu[] = {
    {"Pair", nullptr, nullptr, 0, nullptr},      // Action filled in 8.2
    {"Disconnect", nullptr, nullptr, 0, nullptr}  // Action filled in 8.3
};

inline constexpr MenuItem mainMenu[] = {
    // ... existing items ...
    {"Bluetooth", nullptr, bluetoothSubmenu, 2, nullptr},
    // ... other items ...
};
```

### Parent Pointer Initialization

```cpp
// In initMenuTree() function
void initMenuTree() {
    // ... existing initialization ...

    // Bluetooth submenu parent pointers
    bluetoothSubmenu[0].parent = &mainMenu[BLUETOOTH_INDEX];
    bluetoothSubmenu[1].parent = &mainMenu[BLUETOOTH_INDEX];

    // Bluetooth main menu item parent pointer
    mainMenu[BLUETOOTH_INDEX].parent = nullptr;  // Root level
}
```

### Existing Code Locations

```
src/Menu/Model/MenuTree.h  - Primary location for menu structure
src/Menu/Controller/MenuController.cpp  - Menu navigation logic
src/Menu/Action/MenuAction.h  - Base action class
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Menu/Model/MenuTree.h` | Add Bluetooth submenu and main menu item |
| `src/Menu/Model/MenuTree.cpp` | Update `initMenuTree()` for parent pointers |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Navigation Test**: Verify menu hierarchy navigation works correctly
3. **Back Navigation Test**: Verify back gesture returns to correct parent
4. **Visual Test**: Verify menu items display correctly on SerialDisplay (and OLEDDisplay when implemented)
5. **Parent Pointer Test**: Verify no crashes when navigating (indicates correct parent linking)

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Dynamic allocation
MenuItem* submenu = new MenuItem[2];  // Never do this

// ❌ WRONG - Hardcoded parent in struct definition
MenuItem bluetoothItem = {"Bluetooth", &mainMenuRoot, ...};  // Parent set in initMenuTree()

// ❌ WRONG - Missing parent pointers
inline constexpr MenuItem bluetoothSubmenu[] = {
    {"Pair", nullptr, nullptr, 0, nullptr},  // Parent not set
    {"Disconnect", nullptr, nullptr, 0, nullptr}
};

// ✅ CORRECT - Static arrays with runtime parent linking
inline constexpr MenuItem bluetoothSubmenu[] = {
    {"Pair", nullptr, nullptr, 0, nullptr},
    {"Disconnect", nullptr, nullptr, 0, nullptr}
};

// In initMenuTree():
for (size_t i = 0; i < 2; i++) {
    bluetoothSubmenu[i].parent = &mainMenu[BLUETOOTH_INDEX];
}
```

### References

- [Source: architecture/core-architectural-decisions.md#Component Architecture (Menu System)] - Menu tree structure and parent pointer pattern
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization and file placement
- [Source: project-context.md#Menu System Gotchas] - Menu intercept order and exit behavior
- [Source: epics.md#Story 8.1] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
