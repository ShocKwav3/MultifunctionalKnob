---
title: "Epic Shards Directory"
description: "Navigation hub for UtilityButtonsWithKnobUSB Iteration 2 epic documents"
---

# 📚 Epic Shards Directory

Welcome to the modular epic documentation for **UtilityButtonsWithKnobUSB Iteration 2**. This folder contains 6 focused epic documents plus navigation and reference materials.

---

## 🚀 Quick Start

**New here?** Start with one of these:

1. **[QUICK-START-REFERENCE.md](./QUICK-START-REFERENCE.md)** ← **START HERE**
   - One-page developer guide
   - Implementation phases
   - Workflow examples
   - File reference

2. **[epics-index.md](./epics-index.md)** ← For Overview
   - Master navigation hub
   - All 6 epics listed with details
   - Implementation sequence
   - Dependency mapping

3. **[SHARDING-SUMMARY.md](./SHARDING-SUMMARY.md)** ← For Context
   - What was done and why
   - Benefits of this organization
   - Maintenance notes

---

## 📖 Epic Directory

| # | Epic | Complexity | Stories | Status |
|---|------|-----------|---------|--------|
| [6](#epic-6) | [Codebase Cleanup & Consolidation](./epic-6-codebase-cleanup.md) | Low | 3 | Planning |
| [7](#epic-7) | [Wheel Direction Control](./epic-7-wheel-direction-control.md) | Medium | 3 | Planning |
| [8](#epic-8) | [Bluetooth Menu Control](./epic-8-bluetooth-menu-control.md) | Medium | 3 | Planning |
| [9](#epic-9) | [Physical OLED Display](./epic-9-physical-oled-display.md) | High | 6 | Planning |
| [10](#epic-10) | [Power Management & Deep Sleep](./epic-10-power-management-deep-sleep.md) | Medium | 5 | Planning |
| [11](#epic-11) | [LED Strip Control](./epic-11-led-strip-control.md) | Medium-High | 5 | Planning |

---

## 🎯 Implementation Phases

### Phase 1: Foundation (Start Here! 🚀)
Parallel-safe, can work on multiple epics simultaneously:

- **[Epic 6: Codebase Cleanup & Consolidation](./epic-6-codebase-cleanup.md)**
  - Device identity consolidation
  - Button naming standardization
  - Input handling optimization
  - ⏱️ Low complexity, quick wins

- **[Epic 7: Wheel Direction Control](./epic-7-wheel-direction-control.md)**
  - Menu restructuring
  - Reversible wheel direction
  - NVS persistence
  - ⏱️ Medium complexity

- **[Epic 8: Bluetooth Menu Control](./epic-8-bluetooth-menu-control.md)**
  - BT pairing from menu
  - BT disconnect control
  - Status feedback
  - ⏱️ Medium complexity

### Phase 2: Display Foundation
Essential for Phase 3:

- **[Epic 9: Physical OLED Display](./epic-9-physical-oled-display.md)**
  - 128x32 SSD1306 driver
  - Status screens with icons
  - BT icon states
  - Menu mode display
  - Display power control
  - Wake on button press
  - ⏱️ Highest complexity, 6 stories

### Phase 3: Power Management
Depends on Phase 2:

- **[Epic 10: Power Management & Deep Sleep](./epic-10-power-management-deep-sleep.md)**
  - Inactivity timer (5 minutes)
  - Sleep warning (4 minutes)
  - Deep sleep entry
  - Wake source configuration
  - BLE auto-reconnect
  - ⏱️ Medium complexity

### Phase 4: Independent Features
Can run in parallel with other phases:

- **[Epic 11: LED Strip Control](./epic-11-led-strip-control.md)**
  - LED menu structure
  - Power toggle
  - Brightness control (wheel)
  - Mode selection
  - Color selection
  - Live preview
  - NVS persistence
  - ⏱️ Medium-high complexity, 5 stories

---

## 📊 Statistics

| Metric | Value |
|--------|-------|
| **Total Epics** | 6 |
| **Total Stories** | 25 |
| **Total FRs** | 28 |
| **Total Lines** | 1,714 (detailed & actionable) |
| **Avg Epic Size** | ~130 lines |
| **Phases** | 4 sequential (with parallelization) |

---

## 🔗 Understanding Dependencies

```
✅ Phase 1 (Parallel)
   ├─ Epic 6 (no dependencies)
   ├─ Epic 7 (no dependencies)
   └─ Epic 8 (no dependencies)
         ↓
✅ Phase 2 (Sequential)
   └─ Epic 9 (no dependencies, enables Phase 3)
         ↓
✅ Phase 3 (Sequential, depends on Phase 2)
   └─ Epic 10 (needs Epic 9 for Story 10.2 warning display)
         ↓
✅ Phase 4 (Independent, can run anytime)
   └─ Epic 11 (no dependencies)
```

**Key:** Epics 6-8 can run in parallel. Epic 9 must complete before Story 10.2. Epic 11 independent.

---

## 📝 What's in Each Epic Document

Every epic shard contains:

✅ **Front Matter**
- Epic number, title, complexity, dependencies, FRs covered

✅ **Goal & Context**
- What's being built
- User outcome
- Implementation notes

✅ **Functional Requirements**
- What needs to be delivered
- Non-functional requirements where applicable

✅ **User Stories**
- Multiple stories per epic
- Given/When/Then acceptance criteria
- Highly specific and testable

✅ **Implementation Checklist**
- Step-by-step tasks
- Files to create/modify
- Testing points
- Integration notes

✅ **Technical Notes**
- Design patterns and guidance
- Architecture considerations
- Performance notes

✅ **Related Files**
- What to create
- What to modify
- Integration points

✅ **Specifications** (where applicable)
- Hardware specs
- Configuration examples
- Library dependencies

---

## 🛠️ Developer Workflow

### Getting Started with an Epic

```
1. Choose an epic from Phase 1 (6, 7, or 8)
         ↓
2. Open the epic document (epic-N-title.md)
         ↓
3. Read goal, FRs, and all stories
         ↓
4. Work through implementation checklist
         ↓
5. Use acceptance criteria to verify each story is "done"
         ↓
6. Commit: "Epic N: [story title]"
         ↓
7. Move to next epic or story
```

### Acceptance Criteria Format

All stories use: **Given** → **When** → **Then**

Example:
```
Given   I navigate to Wheel Behavior → Wheel Direction
When    I select "Reversed"
Then    the setting is saved to NVS with key "wheel.dir"
And     the menu exits or returns to parent
And     after power cycle, the direction setting is restored
```

**✓ Story is complete when ALL criteria are met.**

---

## 💡 Pro Tips

🎯 **Tip 1:** Read the entire epic before coding
- Understand full scope
- Identify dependencies
- Plan file structure

🎯 **Tip 2:** Implementation checklist ≈ commit list
- Each checklist item ideally = 1 commit
- Makes git history clear and trackable
- Easier to revert individual items if needed

🎯 **Tip 3:** Acceptance criteria = test cases
- Use them to verify completion
- Don't move to next story until all criteria met
- Document any edge cases discovered

🎯 **Tip 4:** Update as you learn
- Modify stories/epics if requirements clarify
- Document changes in commit messages
- Keep epics as living documents during implementation

🎯 **Tip 5:** Leverage dependencies wisely
- Phase 1 can run in parallel (use feature branches)
- Don't start Epic 10 before Epic 9 Story 9.6
- Epic 11 can be interleaved with other phases

---

## 📂 Folder Structure

```
epics/                              (you are here!)
├── README.md                       ← Start here!
├── QUICK-START-REFERENCE.md        ← One-page guide
├── epics-index.md                  ← Master navigation
├── SHARDING-SUMMARY.md             ← What & why
├── epic-6-codebase-cleanup.md
├── epic-7-wheel-direction-control.md
├── epic-8-bluetooth-menu-control.md
├── epic-9-physical-oled-display.md
├── epic-10-power-management-deep-sleep.md
└── epic-11-led-strip-control.md
```

---

## 🚀 Next Steps

1. **For Overview:** Read [QUICK-START-REFERENCE.md](./QUICK-START-REFERENCE.md)
2. **For Navigation:** See [epics-index.md](./epics-index.md)
3. **For Context:** Review [SHARDING-SUMMARY.md](./SHARDING-SUMMARY.md)
4. **To Start Coding:** Pick an epic from Phase 1 above

---

## 🔍 Finding Information

**"Where do I find...?"**

- **Which epic to start with?** → This README, or QUICK-START-REFERENCE.md
- **All epics & dependencies?** → epics-index.md
- **Specific story details?** → Open the epic-N document
- **Implementation checklist?** → Bottom of each epic
- **Acceptance criteria?** → In each story section
- **Technical guidance?** → "Technical Notes" section of epic
- **Files to modify?** → "Related Files" section of epic

---

## ✨ Key Features

✅ **Self-Contained** - Each epic has everything needed
✅ **Focused** - ~130 lines per epic, not 700
✅ **Actionable** - Checklists and acceptance criteria
✅ **Structured** - Consistent format across all epics
✅ **Connected** - Cross-references and dependency mapping
✅ **Trackable** - Clear progress indicators per epic
✅ **Team-Ready** - Supports parallel development
✅ **Living Docs** - Easy to update as you learn

---

## 📞 Questions?

**Q: Can I implement epics out of order?**
A: Check dependencies in [epics-index.md](./epics-index.md). Phase 1 is fully parallel-safe.

**Q: How long per epic?**
A: Epics 6-8: 2-4 sprints | Epic 9: 4-6 sprints | Epic 10: 2-3 sprints | Epic 11: 2-4 sprints

**Q: Should I modify these documents as I implement?**
A: Yes! Update as you learn. Document changes in commit messages.

**Q: What if I find a story is missing something?**
A: Add it to the epic document, document in commit message.

---

## 📋 Version Information

- **Sharding Date:** 2026-01-03
- **Total Epics:** 6
- **Total Stories:** 25
- **Total FRs:** 28
- **Status:** ✅ Ready for Implementation
- **Location:** `_bmad-output/project-planning-artifacts/epics/`

---

**Happy implementing! 🚀**

**Start with [QUICK-START-REFERENCE.md](./QUICK-START-REFERENCE.md) or jump directly to an epic from Phase 1 above.**
