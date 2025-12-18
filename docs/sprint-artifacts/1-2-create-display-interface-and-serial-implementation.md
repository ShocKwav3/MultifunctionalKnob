# Story 1.2: Create Display Interface and Serial Implementation

Status: Done

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

- [x] Create `src/Display/Interface/DisplayInterface.h`
- [x] Create `src/Display/Impl/SerialDisplay.h`
- [x] Create `src/Display/Impl/SerialDisplay.cpp`

### Review Follow-ups (AI)
- [x] [AI-Review][CRITICAL] Implement unit tests for DisplayInterface and SerialDisplay [test/] - REMOVED from scope (Testable code required, but no unit tests)
- [x] [AI-Review][HIGH] Add null pointer checks in showMenu [src/Display/Impl/SerialDisplay.cpp]
- [x] [AI-Review][MEDIUM] Refactor to use Serial directly instead of Stream* injection (User Request) [src/Display/Impl/SerialDisplay.cpp]
- [x] [AI-Review][LOW] Use ternary operator in showMenu (User Request) [src/Display/Impl/SerialDisplay.cpp]
- [x] [AI-Review][LOW] Add early return/guard clause in showMenu (User Request) [src/Display/Impl/SerialDisplay.cpp]

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
- claude-opus-4-5-20251101 (implementation)

### Implementation Plan

1. Created directory structure: `src/Display/Interface/` and `src/Display/Impl/`
2. Implemented `DisplayInterface` abstract class with pure virtual methods per AC 1
3. Implemented `SerialDisplay` class inheriting from `DisplayInterface` per AC 2
4. Used `Stream*` instead of `HardwareSerial*` for ESP32-C3 USB CDC compatibility

### Debug Log

- Initial build failed: ESP32-C3 uses `HWCDC` (USB CDC) for `Serial`, not `HardwareSerial`
- Fixed by using `Stream*` base class which both `HardwareSerial` and `HWCDC` inherit from
- Final build successful

### Completion Notes

- All acceptance criteria satisfied:
  - AC 1: `DisplayInterface` created with all required pure virtual methods
  - AC 2: `SerialDisplay` implements all methods with correct output formatting
  - AC 3: Default constructor uses `Serial` directly, uses `LOG_INFO` for init
- Note: Simplified from `Stream*` injection to direct `Serial` usage per code review
- Note: Testing strategy updated - Unit tests are not required at this stage, but code must remain testable (interfaces, dependency injection where appropriate) for future test implementation.

## File List

- `src/Display/Interface/DisplayInterface.h` (new)
- `src/Display/Impl/SerialDisplay.h` (new)
- `src/Display/Impl/SerialDisplay.cpp` (new)

## Change Log

- 2025-12-18: Story 1.2 implemented - Display interface and serial implementation created
- 2025-12-18: Code review follow-ups addressed:
  - Added null pointer checks and early return guard clause in showMenu
  - Refactored to use Serial directly (removed Stream* injection)
  - Used ternary operator for selection marker in showMenu
  - Unit tests deferred per user request (pending PM discussion)
