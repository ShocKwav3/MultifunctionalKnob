---
epicNumber: 10
title: "Power Management & Deep Sleep"
complexity: Medium
dependencies: "Epic 9 (for warning display)"
frsCovered: "FR19, FR20, FR21, FR22"
stories: 5
---

# Epic 10: Power Management & Deep Sleep

**Goal:** User's device automatically sleeps after inactivity to save power, with a warning before sleep.

**FRs covered:** FR19, FR20, FR21, FR22

**User Outcome:** After 4 minutes idle, user sees "Sleep in 1 min" warning. After 5 minutes, device sleeps. Pressing wheel button wakes device. Any input resets the timer.

**Implementation Notes:** Depends on Epic 9 for display warning. Wake source is wheel button only. BT connection does NOT prevent sleep.

---

## Functional Requirements

**Feature 7 - Deep Sleep:**
- FR19: System enters deep sleep after 5 minutes of inactivity
- FR20: System displays warning notification at 4 minutes (1 minute before sleep)
- FR21: User input resets inactivity timer
- FR22: Wake source is wheel button ONLY (BT connection does NOT prevent sleep)

---

## Story 10.1: Implement Inactivity Timer with Input Reset

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

## Story 10.2: Display Sleep Warning at 4 Minutes Idle

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

## Story 10.3: Enter Deep Sleep After 5 Minutes Inactivity

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

## Story 10.4: Configure Wheel Button as Wake Source

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

## Story 10.5: Auto-Reconnect Bluetooth After Wake

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

## Implementation Checklist

- [ ] Create `InactivityTimer` class with reset capability
- [ ] Integrate inactivity timer into event dispatcher
- [ ] Reset timer on wheel rotation event
- [ ] Reset timer on all button press events
- [ ] Ensure BLE events do NOT reset timer
- [ ] Create sleep warning display callback
- [ ] Implement warning display at 4-minute mark
- [ ] Configure wheel button as wake source using `esp_sleep_enable_ext1_wakeup()`
- [ ] Implement `esp_deep_sleep_start()` at 5-minute mark
- [ ] Save BLE connection state before sleep
- [ ] Implement wake handler to restore BLE connection
- [ ] Add BLE reconnection timeout (5 seconds per NFR6)
- [ ] Test inactivity timer reset with various input types
- [ ] Test warning display timing and dismissal
- [ ] Test deep sleep entry and wake
- [ ] Test BLE reconnection after wake
- [ ] Verify only wheel button wakes device

---

## Technical Notes

- Use FreeRTOS timer for inactivity tracking (more reliable than millis())
- 4-minute warning should display prominently on OLED
- Ensure display update doesn't prevent timer from reaching 5 minutes
- Save RTC memory with critical state before sleep if needed
- Wheel button wake uses `GPIO_NUM_X` (verify pin number)
- Consider using RTC memory to persist wake count/diagnostics
- BLE reconnection should use last-known peer address if available
- Implement exponential backoff for reconnection attempts during startup

---

## Related Files

- `src/Power/InactivityTimer.cpp/.h` - Create new timer class
- `src/Power/SleepManager.cpp/.h` - Create sleep control logic
- `src/Event/Dispatcher/EventDispatcher.h` - Integrate timer reset
- `src/Button/ButtonManager.h` - Wire up wake source configuration
- BLE HID Manager - Add reconnection logic
- `src/Display/Task/DisplayTask.h` - Add warning display
- `platformio.ini` - Ensure ESP-IDF sleep library linked

---

## Dependency Notes

- **Requires**: Epic 9 (Physical OLED Display) for warning display
- Must complete Epic 9 before implementing Story 10.2
- Stories 10.1, 10.3, 10.4, 10.5 can proceed independently

---

## Power Consumption Notes

- Deep sleep current: target < 10µA (typical ESP32 deep sleep: 10-100µA)
- Wake latency: ~100ms from button press to system ready
- BLE reconnection typically < 1 second in favorable conditions
