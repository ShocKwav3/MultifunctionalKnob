# Story 13.3: Sleep Countdown Display (Bug Fix)

**Epic:** 13 - Configurable Power & Display Management
**Status:** ready-for-dev
**Complexity:** Low-Medium
**Created:** 2026-01-29

---

## User Story

As a **user approaching device sleep**,
I want **to see a live countdown of remaining time**,
So that **I know exactly when sleep will occur and can take action if needed**.

---

## Scope

- Sleep warning/countdown display logic (main loop or display module)
- Timer update mechanism to refresh countdown every second
- Display formatting for countdown (MM:SS format)
- Clear countdown on user activity

---

## Current Behavior (Bug)

Display shows static "Sleep in 1min" message that stays unchanged until sleep occurs. No live countdown or updates.

---

## Desired Behavior

When 60 seconds remain until sleep, display a live countdown: "Sleeping in 1:00", then "0:59", "0:58"... down to "0:01". Countdown clears when display turns off at sleep or when user performs any activity.

---

## Acceptance Criteria

### AC1: Countdown Start
**Given** the sleep timer is counting down
**When** 60 seconds remain until sleep
**Then** the display shows "Sleeping in 1:00"
**And** the countdown decrements every second: "0:59", "0:58", "0:57"...

### AC2: Countdown Clear on Activity
**Given** the countdown is active
**When** I perform any input activity (button press, encoder rotation)
**Then** the countdown clears immediately
**And** the inactivity timer resets to configured sleep timer value
**And** normal display content resumes

### AC3: Countdown Completion
**Given** the countdown reaches "0:01"
**When** the timer expires
**Then** the countdown display clears
**And** the device enters deep sleep

### AC4: Display Already Off
**Given** display timeout is shorter than sleep timer (e.g., 30s display, 5min sleep)
**When** the countdown starts but display has already turned off
**Then** no countdown is shown (display is already off)
**And** sleep occurs at the configured time

### AC5: Never Sleep Configuration
**Given** the sleep timer is set to "Never"
**When** the device is idle indefinitely
**Then** no countdown is ever displayed
**And** the device never sleeps due to inactivity

### AC6: Menu Priority
**Given** the countdown is displaying
**When** the user opens the menu
**Then** the countdown clears (menu takes priority)
**And** the inactivity timer resets (menu activity counts as activity)

---

## Technical Implementation Notes

### Countdown Trigger
- Monitor sleep inactivity timer
- When timer reaches 60 seconds remaining:
  - Check if display is on (no point showing countdown if display is off)
  - If display on, start countdown display
  - If display off, skip countdown (device will sleep silently)

### Timer Update Mechanism
- **Option A:** FreeRTOS timer with 1-second period
  - Create timer when countdown starts
  - Timer callback updates display
  - Delete timer when countdown clears
- **Option B:** Check in main loop
  - Track last update time
  - Update display every 1 second
  - Clear on activity or completion

### Display Format
```cpp
sprintf(buffer, "Sleeping in %d:%02d", minutes, seconds);
```
- Format: "Sleeping in 1:00" → "Sleeping in 0:59" → ... → "Sleeping in 0:01"
- Use existing display functions to show message
- Position: Center of screen or top line (verify with display layout)

### Clear Countdown Events
- **User activity:** Button press, encoder rotation, menu open
- **Countdown completion:** Timer expires, device sleeps
- **Display timeout:** If display turns off before sleep, clear countdown

### Integration with Display Timeout (Story 13.2)
- If display timeout occurs during countdown, countdown clears
- If display wakes up during final 60s, countdown should resume
- If display is off when countdown would start, skip countdown

### Integration with Sleep Timer (Story 13.1)
- Countdown only triggers if sleep timer is NOT "Never"
- If sleep timer is 1 minute or less, countdown starts immediately
- Countdown duration: min(60 seconds, remaining_sleep_time)

---

## Implementation Checklist

- [ ] Identify sleep warning trigger point
  - Find where sleep inactivity timer is tracked
  - Add check for "60 seconds remaining"
  - Trigger countdown display if display is on
- [ ] Implement countdown display mechanism
  - Choose timer approach (FreeRTOS timer or main loop check)
  - Create countdown state tracking
  - Implement 1-second update interval
- [ ] Format countdown string
  - Implement sprintf with MM:SS format
  - Handle minutes and seconds correctly
  - Test format: "Sleeping in 1:00", "0:59", "0:58"...
- [ ] Display countdown on OLED
  - Choose display position (center or top line)
  - Use existing display functions
  - Ensure countdown doesn't interfere with menu if active
- [ ] Hook countdown clear to input events
  - Clear on encoder rotation (EncoderEventHandler)
  - Clear on button press (ButtonEventHandler)
  - Clear on menu open (MenuController)
  - Reset sleep inactivity timer on activity
- [ ] Handle countdown completion
  - Clear countdown when timer expires
  - Allow normal sleep process to proceed
  - Display naturally turns off with device sleep
- [ ] Handle display timeout during countdown
  - If display turns off, clear countdown state
  - If display wakes up during final 60s, resume countdown
  - Handle display off when countdown would start (skip countdown)
- [ ] Handle "Never" sleep timer
  - Check if sleep timer is 0 (Never)
  - Skip countdown trigger if sleep disabled
  - No countdown display for "Never" configuration
- [ ] Handle menu priority
  - If menu opens during countdown, clear countdown
  - Menu activity should reset inactivity timer
  - Resume normal display after menu closes
- [ ] Test countdown from 1:00 to 0:01
  - Verify each second decrements correctly
  - Verify format displays correctly
  - Verify updates smoothly (no flicker)
- [ ] Test countdown clear on user activity
  - Countdown active → press button → verify clears + timer resets
  - Countdown active → rotate encoder → verify clears + timer resets
  - Countdown active → open menu → verify clears
- [ ] Test countdown with display off
  - Configure: Display 30s, Sleep 5min
  - Wait for display to turn off
  - Wait for countdown time → verify no countdown shown
  - Verify sleep still occurs at 5min
- [ ] Test countdown with "Never" sleep
  - Configure sleep to "Never"
  - Remain idle indefinitely
  - Verify no countdown ever shown
- [ ] Test countdown completion
  - Let countdown run to 0:01
  - Verify device sleeps
  - Verify countdown cleared

---

## Files to Modify

- `src/main.cpp` or sleep timer module (countdown trigger and display)
- Display module (render countdown text)
- `src/Event/Handler/EncoderEventHandler.cpp` (clear on encoder activity)
- `src/Event/Handler/ButtonEventHandler.cpp` (clear on button activity)
- `src/Menu/Controller/MenuController.cpp` (clear on menu open, if needed)

---

## Testing Notes

**Manual Testing:**
1. Configure sleep to 5min → remain idle for 4min → verify countdown starts at 1:00
2. During countdown → press button → verify clears + timer resets
3. During countdown → rotate encoder → verify clears + timer resets
4. Let countdown run to completion → verify device sleeps at 0:01
5. Configure: Display 30s, Sleep 5min → let display turn off → verify no countdown shown when sleep approaches
6. Configure sleep to "Never" → remain idle → verify no countdown ever shown
7. During countdown → open menu → verify countdown clears
8. Countdown format verification: "Sleeping in 1:00" → "0:59" → "0:58" → ... → "0:01"

**Visual Verification:**
- Countdown displays in readable location
- No flicker or artifacts during update
- Countdown clears cleanly (no remnants on display)

---

## Definition of Done

- [ ] All acceptance criteria passing
- [ ] All checklist items completed
- [ ] Manual testing completed successfully
- [ ] Countdown timing accurate (1-second intervals)
- [ ] Display format correct and readable
- [ ] Integration with display timeout verified
- [ ] Integration with sleep timer verified
- [ ] No new compiler warnings
- [ ] Code follows project coding standards
- [ ] Ready for code review

---

## Related Epic

See `_bmad-output/project-planning-artifacts/epics/epic-13-power-display-configuration.md` for full epic context.

---

## Notes

This bug fix significantly improves user experience by providing clear feedback before sleep. Users can see exactly how much time remains and take action if needed (e.g., press a button to reset the timer).

The countdown also serves as a gentle reminder that the device is about to sleep, preventing surprise disconnections during active use.

---
