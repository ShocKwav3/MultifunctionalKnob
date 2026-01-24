---
title: "Epic Shards Index - Sprint 2"
description: "Navigation guide for current sprint epic documents"
lastUpdated: "2026-01-22"
sprint: 2
totalEpics: 1
totalStories: 5
totalFRsCovered: 6
---

# UtilityButtonsWithKnobUSB - Epic Shards Index (Sprint 2)

This index provides navigation to the current sprint epic. Previous completed epics (1-10) have been archived.

---

## Current Sprint

| Epic | Title | Complexity | Dependencies | Stories | FRs | Status |
|------|-------|-----------|--------------|---------|-----|--------|
| [11](#epic-11) | LED Strip Control | Medium-High | None | 5 | FR23-FR28 | Backlog |

---

## Epic 11: LED Strip Control

**File:** [epic-11-led-strip-control.md](./epic-11-led-strip-control.md)

**Goal:** User can control an external LED strip (power, brightness, mode, color) via the menu.

**Quick Facts:**
- 5 stories
- Medium-high complexity
- No dependencies
- Covers: LED menu, power toggle, brightness control, mode/color selection, persistence

**Stories:**
- 11.1: Add FastLED Library and LED Menu Structure
- 11.2: Implement LED Power Toggle with NVS Persistence
- 11.3: Implement Wheel-Controlled Brightness with Live Preview
- 11.4: Implement LED Mode Selection with NVS Persistence
- 11.5: Implement LED Color Selection with NVS Persistence

**Library:** FastLED

---

## Related Documents

- **[epics.md](./epics.md)** - Original master epic document (full reference)
- **[prd.md](./prd.md)** - Product requirements document
- **[implementation-patterns-consistency-rules.md](../architecture/implementation-patterns-consistency-rules.md)** - Design guidance
- **[development-guide/index.md](../development-guide/index.md)** - Development workflow

---

## Previous Sprints (Archived)

Completed epics from previous sprints have been archived to `_archive/2026-01-sprint-completed/`:

- Epic 6: Codebase Cleanup & Consolidation ✓
- Epic 7: Wheel Direction Control ✓
- Epic 8: Bluetooth Menu Control ✓
- Epic 9: Physical OLED Display ✓
- Epic 10: Power Management & Deep Sleep ✓

---

**Last Updated:** 2026-01-22
**Sprint:** 2
**Total Stories:** 5
**Total FRs:** 6
