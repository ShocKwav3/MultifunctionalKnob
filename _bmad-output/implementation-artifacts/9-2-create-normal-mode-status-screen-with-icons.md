# Story 9.2: Create Normal Mode Status Screen with Icons

Status: ready-for-dev

## Story

As a **user glancing at the device**,
I want **to see status icons on the OLED showing BT status, wheel mode, direction, and battery**,
so that **I know the device state without entering the menu**.

## Acceptance Criteria

1. **Given** the device is in normal mode (not in menu)
   **When** I look at the OLED display
   **Then** I see the current wheel mode indicator (S/V/Z)
   **And** I see the wheel direction indicator (Arrow or N/R)
   **And** I see battery percentage (placeholder value for now)

2. **Given** 128x32 pixel constraint
   **When** rendering status screen
   **Then** icons are clear and distinguishable
   **And** layout is balanced and readable
   **And** text fits within screen boundaries

3. **Given** the device state changes
   **When** wheel mode changes
   **Then** the mode indicator updates immediately
   **And** no manual refresh is required

4. **Given** the device state changes
   **When** wheel direction changes
   **Then** the direction indicator updates immediately
   **And** no manual refresh is required

5. **Given** implementation is complete
   **When** I build the project
   **Then** build succeeds with no errors or warnings

## Tasks

- [ ] **Task 1: Design Status Screen Layout** (AC: 1, 2)
  - [ ] Create pixel layout for 128x32 screen:
    - [ ] Top row (y=0-7): Status bar with BT icon
    - [ ] Middle row (y=8-23): Large mode indicator (S/V/Z)
    - [ ] Bottom row (y=24-31): Direction and battery info
  - [ ] Document font sizes and character positions
  - [ ] Ensure all elements fit within 128x32 bounds

- [ ] **Task 2: Define SystemState Structure** (AC: 3, 4)
  - [ ] Create `src/Display/Model/SystemState.h`:
    ```cpp
    struct SystemState {
        WheelMode mode;
        WheelDirection direction;
        uint8_t batteryPercent;  // Placeholder for now
        bool bluetoothConnected;
        bool bluetoothPairing;
    };
    ```
  - [ ] Add methods to update state from various components

- [ ] **Task 3: Create Icon Bitmaps** (AC: 2)
  - [ ] Create `src/Display/Bitmaps.h`:
    - [ ] Define BT icon bitmap (8x8 or similar)
    - [ ] Define battery icon bitmap (8x8 or similar)
    - [ ] Define direction arrow bitmap (8x8 or similar)
  - [ ] Use `PROGMEM` for bitmap storage (flash instead of RAM)

- [ ] **Task 4: Implement drawNormalMode() in OLEDDisplay** (AC: 1, 2)
  - [ ] Add `drawNormalMode(const SystemState& state)` method to `OLEDDisplay.h`
  - [ ] Implement in `OLEDDisplay.cpp`:
    - [ ] Clear display
    - [ ] Draw BT icon (top left) based on connection state
    - [ ] Draw mode indicator (center, large text: "S", "V", or "Z")
    - [ ] Draw direction indicator (bottom left: "N" or "R", or arrow icon)
    - [ ] Draw battery percentage (bottom right: "100%" or similar)
    - [ ] Call `display.display()` to update screen

- [ ] **Task 5: Integrate with DisplayTask** (AC: 3, 4)
  - [ ] Update `DisplayTask` to track `SystemState`
  - [ ] Add method to update state from various sources
  - [ ] Call `drawNormalMode()` when not in menu mode
  - [ ] Refresh display when state changes

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Verify status screen displays correctly
  - [ ] Manual test: Change wheel mode, verify indicator updates
  - [ ] Manual test: Change direction, verify indicator updates

## Dev Notes

### Architecture Compliance

- **Display Protocol**: `DisplayTask` should receive state updates via event system
- **State Object**: Use `SystemState` struct to pass full state to display
- **No Dynamic Allocation**: All bitmaps must be stored in PROGMEM (flash)
- **Non-Blocking**: Display updates should be fast and non-blocking

### Screen Layout Design

```
128x32 OLED Layout:
┌────────────────────────────────────────┐
│ [BT]                    [BAT]  │  y=0-7 (8px height)
├────────────────────────────────────────┤
│                                    │  y=8 (separator)
│              S                   │  y=9-23 (15px height)
│                                    │  (large mode indicator)
├────────────────────────────────────────┤
│ N/R                        100%   │  y=24-31 (8px height)
└────────────────────────────────────────┘
  ↑                           ↑
Direction                    Battery
```

### SystemState Structure

```cpp
// src/Display/Model/SystemState.h
#pragma once
#include "EncoderMode/Interface/EncoderModeBaseInterface.h"
#include "Enum/WheelDirection.h"

struct SystemState {
    WheelMode mode;              // Current wheel mode (SCROLL, VOLUME, ZOOM)
    WheelDirection direction;      // Wheel direction (NORMAL, REVERSED)
    uint8_t batteryPercent;      // Battery level (0-100, placeholder)
    bool bluetoothConnected;       // BLE connection state
    bool bluetoothPairing;        // BLE pairing state
};
```

### Bitmap Storage Pattern

```cpp
// src/Display/Bitmaps.h
#pragma once
#include <pgmspace.h>

// 8x8 BT icon (example)
const unsigned char PROGMEM btIcon[] = {
    0b00111100,
    0b01111110,
    0b11011011,
    0b11100111,
    0b11011011,
    0b01111110,
    0b00111100,
    0b00000000
};

// Draw bitmap helper
void drawBitmap(int x, int y, const uint8_t* bitmap) {
    display.drawBitmap(x, y, bitmap, 8, 8, SSD1306_WHITE);
}
```

### Display Update Flow

```
State Change (mode, direction, etc.)
    ↓
Update SystemState
    ↓
Trigger DisplayTask refresh
    ↓
OLEDDisplay::drawNormalMode(state)
    ↓
Render to screen
```

### Existing Code Locations

```
src/Display/Impl/OLEDDisplay.h/cpp       - Add drawNormalMode() method
src/Display/Model/SystemState.h           - New state structure
src/Display/Bitmaps.h                    - Icon bitmaps
src/Display/Task/DisplayTask.cpp            - Track state and trigger updates
src/EncoderMode/Manager/EncoderModeManager.cpp  - Notify state changes
```

### Key Files to Modify

| File | Change |
|------|--------|
| `src/Display/Model/SystemState.h` | New state structure |
| `src/Display/Bitmaps.h` | Icon bitmap definitions |
| `src/Display/Impl/OLEDDisplay.h` | Add `drawNormalMode()` method declaration |
| `src/Display/Impl/OLEDDisplay.cpp` | Implement `drawNormalMode()` rendering |
| `src/Display/Task/DisplayTask.cpp` | Track state and trigger updates |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Layout Test**: Verify all elements fit on 128x32 screen
3. **Mode Indicator Test**:
   - Set mode to Scroll → Verify "S" displays
   - Set mode to Volume → Verify "V" displays
   - Set mode to Zoom → Verify "Z" displays
4. **Direction Indicator Test**:
   - Set direction to Normal → Verify "N" displays
   - Set direction to Reversed → Verify "R" displays
5. **State Update Test**:
   - Change mode/direction → Verify display updates immediately
6. **Battery Placeholder Test**: Verify battery percentage displays (even if placeholder)

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Dynamic allocation for bitmaps
uint8_t* icon = new uint8_t[64];  // Never do this

// ❌ WRONG - RAM storage for bitmaps
const uint8_t btIcon[] = { ... };  // Wastes RAM

// ❌ WRONG - Hardcoded state values
void drawNormalMode() {
    display.println("S");  // Always shows S, ignores actual mode
}

// ❌ WRONG - No state tracking
void drawNormalMode() {
    // Reads from multiple sources directly
    // No single source of truth
}

// ❌ WRONG - Blocking refresh
void updateDisplay() {
    while (stateChanged) {
        drawNormalMode();
        delay(100);  // Blocks system
    }
}

// ✅ CORRECT - PROGMEM storage, state tracking, non-blocking
const unsigned char PROGMEM btIcon[] = { ... };  // Stored in flash

void OLEDDisplay::drawNormalMode(const SystemState& state) {
    display.clearDisplay();

    // Draw mode based on state
    const char* modeChar = (state.mode == WheelMode::SCROLL) ? "S" :
                           (state.mode == WheelMode::VOLUME) ? "V" : "Z";
    display.setCursor(60, 12);
    display.setTextSize(2);
    display.print(modeChar);

    // Draw direction based on state
    display.setCursor(10, 24);
    display.setTextSize(1);
    display.print((state.direction == WheelDirection::NORMAL) ? "N" : "R");

    display.display();
}
```

### Font Size Reference

| Text Size | Width (px) | Height (px) | Use Case |
|-----------|--------------|--------------|-----------|
| 1 (small) | 6 | 8 | Status text, labels |
| 2 (medium) | 12 | 16 | Mode indicator |
| 3 (large) | 18 | 24 | Too big for 32px height |

### References

- [Source: architecture/core-architectural-decisions.md#Display Arbitration Pattern] - Display request queue usage
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#Hardware Constraints] - 128x32 pixel constraint
- [Source: epics.md#Story 9.2] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
