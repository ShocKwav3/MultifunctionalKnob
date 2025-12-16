# Story 1.1: Create Foundation Enums and Configuration Headers

Status: ready-for-dev

## Story

As a **developer**,
I want **standardized enums, error codes, and logging macros**,
so that **all subsequent components have consistent foundations to build upon**.

## Acceptance Criteria

1. **Error Handling:**
   - Create `include/Enum/ErrorEnum.h`
   - Define `enum class Error : uint8_t` with values: `OK`, `INVALID_PARAM`, `NVS_READ_FAIL`, `NVS_WRITE_FAIL`, `INVALID_STATE`
   - Use UPPER_SNAKE_CASE for values
   - Include `const char* errorToString(Error e)` helper function for debugging

2. **Wheel Mode Enum:**
   - Create `include/Enum/WheelModeEnum.h`
   - Define `enum class WheelMode : uint8_t` with values: `SCROLL`, `VOLUME`, `ZOOM`
   - Include `WheelMode_MAX` constant for validation
   - Include `const char* wheelModeToString(WheelMode m)` helper function for debugging

3. **Button Action Enum:**
   - Create `include/Enum/ButtonActionEnum.h`
   - Define `enum class ButtonAction : uint8_t` with values: `NONE`, `MUTE`, `PLAY`, `PAUSE`, `NEXT`, `PREVIOUS`
   - Include `ButtonAction_MAX` constant for validation
   - Include `const char* buttonActionToString(ButtonAction a)` helper function for debugging

4. **Logging Configuration:**
   - Create `include/Config/log_config.h`
   - Include `<Arduino.h>` to ensure Serial availability
   - Define `LOG_ERROR(tag, format, ...)`, `LOG_INFO(tag, format, ...)`, `LOG_DEBUG(tag, format, ...)` macros
   - Log output format: `[LEVEL][Tag] Message`
   - Log level controllable via build flag `LOG_LEVEL` (0=NONE, 1=ERROR, 2=INFO, 3=DEBUG)

5. **Button Configuration:**
   - Create `include/Config/button_config.h`
   - Define `struct ButtonConfig { uint8_t pin; const char* label; bool activeLow; }`
   - Define `constexpr ButtonConfig BUTTONS[]` array with 4 default buttons (Pins 3, 4, 5, 8 as per architecture)
   - Define `constexpr size_t BUTTON_COUNT`

## Tasks / Subtasks

- [ ] Create `include/Enum/ErrorEnum.h`
- [ ] Create `include/Enum/WheelModeEnum.h`
- [ ] Create `include/Enum/ButtonActionEnum.h`
- [ ] Create `include/Config/log_config.h`
- [ ] Create `include/Config/button_config.h`

## Dev Notes

### Architecture Compliance

- **Naming Conventions:**
  - Files: PascalCase (e.g., `ErrorEnum.h`)
  - Enums: PascalCase (e.g., `Error`)
  - Enum Values: UPPER_SNAKE_CASE (e.g., `INVALID_PARAM`)
  - Macros: UPPER_SNAKE_CASE (e.g., `LOG_INFO`)
- **Header Guards:** Use `#pragma once` in all header files.
- **Directory Structure:**
  - `include/Enum/` for enums
  - `include/Config/` for configuration headers

### Technical Requirements

- **Logging Macros:**
  - Should use `Serial.printf` or similar for formatting.
  - Must check `LOG_LEVEL` before printing.
  - Example:
    ```cpp
    #if LOG_LEVEL >= 3
    #define LOG_DEBUG(tag, format, ...) Serial.printf("[DBG][%s] " format "\n", tag, ##__VA_ARGS__)
    #else
    #define LOG_DEBUG(tag, format, ...)
    #endif
    ```

- **Button Config:**
  - Pins: 3, 4, 5, 8 (based on architecture example, verify against hardware docs if needed, but architecture says 3,4,5,8).
  - Active Low: Typically true for buttons with internal pull-ups.

### References

- [Architecture: Implementation Patterns](docs/architecture.md#implementation-patterns--consistency-rules)
- [Architecture: Build Order Dependencies](docs/architecture.md#build-order-dependencies)
- [Epics: Story 1.1](docs/epics.md#story-11-create-foundation-enums-and-configuration-headers)

## Dev Agent Record

### Context Reference

- `docs/architecture.md`
- `docs/epics.md`

### Agent Model Used

- google/gemini-3-pro-preview

### Completion Notes List

- This is the foundational step. Ensure strict adherence to naming conventions as these files will be included everywhere.
