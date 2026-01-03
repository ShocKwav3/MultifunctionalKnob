---
epicNumber: 9
title: "Physical OLED Display (128x32 SSD1306)"
complexity: High
dependencies: None
frsCovered: "FR13, FR14, FR15, FR16, FR17, FR18"
stories: 6
---

# Epic 9: Physical OLED Display

**Goal:** User sees device status at a glance on the physical 128x32 OLED screen with intuitive icons.

**FRs covered:** FR13, FR14, FR15, FR16, FR17, FR18

**User Outcome:** OLED shows BT status (solid/flashing/none), wheel mode (S/V/Z), wheel direction indicator, battery %. Menu mode shows status bar + menu content. User can turn off display and wake it via menu activation.

**Implementation Notes:** Highest complexity epic. Requires OLEDDisplay implementation of DisplayInterface.

---

## Functional Requirements

**Feature 6 - Physical Display (128x32 OLED):**
- FR13: System supports 128x32 SSD1306 I2C OLED hardware
- FR14: Normal mode displays status icons: BT status, wheel direction, battery %, wheel mode (Z/V/S)
- FR15: Menu mode displays small status icons on top, menu content below
- FR16: BT icon behavior: solid (connected), flashing (pairing), none (disconnected)
- FR17: User can turn off display from menu
- FR18: Display wakes on long-press wheel button (menu activation)

---

## Story 9.1: Implement OLEDDisplay Driver for 128x32 SSD1306

As a **developer integrating the physical display**,
I want **an OLEDDisplay class implementing DisplayInterface for the 128x32 SSD1306**,
So that **the existing display abstraction works with the physical hardware**.

**Acceptance Criteria:**

**Given** the DisplayInterface abstraction exists
**When** OLEDDisplay is instantiated with I2C pins (SDA=6, SCL=7)
**Then** it initializes the 128x32 SSD1306 display via I2C
**And** it implements all DisplayInterface methods
**And** text output renders correctly on the physical screen
**And** the display can be swapped in place of SerialDisplay

---

## Story 9.2: Create Normal Mode Status Screen with Icons

As a **user glancing at the device**,
I want **to see status icons on the OLED showing BT status, wheel mode, direction, and battery**,
So that **I know the device state without entering the menu**.

**Acceptance Criteria:**

**Given** the device is in normal mode (not in menu)
**When** I look at the OLED display
**Then** I see the current wheel mode indicator (S/V/Z)
**And** I see the wheel direction indicator
**And** I see battery percentage (placeholder value for now)
**And** the layout fits the 128x32 pixel constraint
**And** icons are clear and distinguishable

---

## Story 9.3: Implement Bluetooth Icon States (Solid/Flashing/None)

As a **user checking Bluetooth connectivity**,
I want **the BT icon to indicate connection state visually**,
So that **I can tell at a glance if I'm connected, pairing, or disconnected**.

**Acceptance Criteria:**

**Given** the device is in normal mode
**When** Bluetooth is connected
**Then** the BT icon displays solid (static)
**When** Bluetooth is in pairing mode
**Then** the BT icon flashes (blinks on/off)
**When** Bluetooth is disconnected and not pairing
**Then** no BT icon is displayed

---

## Story 9.4: Create Menu Mode Display with Status Bar

As a **user navigating the menu**,
I want **the OLED to show a small status bar on top and menu content below**,
So that **I can see important status while browsing settings**.

**Acceptance Criteria:**

**Given** the menu is active
**When** the display renders menu content
**Then** a small status bar appears at the top (BT icon, mode indicator)
**And** menu items display below the status bar
**And** the currently selected item is visually highlighted
**And** the layout fits the 128x32 pixel constraint

---

## Story 9.5: Add Display Power Control Menu Option

As a **user wanting to save power or reduce distraction**,
I want **to turn off the display from the menu**,
So that **the screen goes dark when I don't need it**.

**Acceptance Criteria:**

**Given** I am in the menu
**When** I navigate to a "Display Off" option and select it
**Then** the OLED display turns off (blank/power-save mode)
**And** the device continues to function normally
**And** long-pressing the wheel button wakes the display (and opens menu)

---

## Story 9.6: Implement Display Wake on Menu Activation

As a **user with the display turned off**,
I want **the display to wake when I long-press the wheel button**,
So that **I can see the menu and device status again**.

**Acceptance Criteria:**

**Given** the display is off
**When** I long-press the wheel button
**Then** the display wakes up
**And** the menu opens simultaneously
**And** subsequent inputs work normally

---

## Implementation Checklist

- [ ] Add U8g2lib or Adafruit_SSD1306 library to platformio.ini
- [ ] Create `OLEDDisplay` class implementing `DisplayInterface`
- [ ] Implement I2C initialization (SDA=6, SCL=7)
- [ ] Implement all DisplayInterface methods for OLED
- [ ] Create icon/glyph definitions for BT, modes, direction
- [ ] Implement normal mode status screen layout
- [ ] Add menu mode status bar + content layout
- [ ] Implement BT icon flashing logic for pairing state
- [ ] Add display power control to menu
- [ ] Implement display power save mode
- [ ] Implement display wake trigger on wheel button long-press
- [ ] Test status screen updates with state changes
- [ ] Test menu mode display with various menu depths
- [ ] Test icon clarity and visibility
- [ ] Test display power cycling

---

## Technical Notes

- Use double-buffering to prevent flicker during updates
- Implement flashing via timer-based state toggle (50-100ms blink period)
- Status bar: Consider 8-10px height to leave room for menu items
- Normal layout: 128px wide × 32px tall constraints are tight
- Consider using bitmap icons for efficiency
- Display updates should be non-blocking (use DisplayTask)
- Cache display state to minimize unnecessary I2C writes

---

## Related Files

- `src/Display/Impl/OLEDDisplay.cpp/.h` - Create OLED implementation
- `src/Display/Interface/DisplayInterface.h` - Ensure interface is complete
- `src/Menu/Model/MenuTree.h` - Add "Display Off" menu option
- `src/Button/ButtonManager.h` - Integrate display wake trigger
- `platformio.ini` - Add OLED library dependency

---

## Hardware Specifications

- Display: 128x32 OLED SSD1306
- Interface: I2C
- SDA Pin: GPIO6
- SCL Pin: GPIO7
- I2C Address: 0x3C (typical, verify with device)
