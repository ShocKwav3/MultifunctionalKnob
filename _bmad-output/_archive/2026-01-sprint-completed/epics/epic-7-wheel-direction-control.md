---
epicNumber: 7
title: "Wheel Direction Control"
complexity: Medium
dependencies: None
frsCovered: "FR6, FR7, FR8, FR9"
stories: 3
---

# Epic 7: Wheel Direction Control

**Goal:** User can reverse wheel rotation direction via the menu and have the setting persist.

**FRs covered:** FR6, FR7, FR8, FR9

**User Outcome:** User navigates to Wheel Behavior → Wheel Direction → Reversed, and the encoder now scrolls/zooms/adjusts in the opposite direction. Setting survives power cycles.

**Implementation Notes:** Menu restructure (Wheel Behavior becomes parent with Mode + Direction children). Builds on existing menu patterns.

---

## Functional Requirements

**Feature 4 - Wheel Direction:**
- FR6: "Wheel Behavior" menu restructured with sub-items: Wheel Mode, Wheel Direction
- FR7: User can set wheel direction (Normal/Reversed) via menu
- FR8: System persists wheel direction to NVS
- FR9: System applies direction setting to encoder input processing

---

## Story 7.1: Restructure Wheel Behavior Menu with Sub-Items

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

## Story 7.2: Add Wheel Direction Setting with NVS Persistence

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

## Story 7.3: Apply Wheel Direction to Encoder Input Processing

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

## Implementation Checklist

- [ ] Create `WheelDirection` enum with Normal/Reversed values
- [ ] Add wheel direction to `MenuTree.h` structure
- [ ] Implement `SetWheelDirectionAction` menu action
- [ ] Add NVS persistence for wheel direction (key: "wheel.dir")
- [ ] Update `EncoderModeHandler` to read direction from NVS
- [ ] Implement direction inversion logic in encoder input processing
- [ ] Test menu navigation for Wheel Behavior parent menu
- [ ] Test NVS persistence across power cycles
- [ ] Verify direction applies to all wheel modes (Scroll, Volume, Zoom)

---

## Technical Notes

- Store direction as uint8_t enum value in NVS for efficiency
- Consider caching direction in memory to avoid repeated NVS reads
- Direction inversion should be applied in `EncoderModeHandler` before sending HID commands
- Maintain backward compatibility with devices not yet configured (default to Normal)

---

## Related Files

- `include/Enum/WheelModeEnum.h` - Add `WheelDirection` enum here
- `src/Menu/Model/MenuTree.h` - Update menu structure
- `src/EncoderMode/Handler/*.h` - Update to respect direction setting
- `src/Config/ConfigManager.h` - Add direction persistence
