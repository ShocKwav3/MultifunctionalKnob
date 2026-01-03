---
stepsCompleted: [1, 2, 3, 4]
inputDocuments: ['prd.md', 'architecture/index.md', 'requirement-next.txt', 'brainstorming-session-2026-01-01.md']
workflowType: 'create-epics-and-stories'
projectType: 'brownfield-enhancement'
iterationContext: 'Iteration 2 - 8 new features extending completed Epics 1-5'
---

# UtilityButtonsWithKnobUSB - Epic Breakdown (Iteration 2)

## Overview

This document provides the complete epic and story breakdown for UtilityButtonsWithKnobUSB Iteration 2, decomposing 8 NEW features from the PRD into implementable stories. This iteration builds upon the successfully completed Epics 1-5 from the previous iteration.

## Requirements Inventory

### Functional Requirements

**Feature 1 - Device Identity:**
- FR1: System references device Bluetooth name from single source (`device_config.h`)
- FR2: NVS namespace derives from device name constant (no hardcoded "knobkoky")

**Feature 2 - Button Naming:**
- FR3: Buttons use position-based names: Top Left, Top Right, Bottom Left, Bottom Right
- FR4: Button names defined in single source (`button_config.h`), referenced elsewhere (no duplication)

**Feature 3 - Button Input Review:**
- FR5: System handles button input efficiently (investigate poll() vs event-driven, simplicity wins)

**Feature 4 - Wheel Direction:**
- FR6: "Wheel Behavior" menu restructured with sub-items: Wheel Mode, Wheel Direction
- FR7: User can set wheel direction (Normal/Reversed) via menu
- FR8: System persists wheel direction to NVS
- FR9: System applies direction setting to encoder input processing

**Feature 5 - Bluetooth Control:**
- FR10: User can initiate Bluetooth pairing from menu (disconnect first if connected, then advertise)
- FR11: User can disconnect current Bluetooth connection from menu
- FR12: Display shows Bluetooth status feedback (pairing, connected, disconnected)

**Feature 6 - Physical Display (128x32 OLED):**
- FR13: System supports 128x32 SSD1306 I2C OLED hardware
- FR14: Normal mode displays status icons: BT status, wheel direction, battery %, wheel mode (Z/V/S)
- FR15: Menu mode displays small status icons on top, menu content below
- FR16: BT icon behavior: solid (connected), flashing (pairing), none (disconnected)
- FR17: User can turn off display from menu
- FR18: Display wakes on long-press wheel button (menu activation)

**Feature 7 - Deep Sleep:**
- FR19: System enters deep sleep after 5 minutes of inactivity
- FR20: System displays warning notification at 4 minutes (1 minute before sleep)
- FR21: User input resets inactivity timer
- FR22: Wake source is wheel button ONLY (BT connection does NOT prevent sleep)

**Feature 8 - LED Strip Control:**
- FR23: User can toggle LED strip power (On/Off) via menu
- FR24: User can adjust LED brightness via wheel-controlled percentage
- FR25: User can select LED mode via menu (modes TBD at implementation)
- FR26: User can select LED color via menu (colors TBD at implementation)
- FR27: System persists all LED settings to NVS
- FR28: Brightness changes show real-time preview on LED strip

### Non-Functional Requirements

**Performance:**
- NFR1: Encoder input response < 50ms (no perceptible lag)
- NFR2: Menu navigation feels instant (< 100ms feedback)
- NFR3: BLE HID commands sent within 20ms of input

**Reliability:**
- NFR4: NVS settings persist across 10,000+ power cycles
- NFR5: Device operates continuously for 8+ hours without requiring restart
- NFR6: BLE reconnection completes within 5 seconds of wake

**Resource Constraints:**
- NFR7: Firmware fits within ESP32-C3 flash (4MB)
- NFR8: RAM usage stays within safe limits (< 300KB of 400KB)
- NFR9: No heap fragmentation from dynamic allocation (use static allocation)

**Usability:**
- NFR10: Menu system navigable without documentation
- NFR11: All settings accessible within 3 menu levels

### Additional Requirements

**From Architecture - Technical Constraints:**
- Static menu tree with parent pointers (no dynamic allocation, compile-time menu definition)
- Event interceptor pattern for menu activation (menu intercepts events at higher level)
- Command pattern for action execution (MenuAction base class with execute() method)
- NVS persistence using single namespace with prefixed keys
- Enum storage with validation wrapper for graceful fallback on corruption
- Display arbitration via DisplayTask to prevent race conditions

**From Architecture - Implementation Requirements:**
- Display abstraction (DisplayInterface) with SerialDisplay and OLEDDisplay implementations
- Button configuration via constexpr array of ButtonConfig structs
- Factory reset capability (encoder button held 5+ seconds during boot clears NVS)
- BLE disconnect handling - exit menu to root and show "Disconnected" status
- DI-enabled ConfigManager accepting Preferences* for testability
- LOG_* macros (LOG_ERROR, LOG_INFO, LOG_DEBUG) for all logging - no direct Serial.print
- Error enum return from all fallible operations (OK, INVALID_PARAM, NVS_READ_FAIL, NVS_WRITE_FAIL, INVALID_STATE)

**From Brainstorming - Dependencies:**
- Feature 7 (Deep Sleep) depends on Feature 6 (Physical Display) for warning notification
- Feature 5 (Bluetooth Menu) benefits from menu patterns established in Feature 4
- Features 1-3 are quick wins with no dependencies

### FR Coverage Map

| FR | Epic | Description |
|----|------|-------------|
| FR1 | Epic 6 | Device name from single source |
| FR2 | Epic 6 | NVS namespace from device name |
| FR3 | Epic 6 | Position-based button names |
| FR4 | Epic 6 | Button names single source |
| FR5 | Epic 6 | Button input efficiency review |
| FR6 | Epic 7 | Wheel Behavior menu restructure |
| FR7 | Epic 7 | Wheel direction menu option |
| FR8 | Epic 7 | Wheel direction NVS persistence |
| FR9 | Epic 7 | Apply direction to encoder |
| FR10 | Epic 8 | BT pairing from menu |
| FR11 | Epic 8 | BT disconnect from menu |
| FR12 | Epic 8 | BT status display feedback |
| FR13 | Epic 9 | 128x32 OLED hardware support |
| FR14 | Epic 9 | Normal mode status icons |
| FR15 | Epic 9 | Menu mode dual display |
| FR16 | Epic 9 | BT icon states |
| FR17 | Epic 9 | Display off from menu |
| FR18 | Epic 9 | Display wake on menu activation |
| FR19 | Epic 10 | Deep sleep after 5min |
| FR20 | Epic 10 | Sleep warning at 4min |
| FR21 | Epic 10 | Input resets idle timer |
| FR22 | Epic 10 | Wake via wheel button only |
| FR23 | Epic 11 | LED power toggle |
| FR24 | Epic 11 | LED brightness wheel control |
| FR25 | Epic 11 | LED mode selection |
| FR26 | Epic 11 | LED color selection |
| FR27 | Epic 11 | LED settings NVS persistence |
| FR28 | Epic 11 | Brightness live preview |

## Epic List

### Epic 6: Codebase Cleanup & Consolidation
User can trust that device identity and button configuration come from single, consistent sources.

**FRs covered:** FR1, FR2, FR3, FR4, FR5

**User Outcome:** Device name and button names are consistent everywhere, code is cleaner, and button input handling is optimized.

**Implementation Notes:** Quick wins with no dependencies. Sets foundation for clean implementation of remaining features.

---

### Epic 7: Wheel Direction Control
User can reverse wheel rotation direction via the menu and have the setting persist.

**FRs covered:** FR6, FR7, FR8, FR9

**User Outcome:** User navigates to Wheel Behavior → Wheel Direction → Reversed, and the encoder now scrolls/zooms/adjusts in the opposite direction. Setting survives power cycles.

**Implementation Notes:** Menu restructure (Wheel Behavior becomes parent with Mode + Direction children). Builds on existing menu patterns.

---

### Epic 8: Bluetooth Menu Control
User can manage Bluetooth pairing and connections directly from the device menu.

**FRs covered:** FR10, FR11, FR12

**User Outcome:** User can enter pairing mode or disconnect from current device without touching the host machine. Status feedback confirms actions.

**Implementation Notes:** New top-level "Bluetooth" menu item with Pair/Disconnect sub-items.

---

### Epic 9: Physical OLED Display
User sees device status at a glance on the physical 128x32 OLED screen with intuitive icons.

**FRs covered:** FR13, FR14, FR15, FR16, FR17, FR18

**User Outcome:** OLED shows BT status (solid/flashing/none), wheel mode (S/V/Z), wheel direction indicator, battery %. Menu mode shows status bar + menu content. User can turn off display and wake it via menu activation.

**Implementation Notes:** Highest complexity epic. Requires OLEDDisplay implementation of DisplayInterface.

---

### Epic 10: Power Management & Deep Sleep
User's device automatically sleeps after inactivity to save power, with a warning before sleep.

**FRs covered:** FR19, FR20, FR21, FR22

**User Outcome:** After 4 minutes idle, user sees "Sleep in 1 min" warning. After 5 minutes, device sleeps. Pressing wheel button wakes device. Any input resets the timer.

**Implementation Notes:** Depends on Epic 9 for display warning. Wake source is wheel button only.

---

### Epic 11: LED Strip Control
User can control an external LED strip (power, brightness, mode, color) via the menu.

**FRs covered:** FR23, FR24, FR25, FR26, FR27, FR28

**User Outcome:** User navigates to LED menu, toggles power, adjusts brightness with wheel (seeing live preview), selects modes and colors. All settings persist to NVS.

**Implementation Notes:** Requires FastLED library. Independent of other epics but benefits from menu patterns.

---

## Epic Summary

| Epic | Title | FRs | Complexity | Dependencies |
|------|-------|-----|------------|--------------|
| 6 | Codebase Cleanup & Consolidation | FR1-FR5 | Low | None |
| 7 | Wheel Direction Control | FR6-FR9 | Medium | None (extends existing menu) |
| 8 | Bluetooth Menu Control | FR10-FR12 | Medium | None |
| 9 | Physical OLED Display | FR13-FR18 | High | None |
| 10 | Power Management & Deep Sleep | FR19-FR22 | Medium | Epic 9 (for warning display) |
| 11 | LED Strip Control | FR23-FR28 | Medium-High | None |

**Total: 6 new epics (Epic 6-11), 28 FRs covered**

---

## Epic 6: Codebase Cleanup & Consolidation

**Goal:** User can trust that device identity and button configuration come from single, consistent sources.

**FRs covered:** FR1, FR2, FR3, FR4, FR5

---

### Story 6.1: Consolidate Device Name as Single Source of Truth

As a **developer maintaining the codebase**,
I want **the device Bluetooth name defined in one place (`device_config.h`) and referenced everywhere else**,
So that **I can change the device identity in a single location without hunting for duplicates**.

**Acceptance Criteria:**

**Given** the device name is defined in `include/Config/device_config.h`
**When** the NVS namespace is initialized in `ConfigManager`
**Then** it derives the namespace from the device name constant (not hardcoded "knobkoky")
**And** no other file contains a hardcoded device name string

---

### Story 6.2: Consolidate Button Names with Position-Based Identifiers

As a **user configuring buttons via the menu**,
I want **buttons labeled by position (Top Left, Top Right, Bottom Left, Bottom Right)**,
So that **I can easily identify which physical button I'm configuring**.

**Acceptance Criteria:**

**Given** button configuration exists in `include/Config/button_config.h`
**When** I view the button list in the menu
**Then** I see position-based names: "Top Left", "Top Right", "Bottom Left", "Bottom Right"
**And** `MenuTree.h` references `button_config.h` for names (no duplication)
**And** the mapping matches the physical button layout on the device

---

### Story 6.3: Evaluate and Optimize Button Input Handling

As a **developer maintaining the codebase**,
I want **button input handling reviewed for efficiency**,
So that **the code uses the simplest approach that works reliably**.

**Acceptance Criteria:**

**Given** buttons currently use `buttonManager->poll()` in `main.cpp::loop()`
**When** I investigate whether event-driven input (like the wheel) is simpler
**Then** I document the finding with a clear recommendation
**And** if event-driven is simpler → implement it
**And** if poll() is simpler → keep it and document why
**And** the decision prioritizes simplicity over forced consistency

---

## Epic 7: Wheel Direction Control

**Goal:** User can reverse wheel rotation direction via the menu and have the setting persist.

**FRs covered:** FR6, FR7, FR8, FR9

---

### Story 7.1: Restructure Wheel Behavior Menu with Sub-Items

As a **user accessing wheel settings**,
I want **Wheel Behavior to be a parent menu with Wheel Mode and Wheel Direction as sub-items**,
So that **I can access both mode and direction settings in an organized hierarchy**.

**Acceptance Criteria:**

**Given** I am in the main menu
**When** I select "Wheel Behavior"
**Then** I see two sub-items: "Wheel Mode" and "Wheel Direction"
**And** selecting "Wheel Mode" shows the existing options (Scroll, Volume, Zoom)
**And** I can navigate back to the Wheel Behavior parent menu

---

### Story 7.2: Add Wheel Direction Setting with NVS Persistence

As a **user who prefers inverted scroll direction**,
I want **to select Normal or Reversed wheel direction from the menu**,
So that **the wheel rotates in my preferred direction and the setting persists**.

**Acceptance Criteria:**

**Given** I navigate to Wheel Behavior → Wheel Direction
**When** I select "Reversed" (or "Normal")
**Then** the selection is saved to NVS with key "wheel.dir"
**And** I see confirmation feedback ("Direction saved" or similar)
**And** the menu exits or returns to parent after selection
**And** after power cycle, the direction setting is restored from NVS

---

### Story 7.3: Apply Wheel Direction to Encoder Input Processing

As a **user with reversed wheel direction configured**,
I want **the encoder rotation to respect my direction setting**,
So that **rotating clockwise scrolls/zooms/adjusts in my preferred direction**.

**Acceptance Criteria:**

**Given** wheel direction is set to "Reversed" in NVS
**When** I rotate the encoder clockwise
**Then** the system sends the opposite HID command (e.g., scroll down instead of up)
**And** this applies to all wheel modes (Scroll, Volume, Zoom)
**And** when direction is "Normal", encoder behaves as it does today

---

## Epic 8: Bluetooth Menu Control

**Goal:** User can manage Bluetooth pairing and connections directly from the device menu.

**FRs covered:** FR10, FR11, FR12

---

### Story 8.1: Add Bluetooth Menu with Pair and Disconnect Options

As a **user managing Bluetooth connections**,
I want **a Bluetooth menu item with Pair and Disconnect sub-options**,
So that **I can control BLE connectivity directly from the device**.

**Acceptance Criteria:**

**Given** I am in the main menu
**When** I select "Bluetooth"
**Then** I see two sub-items: "Pair" and "Disconnect"
**And** I can navigate back to the main menu

---

### Story 8.2: Implement Bluetooth Pairing Action

As a **user wanting to pair with a new device**,
I want **to initiate Bluetooth pairing from the menu**,
So that **I can connect to a new host without physical access to it**.

**Acceptance Criteria:**

**Given** I navigate to Bluetooth → Pair
**When** I select "Pair"
**Then** if currently connected, the system disconnects first
**And** the system starts BLE advertising for pairing
**And** the display shows "Pairing..." status feedback
**And** when a device connects, the display shows "Connected"
**And** the menu exits after successful pairing or timeout

---

### Story 8.3: Implement Bluetooth Disconnect Action

As a **user wanting to disconnect from the current host**,
I want **to disconnect Bluetooth from the menu**,
So that **I can release the connection without turning off the device**.

**Acceptance Criteria:**

**Given** I navigate to Bluetooth → Disconnect
**When** I select "Disconnect"
**Then** the system disconnects the current BLE connection
**And** the display shows "Disconnected" status feedback
**And** the menu returns to parent or exits
**And** if not connected, the action shows "Not connected" feedback

---

## Epic 9: Physical OLED Display

**Goal:** User sees device status at a glance on the physical 128x32 OLED screen with intuitive icons.

**FRs covered:** FR13, FR14, FR15, FR16, FR17, FR18

---

### Story 9.1: Implement OLEDDisplay Driver for 128x32 SSD1306

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

### Story 9.2: Create Normal Mode Status Screen with Icons

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

### Story 9.3: Implement Bluetooth Icon States (Solid/Flashing/None)

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

### Story 9.4: Create Menu Mode Display with Status Bar

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

### Story 9.5: Add Display Power Control Menu Option

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

### Story 9.6: Implement Display Wake on Menu Activation

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

## Epic 10: Power Management & Deep Sleep

**Goal:** User's device automatically sleeps after inactivity to save power, with a warning before sleep.

**FRs covered:** FR19, FR20, FR21, FR22, FR12 (PRD)

**Dependency:** Epic 9 (for display warning)

---

### Story 10.1: Implement Inactivity Timer with Input Reset

As a **user actively using the device**,
I want **any input (wheel rotation, button press) to reset the inactivity timer**,
So that **the device stays awake while I'm using it**.

**Acceptance Criteria:**

**Given** an inactivity timer is running
**When** I rotate the wheel or press any button
**Then** the inactivity timer resets to 0
**And** the 5-minute countdown starts fresh
**And** BLE connection status does NOT affect the timer

---

### Story 10.2: Display Sleep Warning at 4 Minutes Idle

As a **user who has been idle**,
I want **to see a warning 1 minute before the device sleeps**,
So that **I can interact to prevent sleep if needed**.

**Acceptance Criteria:**

**Given** the inactivity timer reaches 4 minutes
**When** the warning triggers
**Then** the display shows "Sleep in 1 min" (or similar warning)
**And** the warning remains visible until input or sleep
**And** any user input dismisses the warning and resets the timer

---

### Story 10.3: Enter Deep Sleep After 5 Minutes Inactivity

As a **user who has left the device idle**,
I want **the device to enter deep sleep after 5 minutes**,
So that **power is conserved when not in use**.

**Acceptance Criteria:**

**Given** the inactivity timer reaches 5 minutes with no input
**When** the timeout occurs
**Then** the ESP32 enters deep sleep mode
**And** the display turns off
**And** BLE disconnects (if connected)
**And** minimum power is consumed in sleep state

---

### Story 10.4: Configure Wheel Button as Wake Source

As a **user wanting to wake a sleeping device**,
I want **to press the wheel button to wake the device from deep sleep**,
So that **I can resume using it without unplugging/replugging**.

**Acceptance Criteria:**

**Given** the device is in deep sleep
**When** I press the wheel button
**Then** the device wakes from deep sleep
**And** the system initializes normally
**And** BLE begins advertising/reconnecting
**And** the display turns on showing normal status

---

### Story 10.5: Auto-Reconnect Bluetooth After Wake

As a **user waking the device from sleep**,
I want **the device to automatically reconnect to the previously paired Bluetooth host**,
So that **I can resume using it without manually re-pairing**.

**Acceptance Criteria:**

**Given** the device was connected to a BLE host before entering deep sleep
**When** the device wakes from deep sleep
**Then** the system automatically attempts to reconnect to the last paired device
**And** BLE reconnection completes within 5 seconds (per NFR6)
**And** the display shows connection status during reconnection
**And** if reconnection fails, the device enters advertising/pairing mode

---

## Epic 11: LED Strip Control

**Goal:** User can control an external LED strip (power, brightness, mode, color) via the menu.

**FRs covered:** FR23, FR24, FR25, FR26, FR27, FR28

---

### Story 11.1: Add FastLED Library and LED Menu Structure

As a **user wanting to control LED strips**,
I want **an LED menu item with Power, Brightness, Mode, and Color sub-options**,
So that **I can access all LED settings from one place**.

**Acceptance Criteria:**

**Given** FastLED library is added to `platformio.ini`
**When** I navigate to the main menu
**Then** I see an "LED" menu item
**And** selecting it shows sub-items: "Power", "Brightness", "Mode", "Color"
**And** I can navigate back to the main menu

---

### Story 11.2: Implement LED Power Toggle with NVS Persistence

As a **user controlling the LED strip**,
I want **to toggle LED power On/Off from the menu**,
So that **I can turn the strip on or off and have it remember my choice**.

**Acceptance Criteria:**

**Given** I navigate to LED → Power
**When** I select "On" or "Off"
**Then** the LED strip turns on or off immediately
**And** the setting is saved to NVS
**And** after power cycle, the LED power state is restored

---

### Story 11.3: Implement Wheel-Controlled Brightness with Live Preview

As a **user adjusting LED brightness**,
I want **to use the wheel to change brightness and see the result in real-time**,
So that **I can find the perfect brightness level visually**.

**Acceptance Criteria:**

**Given** I navigate to LED → Brightness
**When** I rotate the wheel
**Then** the brightness percentage changes (0-100%)
**And** the display shows the current percentage
**And** the LED strip updates brightness in real-time (live preview)
**When** I press the wheel button to confirm
**Then** the brightness is saved to NVS
**And** the menu exits or returns to parent

---

### Story 11.4: Implement LED Mode Selection with NVS Persistence

As a **user wanting different LED effects**,
I want **to select an LED mode from a list**,
So that **the strip displays my preferred pattern**.

**Acceptance Criteria:**

**Given** I navigate to LED → Mode
**When** I see the list of available modes
**Then** I can scroll through modes using the wheel
**When** I select a mode
**Then** the LED strip applies the mode immediately
**And** the selection is saved to NVS
**And** after power cycle, the mode is restored

---

### Story 11.5: Implement LED Color Selection with NVS Persistence

As a **user wanting to customize LED color**,
I want **to select a color from a list**,
So that **the strip displays my preferred color**.

**Acceptance Criteria:**

**Given** I navigate to LED → Color
**When** I see the list of available colors
**Then** I can scroll through colors using the wheel
**When** I select a color
**Then** the LED strip applies the color immediately
**And** the selection is saved to NVS
**And** after power cycle, the color is restored

---

## Story Summary

| Epic | Title | Stories | FRs Covered |
|------|-------|---------|-------------|
| 6 | Codebase Cleanup & Consolidation | 3 | FR1-FR5 |
| 7 | Wheel Direction Control | 3 | FR6-FR9 |
| 8 | Bluetooth Menu Control | 3 | FR10-FR12 |
| 9 | Physical OLED Display | 6 | FR13-FR18 |
| 10 | Power Management & Deep Sleep | 5 | FR19-FR22, FR12 (PRD) |
| 11 | LED Strip Control | 5 | FR23-FR28 |

**Total: 6 Epics, 25 Stories, 29 FRs covered**
