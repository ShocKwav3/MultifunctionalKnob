# Story 6.2: Consolidate Button Names with Position-Based Identifiers

Status: ready-for-dev

## Story

As a **user configuring buttons via the menu**,
I want **buttons labeled by position (Top Left, Top Right, Bottom Left, Bottom Right)**,
so that **I can easily identify which physical button I'm configuring**.

## Acceptance Criteria

1. **Given** button configuration exists in `include/Config/button_config.h`
   **When** I view the button list in the menu
   **Then** I see position-based names: "Top Left", "Top Right", "Bottom Left", "Bottom Right"

2. **Given** button names are defined in `button_config.h`
   **When** `MenuTree.h` needs button labels
   **Then** it references `button_config.h` for names (no duplication)

3. **Given** the position-based names are implemented
   **When** comparing names to physical button layout
   **Then** the mapping matches the physical button positions on the device

4. **Given** the change is implemented
   **When** building the project
   **Then** the build succeeds with no errors

## Tasks

- [ ] **Task 1: Update button_config.h with position-based names** (AC: 1, 3)
  - [ ] Modify `BUTTONS[]` array labels from "Button 1-4" to position names
  - [ ] Verify GPIO mapping matches physical layout:
    - GPIO 3 → Top Left
    - GPIO 4 → Top Right
    - GPIO 5 → Bottom Left
    - GPIO 9 → Bottom Right

- [ ] **Task 2: Verify MenuTree.h references button_config.h** (AC: 2)
  - [ ] Check if `MenuTree.h` has hardcoded button names
  - [ ] If yes, refactor to reference `BUTTONS[].label` from `button_config.h`
  - [ ] Ensure no duplicate button name strings exist

- [ ] **Task 3: Update any display code referencing button names** (AC: 1)
  - [ ] Search for "Button 1", "Button 2", etc. in codebase
  - [ ] Replace with references to `BUTTONS[i].label`

- [ ] **Task 4: Build and verify** (AC: 4)
  - [ ] Build with `pio run -e use_nimble`
  - [ ] Verify no compile errors

## Dev Notes

### Architecture Compliance

- **Button Configuration Pattern** (from architecture):
```cpp
// include/Config/button_config.h
struct ButtonConfig {
    uint8_t pin;
    const char* label;
    bool activeLow;
};

inline constexpr ButtonConfig BUTTONS[] = {
    {3, "Top Left", true},
    {4, "Top Right", true},
    {5, "Bottom Left", true},
    {9, "Bottom Right", true}
};
```

### Physical Layout Mapping

```
Device Front View:
┌─────────────────┐
│  [TL]    [TR]   │  TL = GPIO 3, TR = GPIO 4
│                 │
│    [ENCODER]    │
│                 │
│  [BL]    [BR]   │  BL = GPIO 5, BR = GPIO 9
└─────────────────┘
```

### Key Files to Modify

| File | Change |
|------|--------|
| `include/Config/button_config.h` | Update labels to position-based names |
| `src/Menu/Model/MenuTree.h` | Reference `BUTTONS[].label` if hardcoded |

### Naming Convention

- Use human-readable position names: "Top Left", "Top Right", "Bottom Left", "Bottom Right"
- Keep labels short for display constraints (128x32 OLED in future)

### Anti-Patterns to Avoid

```cpp
// WRONG - Hardcoded in multiple places
const char* MENU_BUTTON_NAMES[] = {"Button 1", "Button 2", ...};

// CORRECT - Single source in button_config.h
for (size_t i = 0; i < BUTTON_COUNT; i++) {
    menuItem.label = BUTTONS[i].label;
}
```

### References

- [Source: architecture/core-architectural-decisions.md#Communication Patterns] - Button configuration pattern
- [Source: epics.md#Story 6.2] - Original acceptance criteria
- [Source: project-context.md#Hardware Configuration] - GPIO pin mapping

## Dev Agent Record

### Agent Model Used

Claude Opus 4.5

### Completion Notes

### Files Modified
