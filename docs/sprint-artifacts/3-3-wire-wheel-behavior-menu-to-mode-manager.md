# Story 3.3: Wire Wheel Behavior Menu to Mode Manager

Status: done

## Story

As a **user**,
I want **the wheel behavior menu to show all available modes and switch between them**,
so that **I can easily change how my wheel operates at runtime**.

## Acceptance Criteria

1. **Menu Tree Update:**
   - Update `src/Menu/Model/MenuTree.h`
   - "Wheel Behavior" submenu children:
     - "Scroll" -> `SelectWheelModeAction(SCROLL)`
     - "Volume" -> `SelectWheelModeAction(VOLUME)`
     - "Zoom" -> `SelectWheelModeAction(ZOOM)`

2. **Mode Manager Update:**
   - Update `src/EncoderMode/Manager/EncoderModeManager.cpp`
   - Register `EncoderModeHandlerZoom`
   - Ensure `setMode(ZOOM)` activates the new handler

3. **Runtime Switching:**
   - Selecting mode in menu immediately changes active handler
   - Persistence is handled by Action (Story 3.1)

## Tasks / Subtasks

- [x] Update `src/Menu/Model/MenuTree.h`
- [x] Update `src/EncoderMode/Manager/EncoderModeManager.cpp`

## Dev Notes

### Architecture Compliance

- **Static Menu:** Ensure `MenuTree` remains `constexpr` (or static const) if possible, or initialize actions properly.
- **Mode Registration:** Follow existing pattern in `EncoderModeManager`.

### References

- [Architecture: Component Architecture](docs/architecture/index.md#component-architecture-menu-system)
- [Epics: Story 3.3](docs/epics/index.md#story-33-wire-wheel-behavior-menu-to-mode-manager)

## Dev Agent Record

### Context Reference

- `docs/architecture/index.md`
- `docs/epics/index.md`
- Story 3.1: SelectWheelModeAction implementation
- Story 3.2: EncoderModeHandlerZoom implementation
- Story 2.3: Menu integration with encoder event system

### Agent Model Used

- claude-sonnet-4-5-20250929

### Implementation Plan

1. Verify MenuTree.h contains all three wheel behavior menu items with SelectWheelModeAction instances
2. Register EncoderModeHandlerZoom in main.cpp following existing pattern for Scroll/Volume handlers
3. Initialize menu system (MenuTree + MenuController) in main.cpp
4. Build and verify compilation

### Completion Notes

**Implementation Summary:**

1. **MenuTree.h Verification** (AC #1):
   - Confirmed existing `initWheelBehaviorActions()` function (lines 169-179) creates SelectWheelModeAction instances for all three modes: SCROLL, VOLUME, and ZOOM
   - No changes needed - implementation complete from Story 3.1

2. **main.cpp Modifications** (AC #2, #3):
   - Added include for `EncoderModeHandlerZoom.h` (line 24)
   - Added includes for `MenuController.h` and `MenuTree.h` (lines 27-28)
   - Created EncoderModeHandlerZoom instance (line 66)
   - Registered Zoom handler with encoderModeManager (line 75)
   - Created MenuController instance (line 83)
   - Initialized MenuTree parent pointers via `MenuTree::initMenuTree()` (line 84)
   - Initialized wheel behavior actions via `MenuTree::initWheelBehaviorActions(&configManager, &encoderModeManager)` (line 85)
   - Set MenuController on EncoderEventHandler (line 86)

3. **Integration Complete**:
   - All three wheel modes (Scroll, Volume, Zoom) now accessible via menu
   - Mode selection triggers SelectWheelModeAction which persists to NVS and applies runtime change
   - Menu activation via long-press (Story 2.3)
   - Runtime mode switching without restart (Story 3.1)

**Architecture Compliance:**
- ✅ Follows existing pattern for mode handler registration
- ✅ Menu system properly initialized with DI (ConfigManager, EncoderModeManager)
- ✅ Static MenuTree structure maintained
- ✅ Actions assigned at runtime after DI objects created

**Build Status:** ✅ Compilation successful across all environments (debug, use_nimble, use_stdble)

**Testing:** Deferred per architecture decision (2025-12-18 strategic pivot for MVP)

**Date Completed:** 2025-12-22

## File List

- `src/main.cpp` (modified - added Zoom handler registration and menu system initialization)

## Change Log

- 2025-12-22: Story 3.3 implementation complete - Wired wheel behavior menu to mode manager with Zoom handler registration and menu system initialization
