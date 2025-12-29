# Sprint Change Proposal: Distributed Event Architecture

**Date:** 2025-12-20
**Trigger:** Code Review (SRP Violation in `AppEventHandler`)
**Author:** Feroj (User) / Architect Agent

## 1. Issue Summary

**Problem:** The original design channeled all application logic through a single `AppEventHandler` and a monolithic `AppEvent` union. This violates the Single Responsibility Principle (SRP), creating a "God Object" that couples unrelated domains (Menu logic, System state, BLE status).

**Discovery:** Identified during code review of the foundation phase.

**Impact:** Low maintainability, high coupling, and risk of future complexity explosions as new features (e.g., LED effects) are added.

## 2. Recommended Approach: Option B (Split Queues)

We will pivot from a **Centralized** event architecture to a **Distributed** one.

*   **Decoupled Pipelines:** Separate queues for `Menu`, `System`, and `Input` events.
*   **Specialized Handlers:** `MenuEventHandler` handles only UI logic; `SystemEventHandler` handles device state.
*   **Display Arbitration:** A dedicated `DisplayTask` (consuming a `DisplayRequestQueue`) will manage the single physical screen resource, preventing race conditions between the parallel handlers.

## 3. Impact Analysis

| Artifact | Status | Impact Description |
| :--- | :--- | :--- |
| **Architecture** | **MAJOR** | Complete rewrite of "Event Architecture" section. Addition of "Display Arbitration". |
| **Epics** | **MODERATE** | Epic 2 (Menu) and Epic 5 (Status) stories need to target specific queues instead of generic AppEvent. |
| **Code** | **MODERATE** | Delete `AppEventHandler`. Create `MenuEventHandler`, `SystemEventHandler`, `DisplayTask`. |
| **Tests** | **LOW** | No automated tests exist yet (per "Speed to Market" pivot). |

## 4. Detailed Change Proposals

### 4.1. Architecture Document Changes

**Section: Component Architecture**

*   **OLD:** `AppEventDispatcher` → `AppEventQueue` → `AppEventHandler` (God Handler)
*   **NEW:**
    *   `MenuDispatcher` → `MenuEventQueue` → `MenuEventHandler` (Logic)
    *   `SystemDispatcher` → `SystemEventQueue` → `SystemEventHandler` (State)
    *   *Arbitration:* Both Handlers → `DisplayRequestQueue` → `DisplayTask` (Hardware Access)

**Section: Component Boundaries**

*   **OLD:** Input Layer → Processing Layer → Output Layer
*   **NEW:**
    *   **Input Layer:** `EncoderDriver` / `ButtonManager`
    *   **Domain Layer:** `MenuController` (Logic), `EncoderModeManager` (Logic)
    *   **Output Layer:** `MenuEventHandler` (Formatting), `SystemEventHandler` (Formatting)
    *   **Hardware Layer:** `DisplayTask` (Rendering), `BleKeyboard`

### 4.2. Epic Updates

**Epic 2: Menu Navigation System**

*   **Story 2.3 (Integration):** Change `AppEventDispatcher` to `MenuEventDispatcher`.
*   **Story 2.4 (Display Handler):**
    *   **Rename to:** "Create Menu Event Handler and Display Arbitrator"
    *   **Scope:** Implement `MenuEventHandler` to format menu text, then emit render commands to `DisplayRequestQueue`.

**Epic 5: Device Status**

*   **Story 5.1 (Show Status):** Use `SystemEventDispatcher` to broadcast status updates.

## 5. Implementation Handoff

**Classification:** **Major Scope** (Architectural Pivot)

**Deliverables:**
1.  Updated `_bmad-output/architecture/index.md` (COMPLETE)
2.  Updated `_bmad-output/epics/index.md` (COMPLETE)
3.  Implementation of `DisplayTask` and specialized Dispatchers (PENDING - To be done by Dev Agent).

**Success Criteria:**
*   No `AppEvent` class exists in the codebase.
*   Menu navigation works smoothly without blocking System status updates.
*   Display output is corruption-free (verified via Serial logs initially).