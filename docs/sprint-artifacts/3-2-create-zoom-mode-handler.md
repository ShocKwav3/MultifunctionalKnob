# Story 3.2: Create Zoom Mode Handler

Status: ready-for-dev

## Story

As a **user**,
I want **to use Zoom mode to send zoom in/out commands when rotating the wheel**,
so that **I can zoom in applications like browsers, image editors, and IDEs**.

## Acceptance Criteria

1. **Zoom Handler Class:**
   - Create `src/EncoderMode/Handler/EncoderModeHandlerZoom.cpp` and `EncoderModeHandlerZoom.h`
   - Inherit from `EncoderModeHandlerAbstract`
   - Constructor accepts `BleKeyboard*`

2. **Rotation Logic:**
   - `handleRotation(delta)`:
     - Positive delta: Send `Ctrl + +` (Zoom In)
     - Negative delta: Send `Ctrl + -` (Zoom Out)
     - Repeat `abs(delta)` times

3. **BLE Commands:**
   - Use `KEY_LEFT_CTRL` modifier
   - Ensure modifier is released

## Tasks / Subtasks

- [ ] Create `src/EncoderMode/Handler/EncoderModeHandlerZoom.h`
- [ ] Create `src/EncoderMode/Handler/EncoderModeHandlerZoom.cpp`

## Dev Notes

### Architecture Compliance

- **Handler Pattern:** Must follow existing `EncoderModeHandler` interface.
- **Consistency:** Match implementation style of `EncoderModeHandlerScroll` and `EncoderModeHandlerVolume`.

### References

- [Architecture: Gap Analysis](docs/architecture.md#gap-analysis-results)
- [Epics: Story 3.2](docs/epics.md#story-32-create-zoom-mode-handler)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
