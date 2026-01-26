# Sprint Change Proposal: Macro Button Toggle Activation

**Date:** 2026-01-26
**Project:** UtilityButtonsWithKnobUSB
**Epic:** Epic 11 - Macro Activation System
**Change Type:** Requirement Refinement → UX/Implementation Optimization
**Status:** Pending Implementation

---

## 1. Issue Summary

### Problem Statement

During Epic 11 planning, a critical UX issue was identified with the originally-defined macro button behavior:

**Original Behavior (Press-Hold-Release):**
- User must continuously hold the macro button while pressing other inputs
- While held, all inputs execute macros instead of normal actions
- Upon release, everything returns to normal

**Issues Discovered:**
1. **Difficult single-hand operation** - User cannot easily hold macro button while operating other inputs with the same hand
2. **Implementation overhead** - Requires continuous GPIO state tracking in ButtonEventHandler
3. **Poor UX pattern** - Modifier-style activation is awkward for this use case vs. toggle-based control

### Discovery Context

Identified during Story 11.2 (Hardware Layer) definition, **before implementation began**. Story 11.1 (Infrastructure) has been completed with the infrastructure in place to support either approach.

### Evidence & Impact

- **Operational testing:** Manual simulation confirmed single-hand operation is impractical
- **Architecture review:** Toggle pattern is simpler to implement and align with existing menu patterns
- **Implementation readiness:** Epic 11.2-11.4 are at ready-for-dev stage — perfect timing for refinement

---

## 2. Impact Analysis

### Epic Impact

**Epic 11 Status:** in-progress (Story 11.1 **DONE**, Stories 11.2-11.4 ready-for-dev)

**⚠️ CRITICAL: Story 11.1 Already Implemented**

Story 11.1 (Macro Infrastructure) has been completed with the **original press-hold-release behavior**:
- MacroManager implements `setMacroButtonState(bool held)` interface
- NVS methods implemented for macro persistence
- Macro execution logic coded

**Stories Affected:**
- **Story 11.1 (Macro Infrastructure - COMPLETED):** ⏳ REQUIRES ASSESSMENT
  - May need interface changes: `setMacroButtonState()` → `toggleMacroMode()`
  - **Architect + Dev must assess:** Can toggle be implemented with minimal changes, or does logic need redesign?
  - Pending decision: Modify existing implementation vs. scope expansion

- **Story 11.2 (Hardware Layer):** Major impact — acceptance criteria change from press-hold-release to long-press toggle
- **Story 11.3 (User Configuration):** Minor impact — menu UI remains largely the same, no functional changes
- **Story 11.4 (System Integration):** Minor impact — initialization unchanged, state management simplified

**Change Scope:** Story 11.1 is complete; Stories 11.2-11.4 not yet started. Scope depends on Story 11.1 assessment.

### Artifact Impact

| Artifact | Impact | Reason |
|----------|--------|--------|
| **PRD** | +3 new FRs (FR-10, FR-11, FR-12) + renumbered subsequent FRs | Clarifies toggle model for macro control |
| **Epic 11** | Goal rewritten | Emphasizes toggle activation vs. modifier-style |
| **Story 11.1** | ⏳ PENDING ASSESSMENT | Completed with old behavior; Architect + Dev assess changes needed |
| **Story 11.2** | Major rewrite (AC 1-7) | Implements long-press detection + toggle state |
| **Architecture** | Interface updated | MacroManager toggles mode vs. tracking button state |

### Technical Changes Required

**MacroManager:**
- Replace `setMacroButtonState(bool held)` → `toggleMacroMode()`
- Replace member `macroButtonHeld` → `macroModeActive` (toggle state)
- No changes to macro execution logic

**ButtonEventHandler:**
- Add `uint32_t macroPressStart` member for long-press timing
- Implement long-press detection (≥500ms threshold)
- Call `toggleMacroMode()` on long-press release
- Ignore short-presses (<500ms)

**EncoderEventHandler:**
- No changes (macro interception logic already defined)

**ConfigManager:**
- No changes (no new NVS keys)

### No Breaking Changes

- Existing macros persist (NVS schema unchanged)
- Menu configuration unchanged
- Macro execution logic unchanged
- BLE integration unchanged

---

## 3. Recommended Approach

### Selected Path: Direct Adjustment ✅

**Option 1: Direct Adjustment** (SELECTED)
- Modify Story 11.2 acceptance criteria and tasks
- Update MacroManager interface (toggle vs. state tracking)
- Update Architecture document
- **Effort:** Medium
- **Risk:** Low (implementation not yet started)
- **Timeline:** No impact — handles within sprint

**Why This Path:**
1. **Cleaner UX** — Toggle is more intuitive than sustain-press for users
2. **Simpler implementation** — Long-press detection is straightforward; toggle state machine is simpler than GPIO tracking
3. **Zero implementation waste** — Story 11.1 (infrastructure) already complete; Stories 11.2-11.4 not yet started
4. **Better alignment** — Matches menu navigation pattern (press to activate)

### Design Validation

**Architecture Coherence:**
- Toggle state machine fits existing event-driven architecture
- Long-press detection integrates cleanly with ButtonEventHandler
- No new dependencies or patterns introduced
- Consistent with existing BleKeyboard + menu priority order

**Requirement Coverage:**
- FR-10 (activate/deactivate via long-press) ✅
- FR-11 (mode control input execution) ✅
- FR-12 (state persists across menu) ✅

---

## 4. Detailed Change Proposals

### CHANGE 1: PRD — New Functional Requirements

**File:** `prd.md`
**Section:** Functional Requirements → Added "Macro Control" subsection

```markdown
### Macro Control

- FR10: User can activate/deactivate macro mode by long-pressing the macro button (toggle)
- FR11: When macro mode is active, inputs execute assigned macros; when deactivated, inputs return to normal behavior
- FR12: Macro mode state persists across menu navigation
```

**Rationale:** Clarifies the toggle-based behavior model distinct from traditional modifiers.

---

### CHANGE 2: Epic 11 Overview — Goal & Functional Requirements

**File:** `epic-11-macro-activation-system.md`
**Sections:** Goal statement + FR-1 through FR-6

**OLD → NEW:**
```
OLD Goal: "Enable users to assign key combination macros to existing inputs, triggered by holding a dedicated macro button."

NEW Goal: "Enable users to toggle macro mode on/off via long-press of a dedicated macro button, allowing key combination macros to be triggered on existing inputs."

OLD FR-6: "While macro button held, inputs execute macro; released = normal behavior"

NEW FR-6: "Long-press and release macro button activates macro mode; inputs execute assigned macros. Long-press again deactivates macro mode; inputs return to normal behavior."
```

**Rationale:** Emphasizes toggle activation model and removes ambiguity about "modifier-style" behavior.

---

### CHANGE 3: Story 11.2 — Complete Rewrite (Acceptance Criteria & Dev Notes)

**File:** `11-2-hardware-layer.md`

**Story Description Update:**
```
OLD: "As a user holding the macro button, I want my inputs to execute macros instead of normal actions, So that I can trigger key combinations with a single press."

NEW: "As a user wanting single-hand macro control, I want to toggle macro mode on/off by long-pressing the macro button, So that I can execute macros with a simple press-and-release gesture."
```

**Acceptance Criteria — NEW (AC 1-7):**

1. **Given** the macro button (GPIO 10) is long-pressed (held ≥500ms) and released
   **When** ButtonEventHandler detects the long-press event
   **Then** MacroManager.toggleMacroMode() is called
   **And** macro mode transitions from inactive→active
   **And** a confirmation is logged or displayed

2. **Given** macro mode is currently active
   **When** the macro button is long-pressed (held ≥500ms) and released again
   **Then** MacroManager.toggleMacroMode() is called
   **And** macro mode transitions from active→inactive
   **And** a confirmation is logged or displayed

3. **Given** the macro button is short-pressed (< 500ms)
   **When** ButtonEventHandler receives the event
   **Then** the short-press is ignored (no action triggered, no mode change)

4-7. [Macro execution, priority order unchanged]

**Task 1 Update:** Implement Long-Press Detection for Macro Button
- Add `macroPressStart` member for timing
- Detect release event and calculate duration
- Call `toggleMacroMode()` if duration ≥ 500ms
- Ignore short-presses

**Task 4 Update:** Add Logging for Macro Mode Toggle (instead of button state)

**Task 5 Update:** Build and Verify Toggle Behavior (test long-press, short-press, mode persistence)

**Dev Notes Update:**
- Event Handling Priority diagram updated for long-press detection
- Macro Button Behavior: Long-press threshold, toggle state, short-press ignored
- Testing Approach: Tests for ON/OFF toggle, short-press, persistence

**Rationale:** Implements long-press detection + toggle state machine instead of continuous button state tracking.

---

### CHANGE 4: Architecture Document — MacroManager Interface

**File:** `macro-feature-architecture.md`
**Section:** Component Architecture

**MacroManager Class Definition:**
```cpp
// OLD
class MacroManager {
    bool macroButtonHeld;  // Button state
public:
    void setMacroButtonState(bool held);
    bool isMacroModeActive() const { return macroButtonHeld; }
};

// NEW
class MacroManager {
    bool macroModeActive;  // Toggle state
public:
    void toggleMacroMode();  // Toggle on/off
    bool isMacroModeActive() const { return macroModeActive; }
};
```

**State Pattern Decision Update:**
```
OLD: "State Pattern | Simple boolean flag | Macro button = held/released state"

NEW: "State Pattern | Toggle state machine | Macro mode = active/inactive toggle (long-press to transition)"
NEW: "Long-Press Detection | In ButtonEventHandler | 500ms hold time threshold"
```

**Rationale:** Documents the toggle state pattern and delegates long-press detection responsibility.

---

### CHANGE 5: Architecture Document — Macro Button State Flow

**File:** `macro-feature-architecture.md`
**Section:** Communication Patterns

```
OLD Flow:
  ButtonEventHandler receives macro button event
    → macroManager.setMacroButtonState(pressed)
    → No further action

NEW Flow:
  ButtonEventHandler receives macro button GPIO event
    → Checks hold duration (long-press ≥ 500ms?)
      ├─ YES (long-press released) → macroManager.toggleMacroMode()
      └─ NO (short-press) → Ignored, no action
    → No further action (toggle control only)
```

**Rationale:** Clarifies the long-press detection + toggle behavior in ButtonEventHandler.

---

### CHANGE 6: Architecture Document — Anti-Patterns Updated

**File:** `macro-feature-architecture.md`
**Section:** Anti-Patterns to Avoid

Added specific anti-patterns for toggle implementation:
- ❌ Tracking button state instead of toggle state
- ❌ Not detecting long-press duration
- ✅ Correct: Long-press detection with toggle

**Rationale:** Prevents implementation drift toward original sustained-press model.

---

## 5. Implementation Handoff

### Change Scope Classification: **MAJOR** (Pending Story 11.1 Assessment)

**Why Major:**
- Story 11.1 is already **completed** with the original press-hold-release behavior
- **Assessment required:** Can toggle be implemented by modifying Story 11.1, or is rollback+re-implementation needed?
- This assessment determines actual scope (Moderate if minor changes, Major if significant rework)

### Handoff Route: **Architect + Development Team → Assess Story 11.1 → Route Accordingly**

**Phase 1: Architect Assessment (CRITICAL)**

| Role | Task |
|------|------|
| **Solution Architect** | Review Story 11.1 implementation + updated macro-feature-architecture.md |
| **Architect Decision:** | 1. Can MacroManager interface change with minimal code updates? |
| | 2. Are NVS schema + macro execution logic reusable? |
| | 3. Recommend: Modify Story 11.1 vs. Rollback + Re-implement |

**Phase 2: Development Execution (After Architect Guidance)**

| Role | Task |
|------|------|
| **Product Owner / Scrum Master** | Update sprint backlog per Architect recommendation |
| **Development Team** | Execute Story 11.1 modifications OR Stories 11.2-11.4 per guidance |

### Prerequisites for Development

⏳ **Story 11.1 (Infrastructure)** — COMPLETED but REQUIRES ASSESSMENT
   - MacroManager implemented with old interface (`setMacroButtonState()`)
   - **Pending:** Architect assessment on modification scope

✅ **Updated Story 11.2** — READY (acceptance criteria, tasks, dev notes finalized)

✅ **Architecture Update** — READY (MacroManager interface and patterns documented for toggle approach)

✅ **All Documents** — SYNCHRONIZED (PRD, Epic 11, Story 11.2, Architecture all updated)

### Success Criteria for Implementation

1. **Build succeeds** with no warnings
2. **Long-press detection** works reliably (≥500ms threshold)
3. **Short-press ignored** (no accidental mode switches)
4. **Toggle state persists** across menu navigation
5. **Priority order enforced** (Menu > Macro > Normal)
6. **Manual testing** confirms single-hand operation is practical

---

## 6. Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|-----------|
| Long-press threshold too sensitive | Low | Users accidentally toggle mode | Test threshold with varied hand sizes; document testing approach |
| Performance impact from timing code | Low | Latency on inputs | Simple timestamp math; no heap allocation |
| Macro mode state not persisting | Low | User confusion | Acceptance criteria explicitly test persistence across menu |
| Conflict with menu navigation | Low | Menu becomes hard to use | Priority order already defined (Menu > Macro) |

**Overall Risk Level:** 🟢 **LOW** — Early discovery, zero sunk implementation cost, straightforward long-press detection.

---

## 7. Timeline & Effort Estimate

| Phase | Status |
|-------|--------|
| **Analysis & Planning** | ✅ COMPLETE |
| **Document Updates** | ✅ COMPLETE |
| **Story 11.1 Assessment** | ⏳ PENDING (CRITICAL - Architect must assess modification scope) |
| **Architecture Validation** | ⏳ PENDING (Architect review of toggle pattern + Story 11.1 feasibility) |
| **Implementation** | 📋 READY (after Story 11.1 assessment) |

**Effort for Dev Team:**
- Story 11.2: Implement long-press detection + toggle logic
- Story 11.3: Configure menu (no changes, reuse AC from Story 11.1)
- Story 11.4: Initialize toggle state (simpler than original approach)

**No timeline extension expected** — toggle approach is slightly simpler than press-hold-release.

---

## 8. Approval & Routing

**Status:** CRITICAL - Awaiting Architect Assessment of Story 11.1

**Required Approvals & Actions:**
- [x] Product Manager: APPROVED (Feroj)
- ⏳ **Solution Architect: CRITICAL ASSESSMENT REQUIRED**
  - Review Story 11.1 implementation
  - Assess Story 11.1 modification feasibility
  - Validate macro-feature-architecture.md updates
  - Provide recommendation: Modify vs. Rollback+Re-implement
- [ ] Development Team: READY FOR HANDOFF (pending Architect guidance)

**Next Steps:**
1. ✅ Product Manager approves change proposal (Feroj)
2. ⏳ **[BLOCKING] Solution Architect assesses Story 11.1 + provides recommendation**
3. 📋 Development Team receives Architect guidance + executes accordingly
4. 🔄 Regular sync between teams during implementation

---

## 9. Related Documentation

- **Updated PRD:** `prd.md` (FR-10, FR-11, FR-12 added)
- **Updated Epic 11:** `epics/epic-11-macro-activation-system.md` (goal + FR rewrite)
- **Updated Story 11.2:** `implementation-artifacts/11-2-hardware-layer.md` (complete rewrite)
- **Updated Architecture:** `architecture/macro-feature-architecture.md` (MacroManager interface + patterns)

---

## Approval Sign-Off

| Role | Name | Date | Status | Action |
|------|------|------|--------|--------|
| Product Manager | Feroj | 2026-01-26 | ✅ APPROVED | — |
| Solution Architect | — | — | ⏳ CRITICAL | Assess Story 11.1 + recommend path |
| Scrum Master | — | — | 📋 READY | Execute per Architect guidance |

---

**Sprint Change Proposal Status:** ⏳ **APPROVED (by PM) + AWAITING ARCHITECT ASSESSMENT**

**Blocking Item:** Story 11.1 modification feasibility assessment by Architect

**Next Phase:**
1. Route to Solution Architect for Story 11.1 assessment
2. Architect provides recommendation (Modify vs. Rollback+Re-implement)
3. Route to Development Team for implementation per Architect guidance
