# Story 9.1: Implement OLEDDisplay Driver for 128x32 SSD1306

Status: ready-for-dev

## Story

As a **developer integrating the physical display**,
I want **an OLEDDisplay class implementing DisplayInterface for the 128x32 SSD1306**,
so that **the existing display abstraction works with the physical hardware**.

## Acceptance Criteria

1. **Given** the DisplayInterface abstraction exists
   **When** OLEDDisplay is instantiated with I2C pins (SDA=6, SCL=7)
   **Then** it initializes the 128x32 SSD1306 display via I2C
   **And** I2C communication is established successfully

2. **Given** OLEDDisplay is active
   **When** methods like `drawMenu`, `showStatus` are called
   **Then** text output renders correctly on the physical screen
   **And** it implements all DisplayInterface methods
   **And** no memory allocation occurs at runtime

3. **Given** the system configuration
   **When** swapping SerialDisplay for OLEDDisplay
   **Then** the system works without changing business logic
   **And** all existing display calls work correctly

4. **Given** the display is initialized
   **When** I build the project
   **Then** build succeeds with no errors or warnings

5. **Given** the display is rendering
   **When** multiple display requests are queued
   **Then** DisplayTask arbitrates requests correctly
   **And** no race conditions occur

## Tasks

- [ ] **Task 1: Audit DisplayInterface** (AC: 2, 3)
  - [ ] Review `src/Display/Interface/DisplayInterface.h` for method signatures
  - [ ] Document all required methods: `init()`, `clear()`, `drawMenu()`, `showStatus()`, etc.
  - [ ] Identify any display-specific parameters or structures
  - [ ] Document DisplayRequest structure from `DisplayRequest.h`

- [ ] **Task 2: Add Display Library to platformio.ini** (AC: 1, 4)
  - [ ] Choose library: `Adafruit_SSD1306` (recommended for simplicity) or `U8g2`
  - [ ] Add library to `platformio.ini`:
    ```ini
    lib_deps =
        Adafruit SSD1306 @ ^2.5.15
        Adafruit GFX Library @ ^1.11.9
    ```
  - [ ] Verify library version compatibility with ESP32-C3

- [ ] **Task 3: Create OLEDDisplay Class Header** (AC: 2)
  - [ ] Create `src/Display/Impl/OLEDDisplay.h`:
    - [ ] Inherit from `DisplayInterface`
    - [ ] Include library headers: `Adafruit_SSD1306.h`, `Adafruit_GFX.h`
    - [ ] Add private members:
      - [ ] `Adafruit_SSD1306 display` instance
      - [ ] `bool displayOn` state flag
      - [ ] `uint8_t sdaPin`, `sclPin` for I2C pins
    - [ ] Declare constructor: `OLEDDisplay(uint8_t sda, uint8_t scl)`
    - [ ] Override all DisplayInterface methods

- [ ] **Task 4: Implement OLEDDisplay Class** (AC: 1, 2)
  - [ ] Create `src/Display/Impl/OLEDDisplay.cpp`:
    - [ ] Implement `init()`:
      - [ ] Initialize Wire with I2C pins (SDA=6, SCL=7)
      - [ ] Initialize SSD1306 with `display.begin(SSD1306_SWITCHCAPVCC, 0x3C)`
      - [ ] Set display size: `display.displaySize(128, 32)`
      - [ ] Clear display: `display.clearDisplay()`
      - [ ] Set text properties: `display.setTextSize(1)`, `display.setTextColor(SSD1306_WHITE)`
      - [ ] Log initialization with `LOG_INFO`
    - [ ] Implement `clear()`:
      - [ ] Call `display.clearDisplay()`
      - [ ] Call `display.display()` to update screen
    - [ ] Implement `drawMenu()`:
      - [ ] Parse DisplayRequest menu data
      - [ ] Render menu items to screen
      - [ ] Highlight selected item
      - [ ] Call `display.display()` to update
    - [ ] Implement `showStatus()`:
      - [ ] Parse DisplayRequest status data
      - [ ] Render status text to screen
      - [ ] Call `display.display()` to update
    - [ ] Implement `setPower()` (for Story 9.5):
      - [ ] Call `display.ssd1306_command(SSD1306_DISPLAYOFF)` or `SSD1306_DISPLAYON`
      - [ ] Update `displayOn` state flag

- [ ] **Task 5: Integrate OLEDDisplay in Main** (AC: 3)
  - [ ] Update `src/main.cpp`:
    - [ ] Replace `SerialDisplay` instantiation with `OLEDDisplay(6, 7)`
    - [ ] Pass OLEDDisplay instance to `DisplayTask`
    - [ ] Ensure initialization order: Wire → OLEDDisplay → DisplayTask

- [ ] **Task 6: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Verify display initializes and shows content
  - [ ] Manual test: Verify menu navigation works on OLED
  - [ ] Manual test: Verify status messages display correctly

## Dev Notes

### Architecture Compliance

- **Interface Implementation**: Must strictly follow `DisplayInterface` contract
- **I2C Config**: Use pins 6 (SDA) and 7 (SCL) for ESP32-C3 Super Mini
- **No Dynamic Allocation**: All display buffers must be static or stack-allocated
- **Display Arbitration**: Use `DisplayRequestQueue` for thread-safe display updates

### I2C Pin Configuration

```cpp
// ESP32-C3 Super Mini I2C pins
constexpr uint8_t OLED_SDA_PIN = 6;
constexpr uint8_t OLED_SCL_PIN = 7;
constexpr uint32_t I2C_FREQUENCY = 400000;  // 400kHz
```

### DisplayInterface Reference

```cpp
// From src/Display/Interface/DisplayInterface.h
class DisplayInterface {
public:
    virtual ~DisplayInterface() = default;
    virtual Error init() = 0;
    virtual void clear() = 0;
    virtual void drawMenu(const DisplayRequest& request) = 0;
    virtual void showStatus(const DisplayRequest& request) = 0;
    virtual void setPower(bool on) = 0;  // For Story 9.5
};
```

### Library Choice Analysis

| Library | Pros | Cons | Recommendation |
|----------|-------|-------|----------------|
| Adafruit SSD1306 | Lightweight, well-documented, simple API | Limited font options | **Recommended for MVP** |
| U8g2 | Powerful, many fonts, flexible | Heavier, more complex | Use if advanced features needed |

### OLEDDisplay Class Structure

```cpp
// src/Display/Impl/OLEDDisplay.h
#pragma once
#include "Display/Interface/DisplayInterface.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

class OLEDDisplay : public DisplayInterface {
private:
    Adafruit_SSD1306 display;
    bool displayOn;
    uint8_t sdaPin;
    uint8_t sclPin;

public:
    OLEDDisplay(uint8_t sda, uint8_t scl);
    Error init() override;
    void clear() override;
    void drawMenu(const DisplayRequest& request) override;
    void showStatus(const DisplayRequest& request) override;
    void setPower(bool on) override;
};
```

### Initialization Pattern

```cpp
// src/Display/Impl/OLEDDisplay.cpp
Error OLEDDisplay::init() {
    // Initialize I2C
    Wire.begin(sdaPin, sclPin, I2C_FREQUENCY);

    // Initialize display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        LOG_ERROR("OLED", "SSD1306 init failed");
        return Error::INVALID_STATE;
    }

    // Configure display
    display.displaySize(128, 32);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.cp437(true);

    // Show initial blank screen
    display.display();

    displayOn = true;
    LOG_INFO("OLED", "Initialized 128x32 SSD1306");
    return Error::OK;
}
```

### Existing Code Locations

```
src/Display/Interface/DisplayInterface.h  - Interface to implement
src/Display/Impl/SerialDisplay.cpp       - Reference implementation
src/Display/Task/DisplayTask.cpp          - Display arbitration
src/main.cpp                              - Display instantiation
```

### Key Files to Modify

| File | Change |
|------|--------|
| `platformio.ini` | Add Adafruit SSD1306 library dependency |
| `src/Display/Impl/OLEDDisplay.h` | New display class header |
| `src/Display/Impl/OLEDDisplay.cpp` | New display class implementation |
| `src/main.cpp` | Switch from SerialDisplay to OLEDDisplay |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **Initialization Test**: Verify display initializes on boot
3. **Menu Rendering Test**: Navigate menu, verify items display correctly
4. **Status Display Test**: Trigger status updates, verify they display
5. **Power Control Test**: Test `setPower()` method (for Story 9.5)
6. **Arbitration Test**: Send multiple display requests, verify no corruption

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Dynamic allocation
char* buffer = new char[32];  // Never do this in embedded

// ❌ WRONG - Wrong I2C pins
Wire.begin(21, 22);  // Wrong pins for ESP32-C3 Super Mini

// ❌ WRONG - Missing display() call
display.println("Hello");  // Won't show without display.display()

// ❌ WRONG - Blocking I2C operations
while (!display.begin()) {
    delay(100);  // Blocks entire system
}

// ❌ WRONG - Direct hardware access bypassing DisplayTask
display.clearDisplay();
display.display();  // Should go through DisplayRequestQueue

// ✅ CORRECT - Static allocation, correct pins, proper flow
char buffer[32];  // Stack allocation
Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN, I2C_FREQUENCY);

void OLEDDisplay::drawMenu(const DisplayRequest& request) {
    display.clearDisplay();
    // ... render menu items ...
    display.display();  // Always call display() to update
}
```

### Display Request Flow

```
Handler (Menu/System)
    ↓
Create DisplayRequest
    ↓
xQueueSend(displayQueue, &request, timeout)
    ↓
DisplayTask (FreeRTOS task)
    ↓
OLEDDisplay::drawMenu/showStatus()
    ↓
Adafruit_SSD1306 rendering
    ↓
display.display() → Update screen
```

### References

- [Source: architecture/core-architectural-decisions.md#Display Arbitration Pattern] - Display request queue usage
- [Source: architecture/implementation-patterns-consistency-rules.md#Structure Patterns] - Directory organization
- [Source: project-context.md#Hardware Constraints] - I2C pin configuration
- [Source: epics.md#Story 9.1] - Original acceptance criteria

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
