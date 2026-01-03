---
epicNumber: 6
title: "Codebase Cleanup & Consolidation"
complexity: Low
dependencies: None
frsCovered: "FR1, FR2, FR3, FR4, FR5"
stories: 3
---

# Epic 6: Codebase Cleanup & Consolidation

**Goal:** User can trust that device identity and button configuration come from single, consistent sources.

**FRs covered:** FR1, FR2, FR3, FR4, FR5

**User Outcome:** Device name and button names are consistent everywhere, code is cleaner, and button input handling is optimized.

**Implementation Notes:** Quick wins with no dependencies. Sets foundation for clean implementation of remaining features.

---

## Functional Requirements

**Feature 1 - Device Identity:**
- FR1: System references device Bluetooth name from single source (`device_config.h`)
- FR2: NVS namespace derives from device name constant (no hardcoded "knobkoky")

**Feature 2 - Button Naming:**
- FR3: Buttons use position-based names: Top Left, Top Right, Bottom Left, Bottom Right
- FR4: Button names defined in single source (`button_config.h`), referenced elsewhere (no duplication)

**Feature 3 - Button Input Review:**
- FR5: System handles button input efficiently (investigate poll() vs event-driven, simplicity wins)

---

## Story 6.1: Consolidate Device Name as Single Source of Truth

As a **developer maintaining the codebase**,
I want **the device Bluetooth name defined in one place (`device_config.h`) and referenced everywhere else**,
So that **I can change the device identity in a single location without hunting for duplicates**.

**Acceptance Criteria:**

**Given** the device name is defined in `include/Config/device_config.h`
**When** the NVS namespace is initialized in `ConfigManager`
**Then** it derives the namespace from the device name constant (not hardcoded "knobkoky")
**And** no other file contains a hardcoded device name string

---

## Story 6.2: Consolidate Button Names with Position-Based Identifiers

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

## Story 6.3: Evaluate and Optimize Button Input Handling

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

## Implementation Checklist

- [ ] Define `DEVICE_NAME` in `include/Config/device_config.h`
- [ ] Update `ConfigManager` to derive NVS namespace from `DEVICE_NAME`
- [ ] Audit codebase for hardcoded "knobkoky" strings
- [ ] Define position-based button names in `include/Config/button_config.h`
- [ ] Update `MenuTree.h` to reference button names from config
- [ ] Verify button menu displays correct position names
- [ ] Document button input handling evaluation
- [ ] Implement recommended changes (event-driven or keep poll())
- [ ] Test device identity consistency across power cycles

---

## Technical Notes

- Use constexpr for button names to avoid runtime overhead
- Consider using a `ButtonName[]` array indexed by button position enum
- NVS namespace should be derived at compile time if possible
- Document any breaking changes to existing NVS data
