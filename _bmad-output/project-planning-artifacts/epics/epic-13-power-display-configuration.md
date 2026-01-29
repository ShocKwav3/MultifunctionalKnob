---
epicNumber: 13
title: "Configurable Power & Display Management"
complexity: Low-Medium
dependencies: "None (uses existing menu, NVS, timer infrastructure)"
frsCovered: "FR-22, FR-23, FR-24"
nfrsCovered: "NFR-5, NFR-6"
stories: 3
sourceDocuments:
  - "Requirements gathered in conversation with user"
---

# Epic 13: Configurable Power & Display Management

**Goal:** Give users control over device power behavior by making sleep timer and display timeout independently configurable, while improving sleep UX with a live countdown.

**User Outcome:** Users can customize how long the device stays awake and how long the display stays on to match their usage patterns. Power users can disable sleep entirely while still saving OLED power with display timeout. Users get clear feedback before the device sleeps.

**Story Status:**
- Story 13.1: 📋 READY-FOR-DEV (sleep timer configuration)
- Story 13.2: 📋 READY-FOR-DEV (display timeout configuration)
- Story 13.3: 📋 READY-FOR-DEV (sleep countdown display bug fix)

---

## Functional Requirements

- FR-22: Sleep timer configurable through menu (1, 5, 10, 30, 60 minutes, Never)
- FR-23: Display timeout configurable through menu (30, 60, 120, 180, 300 seconds, Never)
- FR-24: Display live countdown before sleep (e.g., "Sleeping in 1:00", "0:59", "0:58"...)

## Non-Functional Requirements

- NFR-5: Display timeout is independent of sleep timer (display can turn off while device stays awake)
- NFR-6: Settings persist to NVS across power cycles

---

## Architectural Clarification

**Two Independent Timers:**

1. **Display Timeout Timer:**
   - Triggers OLED power down only
   - Device remains fully operational (BLE active, inputs work)
   - Any input activity wakes display back up
   - Default: TBD (likely 60 seconds)

2. **Sleep Timer:**
   - Triggers deep sleep (entire device)
   - Display turns off as part of sleep (if still on)
   - Requires button press to wake entire device
   - Current default: 5 minutes (hardcoded)

**Behavior Matrix:**

| Display Timeout | Sleep Timer | Result |
|----------------|-------------|--------|
| 60s | Never | Display turns off after 60s inactivity; device stays awake forever |
| Never | 5min | Display stays on; device sleeps after 5min |
| 30s | 5min | Display turns off after 30s; device sleeps after 5min |
| Never | Never | Display and device stay on indefinitely |

---

## Story 13.1: Menu-Configurable Sleep Timer

As a **user who wants control over sleep behavior**,
I want **to configure the inactivity sleep timer through the menu**,
So that **I can set sleep duration to match my usage pattern or disable it entirely**.

**Scope:**
- `src/Menu/Model/MenuTree.h` - Add "Sleep Timer" setting under Settings menu
- `src/Menu/Action/SetSleepTimerAction.cpp/.h` - Configuration action
- `src/Config/ConfigManager.cpp/.h` - NVS methods for sleep timer
- `src/main.cpp` or power management module - Apply configured timer value

**Current Behavior:**
- Sleep timer is hardcoded to 5 minutes

**New Behavior:**
- User selects from predefined options: 1, 5, 10, 30, 60 minutes, Never
- Selection saved to NVS
- Timer updated immediately on selection
- Default remains 5 minutes if not configured

**Acceptance Criteria:**

**Given** I navigate to Settings menu
**When** I select "Sleep Timer"
**Then** I see a list of options: "1 min", "5 min", "10 min", "30 min", "60 min", "Never"
**And** the current selection is highlighted

**Given** I select a new sleep timer value (e.g., "10 min")
**When** the selection is confirmed
**Then** the value is saved to NVS
**And** the sleep timer is updated immediately
**And** the display shows confirmation (e.g., "Sleep timer: 10 min")

**Given** I select "Never"
**When** the selection is confirmed
**Then** the device will never automatically sleep due to inactivity
**And** the value is saved to NVS
**And** only manual power-off or explicit sleep actions will sleep the device

**Given** I configure sleep timer to 10 minutes
**When** I remain inactive for 10 minutes
**Then** the device enters deep sleep

**Given** I configure sleep timer and reboot the device
**When** the device starts up
**Then** the configured sleep timer value is loaded from NVS
**And** the timer uses the saved value (not the default)

**Given** no sleep timer has been configured (first boot or factory reset)
**When** the device starts
**Then** the default value of 5 minutes is used

**Technical Implementation Notes:**
- Add NVS key: `"sleep_timer"` (uint8_t, stores minutes; 0 = Never)
- Add `ConfigManager::loadSleepTimer()` and `saveSleepTimer(uint8_t minutes)`
- Add menu action `SetSleepTimerAction` similar to existing mode selection actions
- Update timer initialization in `main.cpp` to use configured value
- Menu display format: "Sleep Timer: [current value]"

---

## Story 13.2: Menu-Configurable Display Timeout

As a **user who wants to save OLED power without sleeping the device**,
I want **to configure the display auto-off timeout through the menu**,
So that **the display turns off during inactivity while the device remains operational**.

**Scope:**
- `src/Menu/Model/MenuTree.h` - Add "Display Timeout" setting under Settings menu
- `src/Menu/Action/SetDisplayTimeoutAction.cpp/.h` - Configuration action
- `src/Config/ConfigManager.cpp/.h` - NVS methods for display timeout
- Display management module - Timer for OLED power control
- Input event handlers - Wake display on any activity

**Current Behavior:**
- Display timeout may not exist or is hardcoded

**New Behavior:**
- User selects from predefined options: 30s, 60s, 120s (2min), 180s (3min), 300s (5min), Never
- Selection saved to NVS
- Timer updated immediately on selection
- Any input activity (button, encoder) wakes display
- Default: TBD (recommend 60 seconds)

**Acceptance Criteria:**

**Given** I navigate to Settings menu
**When** I select "Display Timeout"
**Then** I see a list of options: "30 sec", "60 sec", "2 min", "3 min", "5 min", "Never"
**And** the current selection is highlighted

**Given** I select a new display timeout value (e.g., "60 sec")
**When** the selection is confirmed
**Then** the value is saved to NVS
**And** the display timeout timer is updated immediately
**And** the display shows confirmation (e.g., "Display timeout: 60 sec")

**Given** I configure display timeout to 60 seconds
**When** I remain inactive for 60 seconds
**Then** the OLED display turns off (powers down)
**And** the device remains fully operational (BLE connected, ready for inputs)

**Given** the display is off due to timeout
**When** I press any button or rotate the encoder
**Then** the display turns back on immediately
**And** the input is processed normally
**And** the display timeout timer resets

**Given** I select "Never"
**When** the selection is confirmed
**Then** the display never automatically turns off
**And** the display stays on until device sleep or manual power-off

**Given** display timeout is 60s and sleep timer is 5 minutes
**When** I remain inactive for 60 seconds
**Then** the display turns off
**And** the device continues running
**When** I remain inactive for another 4 minutes (5 min total)
**Then** the device enters deep sleep (display already off)

**Given** I configure display timeout and reboot the device
**When** the device starts up
**Then** the configured display timeout value is loaded from NVS
**And** the timer uses the saved value

**Technical Implementation Notes:**
- Add NVS key: `"display_timeout"` (uint16_t, stores seconds; 0 = Never)
- Add `ConfigManager::loadDisplayTimeout()` and `saveDisplayTimeout(uint16_t seconds)`
- Add menu action `SetDisplayTimeoutAction`
- Create or update display power management module
- Hook into inactivity timer to detect timeout
- Hook into all input handlers to wake display on activity
- OLED power control: Use existing display library power methods
- Display timeout and sleep timer are independent - both run simultaneously
- Menu display format: "Display Timeout: [current value]"

---

## Story 13.3: Sleep Countdown Display (Bug Fix)

As a **user approaching device sleep**,
I want **to see a live countdown of remaining time**,
So that **I know exactly when sleep will occur and can take action if needed**.

**Scope:**
- Sleep warning/countdown display logic
- Timer update to refresh countdown every second
- Display formatting for countdown

**Current Behavior:**
- Display shows "Sleep in 1min" message
- Message stays static until sleep occurs
- No live countdown or update

**New Behavior:**
- When sleep is 60 seconds away, display countdown starts
- Format: "Sleeping in 1:00", then "0:59", "0:58", ... "0:01"
- Updates every second
- Clears when display turns off at sleep

**Acceptance Criteria:**

**Given** the sleep timer is counting down
**When** 60 seconds remain until sleep
**Then** the display shows "Sleeping in 1:00"
**And** the countdown decrements every second: "0:59", "0:58", "0:57"...

**Given** the countdown is active
**When** I perform any input activity (button press, encoder rotation)
**Then** the countdown clears immediately
**And** the inactivity timer resets to configured sleep timer value
**And** normal display content resumes

**Given** the countdown reaches "0:01"
**When** the timer expires
**Then** the countdown display clears
**And** the device enters deep sleep

**Given** display timeout is shorter than sleep timer (e.g., 30s display, 5min sleep)
**When** the countdown starts but display has already turned off
**Then** no countdown is shown (display is already off)
**And** sleep occurs at the configured time

**Given** the sleep timer is set to "Never"
**When** the device is idle indefinitely
**Then** no countdown is ever displayed
**And** the device never sleeps due to inactivity

**Given** the countdown is displaying
**When** the user opens the menu
**Then** the countdown clears (menu takes priority)
**And** the inactivity timer resets (menu activity counts as activity)

**Technical Implementation Notes:**
- Trigger countdown display when sleep timer reaches 60 seconds
- Use FreeRTOS timer or existing timer mechanism to update display every 1 second
- Format: `sprintf(buffer, "Sleeping in %d:%02d", minutes, seconds)`
- Clear countdown on any input event (reset inactivity timer)
- Check if display is on before showing countdown (no point if display is off)
- Countdown display should not interfere with menu (menu has priority)
- After sleep occurs, display naturally turns off with device

---

## Implementation Checklist

### Story 13.1: Sleep Timer Configuration
- [ ] Add "Sleep Timer" item to Settings menu in MenuTree
- [ ] Create `SetSleepTimerAction` class with option selection UI
- [ ] Add `ConfigManager::loadSleepTimer()` and `saveSleepTimer()`
- [ ] Add NVS key `"sleep_timer"` with default 5 minutes
- [ ] Update sleep timer initialization in main.cpp to use configured value
- [ ] Implement "Never" option (disable sleep timer)
- [ ] Test each timer option (1, 5, 10, 30, 60 min, Never)
- [ ] Test NVS persistence across reboot
- [ ] Verify default value on first boot

### Story 13.2: Display Timeout Configuration
- [ ] Add "Display Timeout" item to Settings menu in MenuTree
- [ ] Create `SetDisplayTimeoutAction` class with option selection UI
- [ ] Add `ConfigManager::loadDisplayTimeout()` and `saveDisplayTimeout()`
- [ ] Add NVS key `"display_timeout"` with default value (recommend 60s)
- [ ] Create or update display power management module
- [ ] Implement display power-off after timeout
- [ ] Hook display wake to all input events (button, encoder)
- [ ] Implement "Never" option (display always on)
- [ ] Test each timeout option (30, 60, 120, 180, 300 sec, Never)
- [ ] Test display wake on input activity
- [ ] Test independence from sleep timer (display off, device awake)
- [ ] Test NVS persistence across reboot

### Story 13.3: Sleep Countdown Display
- [ ] Identify sleep warning trigger point (60 seconds before sleep)
- [ ] Implement countdown display update mechanism (1-second interval)
- [ ] Format countdown as "Sleeping in M:SS"
- [ ] Hook countdown clear to any input activity
- [ ] Test countdown from 1:00 to 0:01
- [ ] Test countdown clear on user activity
- [ ] Test countdown when display is already off (should not display)
- [ ] Test countdown with "Never" sleep timer (should not trigger)
- [ ] Test countdown interaction with menu (menu priority)
- [ ] Verify countdown clears at sleep

---

## Technical Architecture

**Power Management Hierarchy:**

```
User Activity
    ↓
Inactivity Detected
    ↓
[Display Timeout Timer] ----→ Display Off (device still awake)
    ↓                              ↓
[Sleep Timer continues]      Any Input → Display On
    ↓
Sleep Countdown (60s warning)
    ↓
Deep Sleep (entire device)
```

**Timer Management:**
- Both timers reset on any user activity (button, encoder, menu)
- Display timeout is independent - can fire while sleep timer continues
- Sleep timer countdown only shows if display is on

**NVS Keys:**
- `"sleep_timer"` - uint8_t (minutes, 0 = Never)
- `"display_timeout"` - uint16_t (seconds, 0 = Never)

**Configuration Defaults:**
- Sleep timer: 5 minutes (current behavior)
- Display timeout: 60 seconds (recommended)

---

## Related Files

**Story 13.1:**
- `src/Menu/Model/MenuTree.h` (add menu item)
- `src/Menu/Action/SetSleepTimerAction.cpp/.h` (new)
- `src/Config/ConfigManager.cpp/.h` (NVS methods)
- `src/main.cpp` or power management module (apply timer)

**Story 13.2:**
- `src/Menu/Model/MenuTree.h` (add menu item)
- `src/Menu/Action/SetDisplayTimeoutAction.cpp/.h` (new)
- `src/Config/ConfigManager.cpp/.h` (NVS methods)
- Display power management module (OLED control)
- Input event handlers (wake display)

**Story 13.3:**
- Sleep countdown display logic (possibly in main loop or display module)
- Timer mechanism for 1-second updates
- Input event handlers (clear countdown on activity)

---

## Embedded Systems Considerations

**Memory Impact:**
- Two additional NVS keys (3 bytes total)
- Two timer structures (minimal, likely reusing existing infrastructure)
- Countdown display string buffer (~20 bytes)
- No dynamic allocation required

**Performance:**
- Display timeout: OLED power control is fast (I2C command)
- Sleep countdown: 1-second update interval is low overhead
- No impact on input latency

**Power Consumption:**
- Display timeout significantly reduces power (OLED is major consumer)
- Sleep timer unchanged from current implementation
- Countdown display has negligible impact (1Hz update)

**OLED Lifespan:**
- Display timeout extends OLED lifespan by reducing burn-in
- Recommended to encourage users to enable display timeout

---

## User Experience Notes

**Recommended Default Configuration:**
- Sleep timer: 5 minutes (maintains current behavior)
- Display timeout: 60 seconds (saves power, extends OLED life)

**Common Use Cases:**

1. **Battery Saver:** Display 30s, Sleep 5min
2. **Always Ready:** Display 60s, Sleep Never
3. **Maximum Battery:** Display 30s, Sleep 1min
4. **Demo Mode:** Display Never, Sleep Never

**Menu Location Recommendation:**
Place both settings under Settings > Power Management submenu for discoverability.

---
