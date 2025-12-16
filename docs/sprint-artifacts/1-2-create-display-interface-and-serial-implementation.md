# Story 1.2: Create Display Interface and Serial Implementation

Status: ready-for-dev

## Story

As a **user**,
I want **to see device output via serial monitor**,
so that **I can observe menu navigation, status messages, and configuration feedback**.

## Acceptance Criteria

1. **Display Interface:**
   - Create `src/Display/Interface/DisplayInterface.h`
   - Include `<Arduino.h>` or `<stdint.h>` for types
   - Define abstract class with pure virtual methods:
     - `showMenu(const char* title, const char** items, uint8_t count, uint8_t selected)`
     - `showMessage(const char* message)`
     - `showConfirmation(const char* message)`
     - `showStatus(const char* key, const char* value)`
     - `clear()`

2. **Serial Implementation:**
   - Create `src/Display/Impl/SerialDisplay.cpp` and `SerialDisplay.h`
   - Include `<Arduino.h>` for `HardwareSerial`
   - Implement `DisplayInterface`
   - `showMenu()`: Output formatted menu with `>` for selection
   - `showMessage()`: Output `[MSG] {message}`
   - `showConfirmation()`: Output `[OK] {message}`
   - `showStatus()`: Output `{key}: {value}`
   - `clear()`: Output separator line (e.g., `---`)

3. **Initialization:**
   - Constructor accepts `HardwareSerial*` (default `&Serial`)
   - Use `LOG_INFO` for initialization confirmation

## Tasks / Subtasks

- [ ] Create `src/Display/Interface/DisplayInterface.h`
- [ ] Create `src/Display/Impl/SerialDisplay.h`
- [ ] Create `src/Display/Impl/SerialDisplay.cpp`

## Dev Notes

### Architecture Compliance

- **Directory Structure:**
  - `src/Display/Interface/`
  - `src/Display/Impl/`
- **Naming:** `DisplayInterface`, `SerialDisplay`
- **Logging:** Use `LOG_INFO`, `LOG_DEBUG` from `log_config.h`

### References

- [Architecture: Component Architecture](docs/architecture.md#component-architecture-menu-system)
- [Epics: Story 1.2](docs/epics.md#story-12-create-display-interface-and-serial-implementation)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview
