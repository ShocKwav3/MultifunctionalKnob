---
epicNumber: 8
title: "Bluetooth Menu Control"
complexity: Medium
dependencies: None
frsCovered: "FR10, FR11, FR12"
stories: 3
---

# Epic 8: Bluetooth Menu Control

**Goal:** User can manage Bluetooth pairing and connections directly from the device menu.

**FRs covered:** FR10, FR11, FR12

**User Outcome:** User can enter pairing mode or disconnect from current device without touching the host machine. Status feedback confirms actions.

**Implementation Notes:** New top-level "Bluetooth" menu item with Pair/Disconnect sub-items.

---

## Functional Requirements

**Feature 5 - Bluetooth Control:**
- FR10: User can initiate Bluetooth pairing from menu (disconnect first if connected, then advertise)
- FR11: User can disconnect current Bluetooth connection from menu
- FR12: Display shows Bluetooth status feedback (pairing, connected, disconnected)

---

## Story 8.1: Add Bluetooth Menu with Pair and Disconnect Options

As a **user managing Bluetooth connections**,
I want **a Bluetooth menu item with Pair and Disconnect sub-options**,
So that **I can control BLE connectivity directly from the device**.

**Acceptance Criteria:**

**Given** I am in the main menu
**When** I select "Bluetooth"
**Then** I see two sub-items: "Pair" and "Disconnect"
**And** I can navigate back to the main menu

---

## Story 8.2: Implement Bluetooth Pairing Action

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

## Story 8.3: Implement Bluetooth Disconnect Action

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

## Implementation Checklist

- [ ] Add Bluetooth menu item to `MenuTree.h`
- [ ] Create "Pair" and "Disconnect" sub-menu items
- [ ] Implement `PairAction` menu action handler
- [ ] Implement `DisconnectAction` menu action handler
- [ ] Add BLE pairing/advertising logic (may integrate with existing BLE manager)
- [ ] Add status display callbacks for "Pairing...", "Connected", "Disconnected"
- [ ] Handle pairing timeout (suggest 30-60 seconds)
- [ ] Handle case where device is not connected for Disconnect action
- [ ] Test menu navigation to Bluetooth options
- [ ] Test pairing flow with host device
- [ ] Test disconnect flow and status feedback
- [ ] Test no-op behavior when already disconnected

---

## Technical Notes

- Use existing BLE HID manager if available, or create BLE control interface
- Pairing mode should timeout and return to normal state if no connection within reasonable time
- Consider adding connection timeout feedback (e.g., "Pairing timeout")
- Disconnect action should cleanly close BLE connection without power down
- Display status messages via DisplayTask for thread safety

---

## Related Files

- `src/Menu/Model/MenuTree.h` - Add Bluetooth menu structure
- `src/Menu/Action/` - Create `PairAction.cpp/.h` and `DisconnectAction.cpp/.h`
- BLE HID Manager (location TBD) - Add pairing/disconnect control methods
- `src/Display/Task/DisplayTask.h` - For status feedback display

---

## Integration Points

- Must integrate with existing BLE HID implementation
- Status feedback should use existing display abstraction
- Consider intercepting menu exit to return to root on disconnect
