# Story 7.2: Add Wheel Direction Setting with NVS Persistence

Status: ready-for-dev

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

- [ ] **Task 1: Define WheelDirection Enum and Constants** (AC: 1, 4)
  - [ ] Create `include/Enum/WheelDirection.h` with enum:
    ```cpp
    enum class WheelDirection : uint8_t {
        NORMAL = 0,
        REVERSED = 1
    };
    ```
  - [ ] Add `KEY_WHEEL_DIR` constant to `include/Config/device_config.h`:
    ```cpp
    constexpr const char* KEY_WHEEL_DIR = "wheel.dir";
    ```
  - [ ] Add default value constant:
    ```cpp
    constexpr WheelDirection DEFAULT_WHEEL_DIR = WheelDirection::NORMAL;
    ```

- [ ] **Task 2: Update ConfigManager with Wheel Direction Methods** (AC: 2, 3, 4)
  - [ ] Add `getWheelDirection()` method to `ConfigManager.h`:
    ```cpp
    WheelDirection getWheelDirection() const;
    ```
  - [ ] Add `setWheelDirection()` method to `ConfigManager.h`:
    ```cpp
    Error setWheelDirection(WheelDirection direction);
    ```
  - [ ] Implement `getWheelDirection()` in `ConfigManager.cpp`:
    - [ ] Use `readEnumConfig` pattern with validation
    - [ ] Pass `DEFAULT_WHEEL_DIR` as fallback
    - [ ] Pass `WheelDirection::REVERSED` as max valid value
  - [ ] Implement `setWheelDirection()` in `ConfigManager.cpp`:
    - [ ] Cast enum to `uint8_t` for NVS storage
    - [ ] Use `preferences.putUChar()` with `KEY_WHEEL_DIR`
    - [ ] Return `Error::OK` on success, `Error::NVS_WRITE_FAIL` on failure

- [ ] **Task 3: Create SelectWheelDirectionAction Class** (AC: 2)
  - [ ] Create `src/Menu/Action/SelectWheelDirectionAction.h`:
    - [ ] Inherit from `MenuAction`
    - [ ] Store `WheelDirection targetDirection` member
    - [ ] Constructor: `explicit SelectWheelDirectionAction(WheelDirection dir)`
    - [ ] Override `execute()` method
    - [ ] Override `getConfirmationMessage()` method
  - [ ] Create `src/Menu/Action/SelectWheelDirectionAction.cpp`:
    - [ ] Implement `execute()`:
      - [ ] Call `ConfigManager::setWheelDirection(targetDirection)`
      - [ ] Log result with `LOG_INFO` or `LOG_ERROR`
    - [ ] Implement `getConfirmationMessage()`:
      - [ ] Return "Direction saved" or similar

- [ ] **Task 4: Update MenuTree.h with Wheel Direction Items** (AC: 1)
  - [ ] Populate `wheelDirectionSubmenu` with two items:
    - [ ] "Normal" with `SelectWheelDirectionAction(WheelDirection::NORMAL)`
    - [ ] "Reversed" with `SelectWheelDirectionAction(WheelDirection::REVERSED)`
  - [ ] Update `initWheelBehaviorActions()` to initialize the two action instances
  - [ ] Ensure parent pointers are set correctly (from Story 7.1)

- [ ] **Task 5: Build and Verify** (AC: all)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Navigate to Wheel Direction → Select "Reversed" → Verify "Saved" message
  - [ ] Manual test: Reboot device → Verify "Reversed" is still selected
  - [ ] Manual test: Select "Normal" → Verify setting changes

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

## Dev Agent Record

### Agent Model Used

GLM-4.7 (regenerated for quality consistency)

### Completion Notes

### Files Modified
