# Story 13.2: Menu-Configurable Display Timeout

**Epic:** 13 - Configurable Power & Display Management
**Status:** ready-for-dev
**Complexity:** Medium
**Created:** 2026-01-29

---

## User Story

As a **user who wants to save OLED power without sleeping the device**,
I want **to configure the display auto-off timeout through the menu**,
So that **the display turns off during inactivity while the device remains operational**.

---

## Scope

- `src/Menu/Model/MenuTree.h` - Add "Display Timeout" setting under Settings menu
- `src/Menu/Action/SetDisplayTimeoutAction.cpp/.h` - Configuration action (NEW)
- `src/Config/ConfigManager.cpp/.h` - NVS methods for display timeout
- Display management module - Timer for OLED power control (NEW or MODIFIED)
- Input event handlers - Wake display on any activity

---

## Current Behavior

Display timeout may not exist or is hardcoded. Behavior needs verification.

---

## Desired Behavior

User can select display timeout: 30s, 60s, 120s (2min), 180s (3min), 300s (5min), or Never. Display turns off after timeout but device stays awake. Any input wakes display immediately.

---

## Architectural Context

**CRITICAL: Display timeout is INDEPENDENT of sleep timer.**

- Display timeout → OLED power off, device still awake, BLE connected
- Sleep timer → Entire device deep sleep
- Both timers run simultaneously
- User can configure: Display 30s, Sleep Never (saves OLED power, device always ready)

---

## Acceptance Criteria

### AC1: Menu Item Display
**Given** I navigate to Settings menu
**When** I select "Display Timeout"
**Then** I see options: "30 sec", "60 sec", "2 min", "3 min", "5 min", "Never"
**And** the current selection is highlighted

### AC2: Save Selection
**Given** I select a new display timeout value (e.g., "60 sec")
**When** the selection is confirmed
**Then** the value is saved to NVS
**And** the display timeout timer is updated immediately
**And** the display shows confirmation (e.g., "Display timeout: 60 sec")

### AC3: Display Power Off on Timeout
**Given** I configure display timeout to 60 seconds
**When** I remain inactive for 60 seconds
**Then** the OLED display turns off (powers down)
**And** the device remains fully operational (BLE connected, ready for inputs)

### AC4: Display Wake on Input
**Given** the display is off due to timeout
**When** I press any button or rotate the encoder
**Then** the display turns back on immediately
**And** the input is processed normally
**And** the display timeout timer resets

### AC5: Never Option
**Given** I select "Never"
**When** the selection is confirmed
**Then** the display never automatically turns off
**And** the display stays on until device sleep or manual power-off

### AC6: Independence from Sleep Timer
**Given** display timeout is 60s and sleep timer is 5 minutes
**When** I remain inactive for 60 seconds
**Then** the display turns off
**And** the device continues running
**When** I remain inactive for another 4 minutes (5 min total)
**Then** the device enters deep sleep (display already off)

### AC7: Persistence Across Reboot
**Given** I configure display timeout and reboot the device
**When** the device starts up
**Then** the configured display timeout value is loaded from NVS
**And** the timer uses the saved value

---

## Technical Implementation Notes

### NVS Storage
- **NVS Key:** `"display_timeout"` (uint16_t)
- **Value:** Seconds (30, 60, 120, 180, 300) or 0 for "Never"
- **Default:** 60 seconds (recommended)
- Add to `ConfigManager`:
  - `uint16_t loadDisplayTimeout()` - Returns seconds, defaults to 60
  - `Error saveDisplayTimeout(uint16_t seconds)` - Saves to NVS

### Display Power Management Module
- **Option A:** Create new module `src/Display/DisplayPowerManager`
- **Option B:** Extend existing display module

**Module Responsibilities:**
- Track inactivity timer (separate from sleep timer)
- Power OLED on/off via I2C commands
- Reset timer on any input activity
- Handle "Never" configuration (disable timeout)

### OLED Power Control
- Most I2C OLEDs support display on/off commands:
  ```cpp
  display.ssd1306_command(SSD1306_DISPLAYOFF); // Power off
  display.ssd1306_command(SSD1306_DISPLAYON);  // Power on
  ```
- Verify your display library API (likely Adafruit_SSD1306 or similar)
- Power off does NOT clear framebuffer - display restores on wake

### Input Event Hooks
- Hook into ALL input handlers:
  - `EncoderEventHandler` - Rotation and button press
  - `ButtonEventHandler` - All button presses
  - Menu interactions
- On any input event:
  1. If display is off, turn on immediately
  2. Reset display timeout timer
  3. Process input normally

### Timer Implementation
- Use FreeRTOS timer or existing timer mechanism
- Timer period = configured timeout value
- Timer callback: Turn off display
- Reset timer on every input event

### Menu Action
- Create `SetDisplayTimeoutAction` class
- Present 6 options with current value highlighted
- On selection:
  1. Save to NVS
  2. Update active display timeout timer
  3. Show confirmation

---

## Implementation Checklist

- [ ] Add "Display Timeout" menu item to MenuTree
  - Add under Settings submenu (suggest: Settings → Power Management)
  - Display format: "Display Timeout: [current value]"
- [ ] Create `SetDisplayTimeoutAction` class
  - Create `src/Menu/Action/SetDisplayTimeoutAction.cpp/.h`
  - Implement option selection UI (6 options)
  - Highlight current selection
  - Handle user selection
- [ ] Add NVS methods to ConfigManager
  - `uint16_t loadDisplayTimeout()` with default 60 seconds
  - `Error saveDisplayTimeout(uint16_t seconds)`
  - Use NVS key: `"display_timeout"`
- [ ] Create or extend display power management module
  - Track display on/off state
  - Track inactivity timer (separate from sleep timer)
  - Implement timer callback to turn off display
  - Implement wake function to turn on display
- [ ] Implement OLED power control
  - Research display library API for power on/off
  - Implement `displayPowerOff()` function
  - Implement `displayPowerOn()` function
  - Test that framebuffer persists (display content restored on wake)
- [ ] Hook display wake to all input events
  - Update `EncoderEventHandler`: Wake display on any encoder event
  - Update `ButtonEventHandler`: Wake display on any button event
  - Update menu system: Wake display on menu activity
  - All hooks should: 1) turn on display if off, 2) reset timer, 3) process input
- [ ] Implement "Never" option
  - Value 0 = Never
  - Disable timer if configured to 0
  - Display stays on indefinitely
- [ ] Test each timeout option
  - Test 30s: Display off after 30s inactivity
  - Test 60s: Display off after 60s inactivity
  - Test 2min: Display off after 2min inactivity
  - Test 3min: Display off after 3min inactivity
  - Test 5min: Display off after 5min inactivity
  - Test Never: Display never auto-off
- [ ] Test display wake on input activity
  - Display off → press button → verify display on + button processed
  - Display off → rotate encoder → verify display on + rotation processed
  - Display off → open menu → verify display on + menu opens
- [ ] Test independence from sleep timer
  - Configure: Display 30s, Sleep 5min
  - Wait 30s → verify display off, device awake
  - Wait 4.5min more → verify device sleeps (display already off)
- [ ] Test NVS persistence
  - Configure to 2min
  - Reboot device
  - Verify starts with 2min timeout
- [ ] Verify framebuffer persistence
  - Display showing content
  - Wait for display timeout
  - Press button to wake
  - Verify previous content restored (not blank screen)

---

## Files to Create

- `src/Display/DisplayPowerManager.cpp` (NEW - if creating separate module)
- `src/Display/DisplayPowerManager.h` (NEW - if creating separate module)
- `src/Menu/Action/SetDisplayTimeoutAction.cpp` (NEW)
- `src/Menu/Action/SetDisplayTimeoutAction.h` (NEW)

---

## Files to Modify

- `src/Menu/Model/MenuTree.h` (add menu item)
- `src/Config/ConfigManager.cpp/.h` (add NVS methods)
- `src/Event/Handler/EncoderEventHandler.cpp` (hook display wake)
- `src/Event/Handler/ButtonEventHandler.cpp` (hook display wake)
- `src/Menu/Controller/MenuController.cpp` (hook display wake if needed)
- `src/main.cpp` (initialize display timeout timer)

---

## Testing Notes

**Manual Testing:**
1. Navigate to Settings → Display Timeout
2. Select "60 sec" → verify saved
3. Remain inactive for 60 seconds → verify display turns off
4. While display off → press button → verify display turns on + button works
5. While display off → rotate encoder → verify display on + rotation works
6. Configure to "Never" → remain inactive → verify display stays on
7. Configure: Display 30s, Sleep 5min → verify display off at 30s, sleep at 5min
8. Configure to "2 min" → reboot → verify setting persists

**Power Consumption Testing (Optional):**
- Measure current draw with display on vs off
- Verify power savings with timeout enabled

**OLED Lifespan:**
- Display timeout significantly extends OLED lifespan (reduces burn-in)
- Recommend users enable this feature

---

## Definition of Done

- [ ] All acceptance criteria passing
- [ ] All checklist items completed
- [ ] Manual testing completed for all timeout values
- [ ] Display wake verified on all input types
- [ ] Independence from sleep timer verified
- [ ] NVS persistence verified
- [ ] Framebuffer persistence verified (content restored on wake)
- [ ] No new compiler warnings
- [ ] Code follows project coding standards
- [ ] Ready for code review

---

## Related Epic

See `_bmad-output/project-planning-artifacts/epics/epic-13-power-display-configuration.md` for full epic context, architectural clarification, and behavior matrix.

---
