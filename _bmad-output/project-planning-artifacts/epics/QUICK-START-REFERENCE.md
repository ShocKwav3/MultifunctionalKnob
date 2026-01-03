---
title: "Epic Sharding Quick Start Reference"
description: "One-page reference for developers implementing sharded epics"
format: "Quick Reference Card"
---

# 🚀 Epic Sharding Quick Start Reference

## Files at a Glance

| File | Purpose | Start Here? |
|------|---------|-------------|
| **epics-index.md** | Navigation hub, dependencies, implementation sequence | ✅ **YES** |
| **epic-6-codebase-cleanup.md** | Device identity & button config consolidation | Phase 1 |
| **epic-7-wheel-direction-control.md** | Wheel direction reversal | Phase 1 |
| **epic-8-bluetooth-menu-control.md** | BT pairing/disconnect from menu | Phase 1 |
| **epic-9-physical-oled-display.md** | 128x32 OLED display implementation | Phase 2 |
| **epic-10-power-management-deep-sleep.md** | Sleep after 5min inactivity | Phase 3 |
| **epic-11-led-strip-control.md** | LED strip control via menu | Phase 4 |
| **SHARDING-SUMMARY.md** | What was done and why | Reference |

---

## Implementation Phases

```
Phase 1 (PARALLEL)          Phase 2              Phase 3           Phase 4
─────────────────────       ──────────          ──────────        ──────────
│ Epic 6: Cleanup    │   →  │ Epic 9:    │   →  │ Epic 10: │   │ Epic 11: │
│ Epic 7: Direction  │      │ OLED       │      │ Sleep    │   │ LED      │
│ Epic 8: Bluetooth  │      │ (6 stories)│      │          │   │ (5 strs) │
│ (3+3+3 stories)    │      │            │      │          │   │          │
└────────────────────┘      └────────────┘      └──────────┘   └──────────┘
```

**Key:** Epic 6-8 can run in parallel. Epic 9 enables Epic 10's Story 10.2. Epic 11 independent.

---

## Quick Epic Selector

**"I want to..."** → **Go to**

- Organize device identity & button names → **Epic 6**
- Let user reverse wheel direction → **Epic 7**
- Add BT pairing/disconnect to menu → **Epic 8**
- Add physical OLED display support → **Epic 9**
- Auto-sleep device after inactivity → **Epic 10**
- Add LED strip control → **Epic 11**
- See all epics & dependencies → **epics-index.md**

---

## Each Epic Contains

✅ **Header:** Epic number, title, complexity, dependencies, FR coverage
✅ **Functional Requirements:** What needs to be built
✅ **Stories:** User stories with acceptance criteria (Given/When/Then)
✅ **Implementation Checklist:** Step-by-step tasks
✅ **Technical Notes:** Design guidance and architecture tips
✅ **Related Files:** What to create/modify
✅ **(Optional) Specs:** Hardware or configuration examples

---

## Workflow Example

### For a Developer Starting Epic 7:

1. Open `epic-7-wheel-direction-control.md`
2. Read functional requirements
3. Work through 3 stories:
   - 7.1: Restructure menu
   - 7.2: Add direction setting
   - 7.3: Apply to encoder
4. Follow implementation checklist
5. Use acceptance criteria to verify "done"
6. Commit with message: "Epic 7: Implement wheel direction control"

---

## Dependency Quick Check

```
✅ Can start immediately:
   └─ Epics 6, 7, 8 (Phase 1) - no dependencies

⏳ Can start after Epic 9:
   └─ Epic 10 (Story 10.2 specifically needs display)
   └─ Stories 10.1, 10.3, 10.4, 10.5 can start earlier

✅ Can start anytime:
   └─ Epic 11 (LED control, independent)
```

---

## Acceptance Criteria Format

Every story uses: **Given** → **When** → **Then**

Example from Epic 7, Story 7.2:

```
Given  I navigate to Wheel Behavior → Wheel Direction
When   I select "Reversed"
Then   the selection is saved to NVS with key "wheel.dir"
And    I see confirmation feedback
And    the menu exits or returns to parent
And    after power cycle, direction is restored from NVS
```

**All acceptance criteria must be met for the story to be "done".**

---

## Implementation Checklist Pattern

Each epic includes a checklist like:

```
- [ ] Create X class
- [ ] Add Y to MenuTree.h
- [ ] Implement Z action
- [ ] Test feature A
- [ ] Test feature B
```

**Mark as complete as you finish each item.**

---

## Key Statistics

| Metric | Value |
|--------|-------|
| **Total Epics** | 6 |
| **Total Stories** | 25 |
| **Total FRs** | 28 |
| **Avg Epic Size** | ~130 lines (focused!) |
| **Largest Epic** | Epic 9 (6 stories, highest complexity) |
| **Smallest Epic** | Epics 6, 7, 8 (3 stories each, quick wins) |

---

## Where to Get Help

**Q: Which epic should I start with?**
A: See epics-index.md Phase 1. Epics 6-8 are parallel-safe, low-medium complexity.

**Q: Can I skip an epic?**
A: Only if dependencies allow. See dependency map in epics-index.md.

**Q: What if I find a problem with a story?**
A: Update the epic shard directly, document in commit message.

**Q: How do I track progress?**
A: Use implementation checklists in each epic, git commits, and update epics-index.md status.

**Q: Are there design patterns I should follow?**
A: Check `implementation-patterns-consistency-rules.md` in architecture folder.

---

## File Locations

```
UtilityButtonsWithKnobUSB/
└── _bmad-output/project-planning-artifacts/
    ├── epics-index.md                          ← Start here!
    ├── epic-6-codebase-cleanup.md
    ├── epic-7-wheel-direction-control.md
    ├── epic-8-bluetooth-menu-control.md
    ├── epic-9-physical-oled-display.md
    ├── epic-10-power-management-deep-sleep.md
    ├── epic-11-led-strip-control.md
    ├── SHARDING-SUMMARY.md
    ├── QUICK-START-REFERENCE.md               ← You are here!
    └── epics.md                                (original reference)
```

---

## Development Workflow

```
1. Read epics-index.md
         ↓
2. Choose epic from Phase 1 (6, 7, or 8)
         ↓
3. Open epic-N-title.md
         ↓
4. Read FRs and all stories
         ↓
5. Work through implementation checklist
         ↓
6. Use acceptance criteria to verify completion
         ↓
7. Commit: "Epic N: Implement [feature]"
         ↓
8. Repeat with next epic
```

---

## Key Concepts

🎯 **Epic** = Large feature grouping multiple related stories
📖 **Story** = Implementable user story with acceptance criteria
✅ **Acceptance Criterion** = Testable condition for "done"
📋 **Checklist** = Step-by-step implementation tasks
🔗 **Dependency** = What must be done before this epic

---

## Pro Tips

💡 **Tip 1:** Read the entire epic before starting to code
💡 **Tip 2:** Implementation checklist ≈ your commit list
💡 **Tip 3:** Each checklist item = ideally 1 commit
💡 **Tip 4:** Acceptance criteria = your test cases
💡 **Tip 5:** Update epics-index.md status as you complete epics

---

## Common Questions Answered

**Q: Is epics.md still needed?**
A: No, use epic shards instead. Original kept for reference only.

**Q: Can I parallelize Phase 1?**
A: Yes! Epics 6, 7, 8 have no interdependencies. Use feature branches.

**Q: What's the expected duration per epic?**
A: Epic 6-8: 2-4 sprints each
     Epic 9: 4-6 sprints (most complex)
     Epic 10: 2-3 sprints
     Epic 11: 2-4 sprints

**Q: Should I modify the epics as I implement?**
A: Yes! Update as you learn. Document changes in commits.

**Q: How do I know if I'm done?**
A: All acceptance criteria met + all checklist items checked ✓

---

**Last Updated:** 2026-01-03
**Version:** 1.0
**Status:** Ready for Implementation

👉 **Next Step:** Open `epics-index.md` →
