# Story 3.3: Wire Wheel Behavior Menu to Mode Manager

Status: ready-for-dev

## Story

As a **user**,
I want **the wheel behavior menu to show all available modes and switch between them**,
so that **I can easily change how my wheel operates at runtime**.

## Acceptance Criteria

1. **Menu Tree Update:**
   - Update `src/Menu/Model/MenuTree.h`
   - "Wheel Behavior" submenu children:
     - "Scroll" -> `SetWheelModeAction(SCROLL)`
     - "Volume" -> `SetWheelModeAction(VOLUME)`
     - "Zoom" -> `SetWheelModeAction(ZOOM)`

2. **Mode Manager Update:**
   - Update `src/EncoderMode/Manager/EncoderModeManager.cpp`
   - Register `EncoderModeHandlerZoom`
   - Ensure `setMode(ZOOM)` activates the new handler

3. **Runtime Switching:**
   - Selecting mode in menu immediately changes active handler
   - Persistence is handled by Action (Story 3.1)

## Tasks / Subtasks

- [ ] Update `src/Menu/Model/MenuTree.h`
- [ ] Update `src/EncoderMode/Manager/EncoderModeManager.cpp`

## Dev Notes

### Architecture Compliance

- **Static Menu:** Ensure `MenuTree` remains `constexpr` (or static const) if possible, or initialize actions properly.
- **Mode Registration:** Follow existing pattern in `EncoderModeManager`.

### References

- [Architecture: Component Architecture](docs/architecture.md#component-architecture-menu-system)
- [Epics: Story 3.3](docs/epics.md#story-33-wire-wheel-behavior-menu-to-mode-manager)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
