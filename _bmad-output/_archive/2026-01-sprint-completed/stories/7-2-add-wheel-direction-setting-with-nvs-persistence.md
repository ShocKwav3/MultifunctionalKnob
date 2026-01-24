# Story 7.2: Add Wheel Direction Setting with NVS Persistence

Status: done

## Story

As a **user who prefers inverted scroll direction**,
I want **to select Normal or Reversed wheel direction from the menu**,
so that **the wheel rotates in my preferred direction and the setting persists**.

## Acceptance Criteria

1. **Given** I navigate to Wheel Behavior → Wheel Direction
   **When** I view the options
   **Then** I see "Normal" and "Reversed" as selectable items
   **And** the currently selected direction is visually highlighted

2. **Given** I select "Reversed" (or "Normal")
   **When** I confirm the selection by pressing the wheel button
   **Then** the selection is saved to NVS with key "wheel.dir"
   **And** I see confirmation feedback ("Saved" or similar)
   **And** the menu exits or returns to parent menu

3. **Given** the setting is saved to NVS
   **When** I cycle power (reboot the device)
   **Then** the direction setting is restored from NVS
   **And** the menu shows the correct selected direction

4. **Given** the NVS contains corrupted data
   **When** the system reads the wheel direction
   **Then** it falls back to the default value (NORMAL)
   **And** logs a warning about the corruption

5. **Given** the implementation is complete
   **When** I build the project
   **Then** the build succeeds with no errors or warnings

## Tasks

- [x] **Task 1: Define WheelDirection Enum and Constants** (AC: 1, 4)
  - [x] Create `include/Enum/WheelDirection.h` with enum:
    ```cpp
    enum class WheelDirection : uint8_t {
        NORMAL = 0,
        REVERSED = 1
    };
    ```
  - [x] Add `KEY_WHEEL_DIR` constant to `include/Config/device_config.h`:
    ```cpp
    constexpr const char* KEY_WHEEL_DIR = "wheel.dir";
    ```
  - [x] Add default value constant:
    ```cpp
    constexpr WheelDirection DEFAULT_WHEEL_DIR = WheelDirection::NORMAL;
    ```

- [x] **Task 2: Update ConfigManager with Wheel Direction Methods** (AC: 2, 3, 4)
  - [x] Add `getWheelDirection()` method to `ConfigManager.h`:
    ```cpp
    WheelDirection getWheelDirection() const;
    ```
  - [x] Add `setWheelDirection()` method to `ConfigManager.h`:
    ```cpp
    Error setWheelDirection(WheelDirection direction);
    ```
  - [x] Implement `getWheelDirection()` in `ConfigManager.cpp`:
    - [x] Use `readEnumConfig` pattern with validation
    - [x] Pass `DEFAULT_WHEEL_DIR` as fallback
    - [x] Pass `WheelDirection::REVERSED` as max valid value
  - [x] Implement `setWheelDirection()` in `ConfigManager.cpp`:
    - [x] Cast enum to `uint8_t` for NVS storage
    - [x] Use `preferences.putUChar()` with `KEY_WHEEL_DIR`
    - [x] Return `Error::OK` on success, `Error::NVS_WRITE_FAIL` on failure

- [x] **Task 3: Create SelectWheelDirectionAction Class** (AC: 2)
  - [x] Create `src/Menu/Action/SelectWheelDirectionAction.h`:
    - [x] Inherit from `MenuAction`
    - [x] Store `WheelDirection targetDirection` member
    - [x] Constructor: `explicit SelectWheelDirectionAction(WheelDirection dir)`
    - [x] Override `execute()` method
    - [x] Override `getConfirmationMessage()` method
  - [x] Create `src/Menu/Action/SelectWheelDirectionAction.cpp`:
    - [x] Implement `execute()`:
      - [x] Call `ConfigManager::setWheelDirection(targetDirection)`
      - [x] Log result with `LOG_INFO` or `LOG_ERROR`
    - [x] Implement `getConfirmationMessage()`:
      - [x] Return "Direction saved" or similar

- [x] **Task 4: Update MenuTree.h with Wheel Direction Items** (AC: 1)
  - [x] Populate `wheelDirectionSubmenu` with two items:
    - [x] "Normal" with `SelectWheelDirectionAction(WheelDirection::NORMAL)`
    - [x] "Reversed" with `SelectWheelDirectionAction(WheelDirection::REVERSED)`
  - [x] Update `initWheelBehaviorActions()` to initialize the two action instances
  - [x] Ensure parent pointers are set correctly (from Story 7.1)

- [x] **Task 5: Build and Verify** (AC: all)
  - [x] Build with `pio run -e use_nimble`
  - [x] Verify no compile errors
  - [ ] Manual test: Navigate to Wheel Direction → Select "Reversed" → Verify "Saved" message
  - [ ] Manual test: Reboot device → Verify "Reversed" is still selected
  - [ ] Manual test: Select "Normal" → Verify setting changes

### Review Follow-ups (User Feedback)

- [x] **Task 6: Fix KEY_WHEEL_DIR vs KEY_WHEEL_MODE Inconsistency**
  - [x] Move `KEY_WHEEL_MODE` from `ConfigManager.h` (private) to `device_config.h` (public)
  - [x] Remove `KEY_WHEEL_MODE` constant from `ConfigManager.h` private section

- [x] **Task 7: Fix wheelDirectionSubmenu Magic Strings**
  - [x] Replace hardcoded strings with `wheelDirectionToDisplayString()` calls
  - [x] Follow `wheelModeSubmenu` pattern for consistency

- [x] **Task 8: Fix buttonBehaviorItems Magic Strings**
  - [x] Add `buttonActionToDisplayString()` helper to `ButtonActionEnum.h`
  - [x] Replace hardcoded strings in `buttonBehaviorItems` with enum-derived labels

## Dev Notes

### Architecture Compliance

- **NVS Pattern**: Use `readEnumConfig` helper for safe enum retrieval with validation
- **Action Pattern**: Create dedicated `MenuAction` subclass following Command pattern
- **Naming**: Key "wheel.dir" follows prefixed key pattern from architecture
- **Error Handling**: All fallible operations return `Error` enum

### NVS Key Structure

```cpp
// include/Config/device_config.h
constexpr const char* KEY_WHEEL_DIR = "wheel.dir";
constexpr WheelDirection DEFAULT_WHEEL_DIR = WheelDirection::NORMAL;
```

### Enum Storage Pattern

```cpp
// From architecture/core-architectural-decisions.md#Data Architecture
template<typename EnumT>
EnumT readEnumConfig(const char* key, EnumT defaultVal, EnumT maxValid) {
    uint8_t stored = preferences.getUChar(key, static_cast<uint8_t>(defaultVal));
    return (stored <= static_cast<uint8_t>(maxValid))
           ? static_cast<EnumT>(stored)
           : defaultVal;
}

// Usage in ConfigManager.cpp
WheelDirection ConfigManager::getWheelDirection() const {
    return readEnumConfig<WheelDirection>(
        KEY_WHEEL_DIR,
        DEFAULT_WHEEL_DIR,
        WheelDirection::REVERSED  // Max valid value
    );
}
```

### Action Class Pattern

```cpp
// From architecture/core-architectural-decisions.md#Command Pattern for Actions
class SelectWheelDirectionAction : public MenuAction {
    WheelDirection targetDirection;
public:
    explicit SelectWheelDirectionAction(WheelDirection dir) : targetDirection(dir) {}
    void execute() override {
        Error result = configManager->setWheelDirection(targetDirection);
        if (result == Error::OK) {
            LOG_INFO("WheelDir", "Direction saved: %d", static_cast<int>(targetDirection));
        } else {
            LOG_ERROR("WheelDir", "Failed to save direction: %d", static_cast<int>(result));
        }
    }
    const char* getConfirmationMessage() const override {
        return "Direction saved";
    }
};
```

### Existing Code Locations

```
include/Config/device_config.h          - Add KEY_WHEEL_DIR constant
include/Enum/WheelDirection.h            - New enum file
src/Config/ConfigManager.h/cpp           - Add getter/setter methods
src/Menu/Action/SelectWheelDirectionAction.h/cpp  - New action class
src/Menu/Model/MenuTree.h                - Update menu items
```

### Key Files to Modify

| File | Change |
|------|--------|
| `include/Enum/WheelDirection.h` | New enum definition |
| `include/Config/device_config.h` | Add KEY_WHEEL_DIR and DEFAULT_WHEEL_DIR constants |
| `src/Config/ConfigManager.h` | Add method declarations |
| `src/Config/ConfigManager.cpp` | Implement getter/setter with NVS logic |
| `src/Menu/Action/SelectWheelDirectionAction.h` | New action class header |
| `src/Menu/Action/SelectWheelDirectionAction.cpp` | New action class implementation |
| `src/Menu/Model/MenuTree.h` | Update menu items and action initialization |

### Testing Approach

1. **Build Verification**: Compile succeeds with no errors
2. **NVS Persistence Test**:
   - Set direction to "Reversed"
   - Reboot device
   - Verify "Reversed" is still selected
3. **Corruption Recovery Test**:
   - Manually corrupt NVS value (via test code or direct NVS write)
   - Reboot device
   - Verify fallback to "Normal" (default)
4. **UI Feedback Test**:
   - Select direction
   - Verify confirmation message displays
   - Verify menu exits after selection

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Direct NVS access without validation
uint8_t dir = preferences.getUChar("wheel.dir", 0);
return static_cast<WheelDirection>(dir);  // Could return invalid enum

// ❌ WRONG - No error handling
void setWheelDirection(WheelDirection dir) {
    preferences.putUChar("wheel.dir", static_cast<uint8_t>(dir));
}

// ❌ WRONG - Hardcoded key string
preferences.getUChar("wheel.dir", ...);  // Should use KEY_WHEEL_DIR constant

// ✅ CORRECT - With validation and error handling
WheelDirection getWheelDirection() const {
    return readEnumConfig<WheelDirection>(
        KEY_WHEEL_DIR,
        DEFAULT_WHEEL_DIR,
        WheelDirection::REVERSED
    );
}

Error setWheelDirection(WheelDirection dir) {
    if (!preferences.putUChar(KEY_WHEEL_DIR, static_cast<uint8_t>(dir))) {
        return Error::NVS_WRITE_FAIL;
    }
    return Error::OK;
}
```

### References

- [Source: architecture/core-architectural-decisions.md#Data Architecture] - NVS namespace and enum storage pattern
- [Source: architecture/core-architectural-decisions.md#Command Pattern for Actions] - Action class structure
- [Source: architecture/implementation-patterns-consistency-rules.md#Format Patterns] - Error return pattern
- [Source: project-context.md#Configuration Persistence Gotchas] - NVS write validation and corruption handling
- [Source: epics.md#Story 7.2] - Original acceptance criteria

## Change Log

- **2026-01-07**: Addressed user feedback (3 consistency issues):
  - Fixed KEY_WHEEL_MODE location inconsistency (moved to device_config.h)
  - Replaced wheelDirectionSubmenu magic strings with enum-derived labels
  - Replaced buttonBehaviorItems magic strings with enum-derived labels
  - Added buttonActionToDisplayString() helper function
  - Build verified successful

## Dev Agent Record

### Agent Model Used

Claude 3.7 Sonnet (claude-sonnet-4-20250514)

### Implementation Notes

All tasks completed successfully:

**Task 1**: Created WheelDirection enum following existing enum patterns (WheelModeEnum, ErrorEnum). Placed DEFAULT_WHEEL_DIR constant in enum file to avoid circular dependency between device_config.h and ConfigManager.h.

**Task 2**: Implemented ConfigManager methods following existing saveWheelMode/loadWheelMode pattern. getWheelDirection() uses validation against WheelDirection_MAX, returns DEFAULT_WHEEL_DIR on corruption. setWheelDirection() validates input, writes to NVS, returns Error enum.

**Task 3**: Created SelectWheelDirectionAction following SelectWheelModeAction pattern. Simple action class - takes WheelDirection in constructor, persists via ConfigManager, returns "Direction saved" message.

**Task 4**: Updated MenuTree.h to initialize wheel direction actions in initWheelBehaviorActions(). Added setWheelDirectionAction() setter function. Actions assigned using WheelDirection enum values as indices.

**Task 5**: Build succeeded with no errors. Manual hardware testing deferred - requires physical device.

**Task 6 (Review Follow-up)**: Fixed KEY_WHEEL_MODE inconsistency. Moved KEY_WHEEL_MODE from ConfigManager.h private section to device_config.h public section, matching KEY_WHEEL_DIR pattern. Improves consistency and makes all NVS keys discoverable in single location.

**Task 7 (Review Follow-up)**: Replaced wheelDirectionSubmenu magic strings with wheelDirectionToDisplayString() calls. Now matches wheelModeSubmenu pattern - labels derived from enum ensure alignment between menu items and action indices.

**Task 8 (Review Follow-up)**: Added buttonActionToDisplayString() helper function to ButtonActionEnum.h. Updated buttonBehaviorItems to use enum-derived labels instead of magic strings. Ensures menu labels always match ButtonAction enum values.

### Completion Notes

✅ All acceptance criteria implementation complete
✅ Build passes with no errors
✅ Code follows project patterns and architecture
✅ NVS persistence with validation and corruption handling
✅ Menu integration complete with confirmation feedback
✅ User feedback addressed - eliminated magic strings and NVS key inconsistency
⚠️ Manual hardware testing requires physical device (AC 2, 3 UI verification)

### Files Modified

**Original Implementation:**
- include/Enum/WheelDirection.h (new)
- include/Config/device_config.h
- src/Config/ConfigManager.h
- src/Config/ConfigManager.cpp
- src/Menu/Action/SelectWheelDirectionAction.h (new)
- src/Menu/Action/SelectWheelDirectionAction.cpp (new)
- src/Menu/Model/MenuTree.h

**Review Follow-ups:**
- include/Config/device_config.h (added KEY_WHEEL_MODE)
- src/Config/ConfigManager.h (removed private KEY_WHEEL_MODE)
- include/Enum/ButtonActionEnum.h (added buttonActionToDisplayString)
- src/Menu/Model/MenuTree.h (fixed magic strings in wheelDirectionSubmenu and buttonBehaviorItems)
