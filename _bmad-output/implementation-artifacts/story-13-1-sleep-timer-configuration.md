# Story 13.1: Menu-Configurable Sleep Timer

**Epic:** 13 - Configurable Power & Display Management
**Status:** ready-for-dev
**Complexity:** Low-Medium
**Created:** 2026-01-29

---

## User Story

As a **user who wants control over sleep behavior**,
I want **to configure the inactivity sleep timer through the menu**,
So that **I can set sleep duration to match my usage pattern or disable it entirely**.

---

## Scope

- `src/Menu/Model/MenuTree.h` - Add "Sleep Timer" setting under Settings menu
- `src/Menu/Action/SetSleepTimerAction.cpp/.h` - Configuration action (NEW)
- `src/Config/ConfigManager.cpp/.h` - NVS methods for sleep timer
- `src/main.cpp` or power management module - Apply configured timer value

---

## Current Behavior

Sleep timer is hardcoded to 5 minutes in the codebase.

---

## Desired Behavior

User can select sleep timer duration from menu: 1, 5, 10, 30, 60 minutes, or Never. Selection is saved to NVS and applied immediately. Default remains 5 minutes.

---

## Acceptance Criteria

### AC1: Menu Item Display
**Given** I navigate to Settings menu
**When** I select "Sleep Timer"
**Then** I see a list of options: "1 min", "5 min", "10 min", "30 min", "60 min", "Never"
**And** the current selection is highlighted

### AC2: Save Selection
**Given** I select a new sleep timer value (e.g., "10 min")
**When** the selection is confirmed
**Then** the value is saved to NVS
**And** the sleep timer is updated immediately
**And** the display shows confirmation (e.g., "Sleep timer: 10 min")

### AC3: Never Option
**Given** I select "Never"
**When** the selection is confirmed
**Then** the device will never automatically sleep due to inactivity
**And** the value is saved to NVS
**And** only manual power-off or explicit sleep actions will sleep the device

### AC4: Timer Enforcement
**Given** I configure sleep timer to 10 minutes
**When** I remain inactive for 10 minutes
**Then** the device enters deep sleep

### AC5: Persistence Across Reboot
**Given** I configure sleep timer and reboot the device
**When** the device starts up
**Then** the configured sleep timer value is loaded from NVS
**And** the timer uses the saved value (not the default)

### AC6: Default Value on First Boot
**Given** no sleep timer has been configured (first boot or factory reset)
**When** the device starts
**Then** the default value of 5 minutes is used

---

## Technical Implementation Notes

### NVS Storage
- **NVS Key:** `"sleep_timer"` (uint8_t)
- **Value:** Minutes (1, 5, 10, 30, 60) or 0 for "Never"
- **Default:** 5 (minutes)
- Add to `ConfigManager`:
  - `uint8_t loadSleepTimer()` - Returns minutes, defaults to 5
  - `Error saveSleepTimer(uint8_t minutes)` - Saves to NVS

### Menu Action
- Create `SetSleepTimerAction` class (similar to existing mode selection actions)
- Present 6 options with current value highlighted
- On selection:
  1. Save to NVS via `ConfigManager::saveSleepTimer()`
  2. Update active timer (see below)
  3. Show confirmation on display

### Timer Update
- **Option A:** If timer is managed in `main.cpp`:
  - Add global or static variable for timer value
  - Update after configuration change
  - Reset inactivity counter
- **Option B:** If timer is in power management module:
  - Add `PowerManager::setSleepTimer(uint8_t minutes)` method
  - Call after configuration change

### Initialization
- In `main.cpp` setup:
  ```cpp
  uint8_t sleepMinutes = configManager.loadSleepTimer();
  // Apply sleepMinutes to timer
  ```

### "Never" Handling
- Value 0 = Never
- Check for 0 before starting sleep countdown
- Disable automatic sleep entirely if 0

---

## Implementation Checklist

- [ ] Add "Sleep Timer" menu item to MenuTree
  - Add under Settings submenu
  - Display format: "Sleep Timer: [current value]"
- [ ] Create `SetSleepTimerAction` class
  - Create `src/Menu/Action/SetSleepTimerAction.cpp/.h`
  - Implement option selection UI (6 options)
  - Highlight current selection
  - Handle user selection
- [ ] Add NVS methods to ConfigManager
  - `uint8_t loadSleepTimer()` with default 5
  - `Error saveSleepTimer(uint8_t minutes)`
  - Use NVS key: `"sleep_timer"`
- [ ] Update timer initialization in main.cpp
  - Load sleep timer value during setup
  - Apply to active timer mechanism
  - Handle 0 (Never) case
- [ ] Implement timer update on configuration change
  - Update active timer when user changes setting
  - Reset inactivity counter
  - Handle "Never" case (disable timer)
- [ ] Test each timer option
  - Test 1 min: Device sleeps after 1 min inactivity
  - Test 5 min: Device sleeps after 5 min inactivity
  - Test 10 min: Device sleeps after 10 min inactivity
  - Test 30 min: Device sleeps after 30 min inactivity
  - Test 60 min: Device sleeps after 60 min inactivity
  - Test Never: Device never auto-sleeps
- [ ] Test NVS persistence
  - Configure to 10 min
  - Reboot device
  - Verify starts with 10 min timer
- [ ] Test default value
  - Factory reset device
  - Verify uses 5 min default
- [ ] Verify menu display
  - Current value shown correctly
  - Selection updates display
  - Confirmation message shown

---

## Files to Create

- `src/Menu/Action/SetSleepTimerAction.cpp` (NEW)
- `src/Menu/Action/SetSleepTimerAction.h` (NEW)

---

## Files to Modify

- `src/Menu/Model/MenuTree.h` (add menu item)
- `src/Config/ConfigManager.cpp/.h` (add NVS methods)
- `src/main.cpp` or power management module (apply timer value)

---

## Testing Notes

**Manual Testing:**
1. Navigate to Settings → Sleep Timer
2. Select "10 min" → verify saved, timer active
3. Remain inactive for 10 minutes → verify device sleeps
4. Configure to "Never" → remain inactive indefinitely → verify no sleep
5. Configure to "1 min" → remain inactive for 1 min → verify quick sleep
6. Configure to "30 min" → reboot → verify setting persists
7. Factory reset → verify defaults to "5 min"

**Timing Verification:**
- Use serial logging to track inactivity counter
- Verify timer resets on any input activity
- Verify sleep occurs at configured time

---

## Definition of Done

- [ ] All acceptance criteria passing
- [ ] All checklist items completed
- [ ] Manual testing completed for all timer values
- [ ] NVS persistence verified
- [ ] Default value verified
- [ ] No new compiler warnings
- [ ] Code follows project coding standards
- [ ] Ready for code review

---

## Related Epic

See `_bmad-output/project-planning-artifacts/epics/epic-13-power-display-configuration.md` for full epic context and architectural details.

---
