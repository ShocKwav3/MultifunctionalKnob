# Story 6.3: Evaluate and Optimize Button Input Handling

Status: ready-for-dev

## Story

As a **developer maintaining the codebase**,
I want **button input handling reviewed for efficiency**,
so that **the code uses the simplest approach that works reliably**.

## Acceptance Criteria

1. **Given** buttons currently use `buttonManager->poll()` in `main.cpp::loop()`
   **When** I investigate whether event-driven input (like the wheel) is simpler
   **Then** I document the finding with a clear recommendation

2. **Given** the investigation is complete
   **When** event-driven is simpler
   **Then** implement event-driven approach

3. **Given** the investigation is complete
   **When** poll() is simpler
   **Then** keep poll() and document why

4. **Given** a decision is made
   **Then** the decision prioritizes simplicity over forced consistency

## Tasks

- [ ] **Task 1: Document current implementation** (AC: 1)
  - [ ] Review `src/Button/ButtonManager.cpp` poll() implementation
  - [ ] Review `src/main.cpp` loop() button handling
  - [ ] Document how encoder event-driven approach works for comparison

- [ ] **Task 2: Analyze trade-offs** (AC: 1)
  - [ ] **Poll-based advantages:**
    - Simpler implementation (no ISR complexity)
    - Predictable timing in main loop
    - Easier debugging
  - [ ] **Event-driven advantages:**
    - Immediate response
    - Consistent with encoder pattern
    - Lower power (no continuous polling)

- [ ] **Task 3: Make recommendation** (AC: 1, 4)
  - [ ] Document decision with rationale
  - [ ] Consider ESP32-C3 single-core constraint
  - [ ] Prioritize simplicity (KISS principle)

- [ ] **Task 4: Implement if change needed** (AC: 2, 3)
  - [ ] If keeping poll(): Add code comment explaining decision
  - [ ] If switching to event-driven: Implement ISR-based button handling

- [ ] **Task 5: Build and test** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Test button responsiveness

## Dev Notes

### Current Implementation Analysis

**ButtonManager.poll() approach:**
```cpp
// main.cpp loop()
buttonManager->poll();  // Called every loop iteration
```

**EncoderDriver event-driven approach:**
```cpp
// Uses hardware interrupts via ESP32 PCNT
// Events dispatched to queue from ISR
```

### Decision Framework

| Factor | Poll-Based | Event-Driven |
|--------|------------|--------------|
| Complexity | Low | Medium (ISR handling) |
| Latency | ~10-50ms (loop dependent) | <1ms |
| Power | Higher (continuous polling) | Lower |
| Debugging | Easier | Harder (ISR context) |
| Single-Core Safety | Safer | Needs careful ISR design |

### Recommendation Guidance

**Keep Poll-Based IF:**
- Current latency is acceptable (<50ms per NFR1)
- No user complaints about button responsiveness
- Code is simpler and easier to maintain
- Single-core ESP32-C3 benefits from predictable execution

**Switch to Event-Driven IF:**
- Latency is unacceptable
- Power consumption is critical
- Consistency with encoder is more important than simplicity

### ESP32-C3 Considerations

- **Single-core RISC-V**: ISRs block main execution
- **GPIO interrupt capability**: Available but adds complexity
- **FreeRTOS integration**: Would need queue from ISR context

### Documentation Template

If keeping poll():
```cpp
// ButtonManager uses poll-based input handling.
// Decision rationale (Story 6.3):
// - Simpler than ISR-based approach
// - Acceptable latency for button inputs (~10-50ms)
// - Easier debugging and maintenance
// - Safe for single-core ESP32-C3
```

### References

- [Source: project-context.md#ESP32-C3 Specific Gotchas] - Single-core constraints
- [Source: project-context.md#Input Handling Gotchas] - Debounce handled by EncoderDriver
- [Source: architecture/core-architectural-decisions.md#Communication Patterns] - Event pipeline patterns

## Dev Agent Record

### Agent Model Used

Claude Opus 4.5

### Completion Notes

### Files Modified
