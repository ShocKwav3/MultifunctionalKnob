# Story 11.1: Add FastLED Library and LED Menu Structure

Status: ready-for-dev

## Story

As a **user wanting to control LED strips**,
I want **an LED menu item with Power, Brightness, Mode, and Color sub-options**,
so that **I can access all LED settings from one place**.

## Acceptance Criteria

1. **Given** library installation
   **When** I build the project
   **Then** `FastLED` is available in the project
   **And** no compilation errors occur

2. **Given** menu navigation
   **When** I see the main menu
   **Then** "LED" item is present
   **And** it's clearly labeled

3. **Given** I select "LED"
   **Then** I see sub-items: "Power", "Brightness", "Mode", "Color"
   **And** I can navigate back to the main menu

4. **Given** menu structure is implemented
   **When** I compile the code
   **Then** the static menu tree is valid
   **And** all parent pointers are correctly linked

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Audit Existing Menu Structure** (AC: 4)
  - [ ] Review `src/Menu/Model/MenuTree.h` for current main menu structure
  - [ ] Document existing `MenuItem` array layout
  - [ ] Identify available slots in main menu for new "LED" item
  - [ ] Identify `initMenuTree()` function and parent pointer initialization logic

- [ ] **Task 2: Add FastLED Library to platformio.ini** (AC: 1, 5)
  - [ ] Add `FastLED` to `lib_deps` in `platformio.ini`:
    ```ini
    lib_deps =
        FastLED @ ^3.6.0
    ```
  - [ ] Verify library version compatibility with ESP32-C3
  - [ ] Build to verify no conflicts with existing dependencies

- [ ] **Task 3: Create LED Submenu Array** (AC: 3, 4)
  - [ ] Define `ledSubmenu` array with 4 items:
    - [ ] "Power" (action = nullptr initially, filled in Story 11.2)
    - [ ] "Brightness" (action = nullptr initially, filled in Story 11.3)
    - [ ] "Mode" (action = nullptr initially, filled in Story 11.4)
    - [ ] "Color" (action = nullptr initially, filled in Story 11.5)
  - [ ] Ensure array is `static constexpr` or `inline constexpr` for compile-time initialization

- [ ] **Task 4: Add LED Item to Main Menu** (AC: 2, 3)
  - [ ] Add "LED" item to `mainMenu` array
  - [ ] Set `children` pointer to `ledSubmenu`
  - [ ] Set `childCount` to 4
  - [ ] Set `action` to nullptr (parent menu item has no action)

- [ ] **Task 5: Initialize Parent Pointers** (AC: 4)
  - [ ] Update `initMenuTree()` function:
    - [ ] Set parent of `ledSubmenu` items to be the "LED" main menu item
    - [ ] Set parent of "LED" main menu item to be the main menu root
    - [ ] Verify no circular references in parent pointers

- [ ] **Task 6: Create Placeholder Action Initialization** (AC: 4)
  - [ ] Create `initLedActions()` function (or add to existing init function)
  - [ ] Initialize all LED actions as nullptr for now
  - [ ] These will be filled in Stories 11.2-11.5

- [ ] **Task 7: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Navigate to LED → Verify all 4 sub-items
  - [ ] Manual test: Test back navigation from LED submenu to main menu

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
├── Bluetooth
├── LED (NEW)
│   ├── Power (NEW)
│   ├── Brightness (NEW)
│   ├── Mode (NEW)
│   └── Color (NEW)
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
inline constexpr MenuItem ledSubmenu[] = {
    {"Power", nullptr, nullptr, 0, nullptr},      // Action filled in 11.2
    {"Brightness", nullptr, nullptr, 0, nullptr},  // Action filled in 11.3
    {"Mode", nullptr, nullptr, 0, nullptr},        // Action filled in 11.4
    {"Color", nullptr, nullptr, 0, nullptr}         // Action filled in 11.5
};

inline constexpr MenuItem mainMenu[] = {
    // ... existing items ...
    {"LED", nullptr, ledSubmenu, 4, nullptr},
    // ... other items ...
};
```

### Parent Pointer Initialization

```cpp
// In initMenuTree() function
void initMenuTree() {
    // ... existing initialization ...

    // LED submenu parent pointers
    for (size_t i = 0; i < 4; i++) {
        ledSubmenu[i].parent = &mainMenu[LED_INDEX];
    }

    // LED main menu item parent pointer
    mainMenu[LED_INDEX].parent = nullptr;  // Root level
}
```

### FastLED Library Reference

```ini
# platformio.ini
[env:use_nimble]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino

lib_deps =
    NimBLE-Arduino @ ^2.2.3
    ai-esp32-rotary-encoder @ ^1.7
    Adafruit SSD1306 @ ^2.5.15
    FastLED @ ^3.6.0  # NEW
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
| `platformio.ini` | Add FastLED library dependency |
| `src/Menu/Model/MenuTree.h` | Add LED submenu and main menu item |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Library Test**: Verify FastLED compiles without conflicts
3. **Navigation Test**: Verify menu hierarchy navigation works correctly
4. **Back Navigation Test**: Verify back gesture returns to correct parent
5. **Parent Pointer Test**: Verify no crashes when navigating (indicates correct parent linking)

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Dynamic allocation
MenuItem* submenu = new MenuItem[4];  // Never do this

// ❌ WRONG - Hardcoded parent in struct definition
MenuItem ledItem = {"LED", &mainMenuRoot, ...};  // Parent set in initMenuTree()

// ❌ WRONG - Missing parent pointers
inline constexpr MenuItem ledSubmenu[] = {
    {"Power", nullptr, nullptr, 0, nullptr},  // Parent not set
    {"Brightness", nullptr, nullptr, 0, nullptr}
};

// ✅ CORRECT - Static arrays with runtime parent linking
inline constexpr MenuItem ledSubmenu[] = {
    {"Power", nullptr, nullptr, 0, nullptr},
    {"Brightness", nullptr, nullptr, 0, nullptr},
    {"Mode", nullptr, nullptr, 0, nullptr},
    {"Color", nullptr, nullptr, 0, nullptr}
};

// In initMenuTree():
for (size_t i = 0; i < 4; i++) {
    ledSubmenu[i].parent = &mainMenu[LED_INDEX];
}
```

### LED Menu Flow

```
User Action: Select "LED"
    ↓
Navigate to LED submenu
    ↓
Display 4 options:
  - Power
  - Brightness
  - Mode
  - Color
    ↓
User selects option
    ↓
Execute action (Stories 11.2-11.5)
```

### References

- [Source: architecture/core-architectural-decisions.md#Component Architecture (Menu System)] - Menu tree structure and parent pointer pattern
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#Menu System Gotchas] - Menu intercept order and exit behavior
- [Source: epics.md#Story 11.1] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
