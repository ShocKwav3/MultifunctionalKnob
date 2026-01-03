---
title: "Epic Shards Index"
description: "Navigation guide for all modular epic documents"
lastUpdated: "2026-01-03"
totalEpics: 6
totalStories: 25
totalFRsCovered: 28
---

# UtilityButtonsWithKnobUSB - Epic Shards Index

This index provides navigation to all individual epic documents that have been sharded from the master `epics.md` file for better modularity and focused implementation.

---

## Quick Reference Table

| Epic | Title | Complexity | Dependencies | Stories | FRs | Status |
|------|-------|-----------|--------------|---------|-----|--------|
| [6](#epic-6) | Codebase Cleanup & Consolidation | Low | None | 3 | FR1-FR5 | Planning |
| [7](#epic-7) | Wheel Direction Control | Medium | None | 3 | FR6-FR9 | Planning |
| [8](#epic-8) | Bluetooth Menu Control | Medium | None | 3 | FR10-FR12 | Planning |
| [9](#epic-9) | Physical OLED Display | High | None | 6 | FR13-FR18 | Planning |
| [10](#epic-10) | Power Management & Deep Sleep | Medium | Epic 9 | 5 | FR19-FR22 | Planning |
| [11](#epic-11) | LED Strip Control | Medium-High | None | 5 | FR23-FR28 | Planning |

---

## Implementation Sequence Recommendations

### Phase 1: Foundation (No Dependencies)
These epics can be implemented in parallel and should be completed first:

1. **[Epic 6: Codebase Cleanup & Consolidation](./epic-6-codebase-cleanup.md)** 
   - Sets foundation for clean code practices
   - Quick wins with low complexity
   - No blocking dependencies

2. **[Epic 7: Wheel Direction Control](./epic-7-wheel-direction-control.md)**
   - Medium complexity, builds on existing menu patterns
   - No dependencies, can start immediately after Epic 6

3. **[Epic 8: Bluetooth Menu Control](./epic-8-bluetooth-menu-control.md)**
   - Medium complexity, independent feature
   - Benefits from Epic 7 menu restructuring patterns

### Phase 2: Display Implementation
Essential for later features:

4. **[Epic 9: Physical OLED Display](./epic-9-physical-oled-display.md)**
   - Highest complexity, highest impact
   - Required by Epic 10 for warning display
   - Start after Phase 1 or in parallel if resources allow

### Phase 3: Power Management (Depends on Display)

5. **[Epic 10: Power Management & Deep Sleep](./epic-10-power-management-deep-sleep.md)**
   - Depends on Epic 9 for warning display
   - Can implement Stories 10.1, 10.3, 10.4, 10.5 independently
   - Story 10.2 requires Epic 9

### Phase 4: Enhancement Features

6. **[Epic 11: LED Strip Control](./epic-11-led-strip-control.md)**
   - Independent feature, can run in parallel with other phases
   - Medium-high complexity, self-contained
   - Benefits from menu patterns established in previous epics

---

## Epic Details

### Epic 6
**File:** [epic-6-codebase-cleanup.md](./epic-6-codebase-cleanup.md)

**Goal:** User can trust that device identity and button configuration come from single, consistent sources.

**Quick Facts:**
- 3 stories
- Low complexity
- No dependencies
- Covers: Device name consolidation, button naming, input review

**Key Stories:**
- 6.1: Consolidate Device Name as Single Source of Truth
- 6.2: Consolidate Button Names with Position-Based Identifiers
- 6.3: Evaluate and Optimize Button Input Handling

---

### Epic 7
**File:** [epic-7-wheel-direction-control.md](./epic-7-wheel-direction-control.md)

**Goal:** User can reverse wheel rotation direction via the menu and have the setting persist.

**Quick Facts:**
- 3 stories
- Medium complexity
- No dependencies
- Covers: Menu restructure, direction setting, encoder application

**Key Stories:**
- 7.1: Restructure Wheel Behavior Menu with Sub-Items
- 7.2: Add Wheel Direction Setting with NVS Persistence
- 7.3: Apply Wheel Direction to Encoder Input Processing

---

### Epic 8
**File:** [epic-8-bluetooth-menu-control.md](./epic-8-bluetooth-menu-control.md)

**Goal:** User can manage Bluetooth pairing and connections directly from the device menu.

**Quick Facts:**
- 3 stories
- Medium complexity
- No dependencies
- Covers: BT pairing, disconnect, status feedback

**Key Stories:**
- 8.1: Add Bluetooth Menu with Pair and Disconnect Options
- 8.2: Implement Bluetooth Pairing Action
- 8.3: Implement Bluetooth Disconnect Action

---

### Epic 9
**File:** [epic-9-physical-oled-display.md](./epic-9-physical-oled-display.md)

**Goal:** User sees device status at a glance on the physical 128x32 OLED screen with intuitive icons.

**Quick Facts:**
- 6 stories
- High complexity
- No direct dependencies (but enables Epic 10)
- Covers: OLED driver, status screens, power control, wake trigger

**Key Stories:**
- 9.1: Implement OLEDDisplay Driver for 128x32 SSD1306
- 9.2: Create Normal Mode Status Screen with Icons
- 9.3: Implement Bluetooth Icon States (Solid/Flashing/None)
- 9.4: Create Menu Mode Display with Status Bar
- 9.5: Add Display Power Control Menu Option
- 9.6: Implement Display Wake on Menu Activation

**Hardware Specs:**
- 128x32 OLED SSD1306
- I2C interface (SDA=GPIO6, SCL=GPIO7)

---

### Epic 10
**File:** [epic-10-power-management-deep-sleep.md](./epic-10-power-management-deep-sleep.md)

**Goal:** User's device automatically sleeps after inactivity to save power, with a warning before sleep.

**Quick Facts:**
- 5 stories
- Medium complexity
- Depends on: Epic 9 (for Story 10.2 warning display)
- Covers: Inactivity timer, sleep warning, deep sleep, wake source, BLE reconnection

**Key Stories:**
- 10.1: Implement Inactivity Timer with Input Reset
- 10.2: Display Sleep Warning at 4 Minutes Idle *(requires Epic 9)*
- 10.3: Enter Deep Sleep After 5 Minutes Inactivity
- 10.4: Configure Wheel Button as Wake Source
- 10.5: Auto-Reconnect Bluetooth After Wake

**Timing:**
- 4 minutes: Display warning
- 5 minutes: Enter deep sleep
- Wake source: Wheel button only

---

### Epic 11
**File:** [epic-11-led-strip-control.md](./epic-11-led-strip-control.md)

**Goal:** User can control an external LED strip (power, brightness, mode, color) via the menu.

**Quick Facts:**
- 5 stories
- Medium-high complexity
- No dependencies
- Covers: LED menu, power toggle, brightness control, mode/color selection, persistence

**Key Stories:**
- 11.1: Add FastLED Library and LED Menu Structure
- 11.2: Implement LED Power Toggle with NVS Persistence
- 11.3: Implement Wheel-Controlled Brightness with Live Preview
- 11.4: Implement LED Mode Selection with NVS Persistence
- 11.5: Implement LED Color Selection with NVS Persistence

**Library:** FastLED

---

## Cross-Epic Dependencies

### Direct Dependencies
- **Epic 10 → Epic 9**: Story 10.2 (sleep warning display) requires OLED display implementation

### Beneficial Orderings (not blocking)
- **Epic 6 → All Others**: Cleanup should complete first for foundation
- **Epic 7 → Epic 8, 11**: Menu restructuring patterns benefit later menu features
- **Epic 9 → Epic 10**: While not blocking Stories 10.1/10.3/10.4/10.5, display needed for Story 10.2

---

## Functional Requirements Coverage

All 28 functional requirements are distributed across the 6 epics:

**Epic 6:** FR1, FR2, FR3, FR4, FR5 (Device Identity & Button Config)
**Epic 7:** FR6, FR7, FR8, FR9 (Wheel Direction)
**Epic 8:** FR10, FR11, FR12 (Bluetooth Control)
**Epic 9:** FR13, FR14, FR15, FR16, FR17, FR18 (OLED Display)
**Epic 10:** FR19, FR20, FR21, FR22 (Power Management)
**Epic 11:** FR23, FR24, FR25, FR26, FR27, FR28 (LED Control)

---

## How to Use These Shards

1. **For Implementation**: Select an epic shard and work through its stories sequentially
2. **For Planning**: Use the cross-epic dependencies table to plan your implementation sequence
3. **For Tracking**: Each shard includes an implementation checklist
4. **For Reference**: Each story includes acceptance criteria and technical notes

---

## Related Documents

- **[epics.md](./epics.md)** - Original master epic document (full reference)
- **[prd.md](./prd.md)** - Product requirements document
- **[implementation-patterns-consistency-rules.md](../architecture/implementation-patterns-consistency-rules.md)** - Design guidance
- **[development-guide/index.md](../development-guide/index.md)** - Development workflow

---

## Status Legend

- **Planning** - Ready to implement
- **In Progress** - Currently being worked on
- **Blocked** - Waiting on dependency
- **Complete** - Implementation and testing done
- **Verified** - Merged and verified in main

---

## Notes for Developers

- Each epic shard is **completely self-contained** with all necessary context
- Shards include implementation checklists for progress tracking
- Technical notes provide guidance on architecture and design decisions
- Related files sections identify what needs to be created/modified
- Acceptance criteria are the source of truth for "done"

---

**Last Updated:** 2026-01-03
**Total Stories:** 25
**Total FRs:** 28
**Recommended Phases:** 4 sequential phases with optional parallelization
