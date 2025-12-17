# Story 1.1: Create Foundation Enums and Configuration Headers

Status: Done

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

- [x] Create `include/Enum/ErrorEnum.h`
- [x] Create `include/Enum/WheelModeEnum.h`
- [x] Create `include/Enum/ButtonActionEnum.h`
- [x] Create `include/Config/log_config.h`
- [x] Create `include/Config/button_config.h`

### Review Follow-ups (AI)

- [x] [AI-Review][HIGH] Hardware Risk: GPIO 8 is a strapping pin (Download Mode). Change Button 4 pin. [include/Config/button_config.h]
- [~] [AI-Review][MEDIUM] Code Quality: LOG_ macros restrict format strings. [include/Config/log_config.h] - Won't fix (intentional for safety)
- [~] [AI-Review][MEDIUM] Code Size: BUTTONS array duplication in header. [include/Config/button_config.h] - Won't fix (acceptable)
- [x] [AI-Review][LOW] Consistency: Missing Error_MAX. [include/Enum/ErrorEnum.h]
- [~] [AI-Review][LOW] Best Practice: LOG_ macros lack do-while(0). [include/Config/log_config.h] - Won't fix (single statements)

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
- ✅ All 5 header files created following architecture naming conventions
- ✅ All enums use `enum class` with `: uint8_t` base type
- ✅ All enum values use UPPER_SNAKE_CASE
- ✅ Helper functions (`errorToString`, `wheelModeToString`, `buttonActionToString`) implemented as `inline` functions
- ✅ `WheelMode_MAX` and `ButtonAction_MAX` constants defined for validation
- ✅ Logging macros use `Serial.printf` with configurable `LOG_LEVEL`
- ✅ Button config uses `constexpr` for compile-time initialization with pins 3, 4, 5, 9
- ✅ Build verified: Project compiles successfully with all new headers
- ✅ Resolved review finding [HIGH]: Changed Button 4 pin from GPIO 8 to GPIO 9 (strapping pin risk)
- ✅ Resolved review finding [LOW]: Added Error_MAX constant for consistency with other enums

## File List

- `include/Enum/ErrorEnum.h` (new)
- `include/Enum/WheelModeEnum.h` (new)
- `include/Enum/ButtonActionEnum.h` (new)
- `include/Config/log_config.h` (new)
- `include/Config/button_config.h` (new)

## Change Log

- 2025-12-16: Created foundation enums (Error, WheelMode, ButtonAction) and configuration headers (log_config, button_config) per Story 1.1 acceptance criteria
- 2025-12-17: Addressed code review findings - changed Button 4 from GPIO 8 (strapping pin) to GPIO 9, added Error_MAX constant for consistency
