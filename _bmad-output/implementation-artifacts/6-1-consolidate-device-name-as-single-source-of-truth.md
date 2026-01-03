# Story 6.1: Consolidate Device Name as Single Source of Truth

Status: ready-for-dev

## Story

As a **developer maintaining the codebase**,
I want **the device Bluetooth name defined in one place (`device_config.h`) and referenced everywhere else**,
so that **I can change the device identity in a single location without hunting for duplicates**.

## Acceptance Criteria

1. **Given** the device name is defined in `include/Config/device_config.h`
   **When** the NVS namespace is initialized in `ConfigManager`
   **Then** it derives the namespace from the device name constant (not hardcoded "knobkoky")

2. **Given** the device name constant exists in `device_config.h`
   **When** searching the entire codebase for hardcoded device name strings
   **Then** no other file contains a hardcoded device name (all reference the constant)

3. **Given** the change is implemented
   **When** building the project
   **Then** the build succeeds with no errors or warnings related to the change

## Tasks

- [ ] **Task 1: Audit existing device name usage** (AC: 2)
  - [ ] Search codebase for "knobkoky" and any BLE device name strings
  - [ ] Document all locations where device name is hardcoded
  - [ ] Identify ConfigManager NVS namespace initialization

- [ ] **Task 2: Define device name constant** (AC: 1)
  - [ ] Verify `include/Config/device_config.h` has `DEVICE_NAME` constant
  - [ ] If not present, add: `constexpr const char* DEVICE_NAME = "KnobKoky";`
  - [ ] Add NVS namespace constant derived from device name

- [ ] **Task 3: Update ConfigManager to use constant** (AC: 1)
  - [ ] Modify `src/Config/ConfigManager.cpp` to use device name constant for NVS namespace
  - [ ] Replace hardcoded "knobkoky" with reference to constant

- [ ] **Task 4: Update any other hardcoded references** (AC: 2)
  - [ ] Update BLE initialization to use `DEVICE_NAME` constant
  - [ ] Update any display strings referencing device name

- [ ] **Task 5: Verify build and test** (AC: 3)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Manual test: Confirm BLE advertising name is correct

## Dev Notes

### Architecture Compliance

- **Single Source of Truth Pattern**: All configuration constants belong in `include/Config/` headers
- **Naming Convention**: Use `UPPER_SNAKE_CASE` for constants (e.g., `DEVICE_NAME`, `NVS_NAMESPACE`)
- **Include Order**: Project config headers come last in include order

### Existing Code Locations

```
include/Config/device_config.h  - Primary location for device identity
src/Config/ConfigManager.cpp    - NVS namespace initialization
src/main.cpp                    - BLE keyboard initialization
```

### NVS Namespace Pattern

Per architecture, use single namespace with prefixed keys:
```cpp
// include/Config/device_config.h
constexpr const char* DEVICE_NAME = "KnobKoky";
constexpr const char* NVS_NAMESPACE = "knobkoky";  // lowercase, derived from DEVICE_NAME

// src/Config/ConfigManager.cpp
preferences.begin(NVS_NAMESPACE, false);
```

### Key Files to Modify

| File | Change |
|------|--------|
| `include/Config/device_config.h` | Add/verify `DEVICE_NAME` and `NVS_NAMESPACE` constants |
| `src/Config/ConfigManager.cpp` | Use `NVS_NAMESPACE` constant instead of hardcoded string |
| `src/main.cpp` | Ensure BLE name uses `DEVICE_NAME` constant |

### Testing Approach

1. Build verification (compile succeeds)
2. Manual test: Check BLE advertising shows correct device name
3. Manual test: Verify NVS read/write works correctly

### References

- [Source: architecture/core-architectural-decisions.md#Data Architecture] - NVS namespace pattern
- [Source: architecture/implementation-patterns-consistency-rules.md#Naming Patterns] - Constant naming
- [Source: project-context.md#Configuration Persistence Gotchas] - NVS namespace rules

## Dev Agent Record

### Agent Model Used

Claude Opus 4.5

### Completion Notes

### Files Modified
