# Story 9.4: Create Menu Mode Display with Status Bar

Status: ready-for-dev

## Story

As a **user navigating the menu**,
I want **the OLED to show a small status bar on top and menu content below**,
so that **I can see important status while browsing settings**.

## Acceptance Criteria

1. **Given** menu is active
   **When** display renders menu content
   **Then** a small status bar appears at the top (BT icon, mode indicator)
   **And** status bar is clearly separated from menu items

2. **Given** status bar is present
   **When** rendering menu items
   **Then** they display below the status bar (y > 8)
   **And** menu items are clearly readable

3. **Given** multiple menu items
   **When** rendering
   **Then** currently selected item is visually highlighted (inverted colors or arrow)
   **And** non-selected items are clearly distinguishable

4. **Given** menu has more items than screen can show
   **When** navigating through menu
   **Then** a scrolling window shows current items
   **And** selected item remains visible

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Define Menu Layout** (AC: 1, 2)
  - [ ] Define pixel layout for 128x32 screen:
    - [ ] Top 8px (y=0-7): Status bar with separator line
    - [ ] Bottom 24px (y=8-31): Menu items (2-3 lines visible)
  - [ ] Document font sizes for status bar and menu items
  - [ ] Ensure all elements fit within 128x32 bounds

- [ ] **Task 2: Implement drawStatusBar() Helper** (AC: 1)
  - [ ] Add `drawStatusBar(const SystemState& state)` method to `OLEDDisplay.h`
  - [ ] Implement in `OLEDDisplay.cpp`:
    - [ ] Draw horizontal separator line at y=7
    - [ ] Draw BT icon (top left) based on connection state
    - [ ] Draw mode indicator (top right): "S", "V", or "Z"
    - [ ] Use small font (size 1) for status bar

- [ ] **Task 3: Update drawMenu() for Status Bar** (AC: 1, 2)
  - [ ] Modify `OLEDDisplay::drawMenu()`:
    - [ ] Call `drawStatusBar()` first (top 8px)
    - [ ] Set cursor to y=8 for menu items
    - [ ] Render menu items with offset from status bar
    - [ ] Ensure menu items don't overlap status bar

- [ ] **Task 4: Implement Menu Item Highlighting** (AC: 3)
  - [ ] In `drawMenu()`:
    - [ ] Track selected item index from DisplayRequest
    - [ ] For selected item:
      - [ ] Invert colors (black on white) or draw arrow indicator
      - [ ] Make it clearly distinguishable from non-selected
    - [ ] For non-selected items:
      - [ ] Use normal colors (white on black)
      - [ ] Ensure readability

- [ ] **Task 5: Implement Menu Scrolling Window** (AC: 4)
  - [ ] Add scrolling logic to `drawMenu()`:
    - [ ] Calculate visible window based on selected index
    - [ ] Show 2-3 items centered around selection
    - [ ] Handle edge cases (first/last items)
    - [ ] Ensure selected item is always visible

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Navigate menu, verify status bar stays visible
  - [ ] Manual test: Verify selected item is highlighted
  - [ ] Manual test: Navigate through long menu, verify scrolling works

## Dev Notes

### Architecture Compliance

- **DisplayInterface**: `drawMenu()` signature handles menu rendering internally
- **State Object**: Use `SystemState` to pass BT and mode info to status bar
- **No Dynamic Allocation**: All menu rendering must use static buffers
- **Non-Blocking**: Menu rendering should be fast and non-blocking

### Menu Layout Design

```
128x32 OLED Menu Layout:
┌────────────────────────────────────────┐
│ [BT]                    S          │  y=0-7 (status bar, 8px)
├────────────────────────────────────────┤
│  > Wheel Behavior                 │  y=8-15 (selected)
│    Buttons                      │  y=16-23
│    Bluetooth                     │  y=24-31
└────────────────────────────────────────┘
  ↑                           ↑
Status Bar                  Menu Items (scrolling window)
```

### Scrolling Window Logic

```cpp
// Calculate visible window around selected item
constexpr uint8_t VISIBLE_ITEMS = 3;
uint8_t windowStart = 0;

if (selectedIndex >= VISIBLE_ITEMS - 1) {
    windowStart = selectedIndex - VISIBLE_ITEMS + 2;
}

// Render items from windowStart to windowStart + VISIBLE_ITEMS
for (uint8_t i = 0; i < VISIBLE_ITEMS; i++) {
    uint8_t itemIndex = windowStart + i;
    if (itemIndex >= totalItems) break;

    // Render item at y = 8 + (i * 8)
    drawMenuItem(itemIndex, 8 + (i * 8), itemIndex == selectedIndex);
}
```

### Highlighting Pattern

```cpp
// Inverted colors for selected item
void drawMenuItem(uint8_t index, uint8_t y, bool selected) {
    if (selected) {
        // Inverted: black on white
        display.fillRect(0, y, 128, 8, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
    } else {
        // Normal: white on black
        display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(4, y + 1);
    display.print(menuItems[index].label);

    // Draw arrow indicator for selected
    if (selected) {
        display.setCursor(0, y + 1);
        display.print(">");
    }
}
```

### Display Request Structure

```cpp
// From DisplayRequest.h
struct DisplayRequest {
    enum Type { DRAW_MENU, SHOW_STATUS, CLEAR } type;
    union {
        struct {
            const char* title;
            const char** items;
            uint8_t count;
            uint8_t selected;
        } menu;
        struct {
            const char* key;
            const char* value;
        } status;
    } data;
};
```

### Existing Code Locations

```
src/Display/Impl/OLEDDisplay.h/cpp       - Menu rendering logic
src/Display/Model/SystemState.h           - State for status bar
src/Display/Bitmaps.h                        - BT icon bitmap
src/Menu/Controller/MenuController.cpp        - Menu navigation
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Display/Impl/OLEDDisplay.h` | Add `drawStatusBar()` method |
| `src/Display/Impl/OLEDDisplay.cpp` | Implement status bar and menu rendering |
| `src/Display/Model/SystemState.h` | Ensure state has BT and mode info |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Status Bar Test**:
   - Navigate to menu
   - Verify status bar displays at top
   - Verify BT icon and mode indicator are visible
3. **Highlighting Test**:
   - Navigate through menu items
   - Verify selected item is highlighted
   - Verify non-selected items are normal
4. **Scrolling Test**:
   - Navigate through long menu (more than 3 items)
   - Verify scrolling window works
   - Verify selected item is always visible
5. **Layout Test**:
   - Verify menu items don't overlap status bar
   - Verify separator line is visible

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - No status bar
void drawMenu(const DisplayRequest& request) {
    // Renders menu items starting at y=0
    // No status bar visible
}

// ❌ WRONG - Overlapping layout
void drawMenu(const DisplayRequest& request) {
    drawStatusBar();  // y=0-7
    drawMenuItems(0);  // y=0, overlaps status bar!
}

// ❌ WRONG - No highlighting
void drawMenuItem(uint8_t index, uint8_t y) {
    display.print(menuItems[index].label);  // All items look the same
}

// ❌ WRONG - No scrolling
void drawMenu(const DisplayRequest& request) {
    for (uint8_t i = 0; i < request.data.menu.count; i++) {
        drawMenuItem(i, 8 + (i * 8), false);  // Always shows first 3
    }
}

// ❌ WRONG - Dynamic allocation
char** itemLabels = new char*[count];  // Never do this

// ✅ CORRECT - Status bar, highlighting, scrolling
void OLEDDisplay::drawMenu(const DisplayRequest& request) {
    // Draw status bar (y=0-7)
    drawStatusBar(state);

    // Draw separator line
    display.drawLine(0, 7, 128, 7, SSD1306_WHITE);

    // Calculate scrolling window
    uint8_t windowStart = calculateWindowStart(request.data.menu.selected);

    // Draw menu items (y=8-31)
    for (uint8_t i = 0; i < VISIBLE_ITEMS; i++) {
        uint8_t itemIndex = windowStart + i;
        if (itemIndex >= request.data.menu.count) break;

        bool selected = (itemIndex == request.data.menu.selected);
        drawMenuItem(itemIndex, 8 + (i * 8), selected);
    }

    display.display();
}
```

### Font Size Reference

| Element | Font Size | Height (px) | Y Position |
|---------|-----------|--------------|------------|
| Status bar | 1 (small) | 8 | 0-7 |
| Menu items | 1 (small) | 8 | 8, 16, 24 |
| Separator line | N/A | 1 | 7 |

### References

- [Source: architecture/core-architectural-decisions.md#Display Arbitration Pattern] - Display request queue usage
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#Hardware Constraints] - 128x32 pixel constraint
- [Source: epics.md#Story 9.4] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
