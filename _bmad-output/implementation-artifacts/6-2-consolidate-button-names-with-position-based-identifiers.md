# Story 6.2: Consolidate Button Names with Position-Based Identifiers

Status: done

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

- [x] **Task 1: Update button_config.h with position-based names** (AC: 1, 3)
  - [x] Modify `BUTTONS[]` array labels from "Button 1-4" to position names
  - [x] Verify GPIO mapping matches physical layout:
    - GPIO 3 → Top Left
    - GPIO 4 → Top Right
    - GPIO 5 → Bottom Left
    - GPIO 9 → Bottom Right

- [x] **Task 2: Verify MenuTree.h references button_config.h** (AC: 2)
  - [x] Check if `MenuTree.h` has hardcoded button names
  - [x] If yes, refactor to reference `BUTTONS[].label` from `button_config.h`
  - [x] Ensure no duplicate button name strings exist

- [x] **Task 3: Update any display code referencing button names** (AC: 1)
  - [x] Search for "Button 1", "Button 2", etc. in codebase
  - [x] Replace with references to `BUTTONS[i].label`

- [x] **Task 4: Build and verify** (AC: 4)
  - [x] Build with `pio run -e use_nimble`
  - [x] Verify no compile errors

### Review Follow-ups (AI)
- [x] [AI-Review][MEDIUM] `BUTTON_CONFIG_COUNT` is hardcoded to 4, independent of `BUTTON_COUNT` in `button_config.h` [src/Menu/Model/MenuTree.h:103]
  - **FIXED**: Removed `BUTTON_CONFIG_COUNT` constant, replaced all usages with `BUTTON_COUNT` from button_config.h
- [x] [AI-Review][LOW] Stale comment still references "(Button 1)" [src/Menu/Model/MenuTree.h:129]
  - **FIXED**: Updated comment to "(Top Left)" to match position-based naming
- [x] [AI-Review][LOW] Log output uses button index instead of position-based label [src/Menu/Action/SetButtonBehaviorAction.cpp:28]
  - **FIXED**: Changed log format from "Button %d" to "%s" using BUTTONS[buttonIndex].label
- [x] [AI-Review][HIGH] extractButtonIndex returns 0 on failure, risking silent overwrite of Button 0 [src/Menu/Action/SetButtonBehaviorAction.cpp:52]
  - **FIXED**: Changed return type to int8_t, returns -1 on failure. Added validation in execute() to abort on invalid index.
- [x] [AI-Review][MEDIUM] buttonConfigSubmenu array size is hardcoded to 4, risking overflow if BUTTON_COUNT increases [src/Menu/Model/MenuTree.h:103]
  - **FIXED**: Changed array declaration to use BUTTON_COUNT for size: `MenuItem buttonConfigSubmenu[BUTTON_COUNT]`

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

**Implementation Summary:**
All tasks completed successfully. Button names consolidated to position-based identifiers with BUTTONS[] as single source of truth.

**Key Changes:**
1. **button_config.h**: Updated labels from "Button 1-4" to "Top Left", "Top Right", "Bottom Left", "Bottom Right"
2. **MenuTree.h**: 
   - Added `#include "Config/button_config.h"`
   - Removed hardcoded button labels from buttonConfigSubmenu[] initialization
   - Modified initMenuTree() to assign labels from BUTTONS[].label at runtime
   - Updated documentation comments to reflect position-based names
   - **[Code Review Fix]** Removed hardcoded `BUTTON_CONFIG_COUNT`, replaced with `BUTTON_COUNT` from button_config.h
   - **[Code Review Fix]** Updated stale comment from "(Button 1)" to "(Top Left)"
3. **SetButtonBehaviorAction.cpp**: 
   - Refactored extractButtonIndex() to match labels against BUTTONS[].label instead of parsing "Button X" format
   - **[Code Review Fix]** Updated log output to use button label instead of index: `BUTTONS[buttonIndex].label`
4. **SetButtonBehaviorAction.h**: Updated documentation comments to reflect new label matching approach

**Architecture Compliance:**
- Followed KISS principle - simple label matching instead of string parsing
- Maintained single source of truth pattern - all button labels come from button_config.h
- No duplication - eliminated hardcoded button name strings AND count constants
- Runtime initialization pattern consistent with existing MenuTree architecture

**Build Verification:**
✓ Project builds successfully with `pio run -e use_nimble`
✓ All acceptance criteria satisfied
✓ All code review issues resolved

### Files Modified

- include/Config/button_config.h
- src/Menu/Model/MenuTree.h
- src/Menu/Action/SetButtonBehaviorAction.h
- src/Menu/Action/SetButtonBehaviorAction.cpp

## Change Log

- 2026-01-03: Story implementation completed - consolidated button names to position-based identifiers with single source of truth pattern
- 2026-01-03: Code review follow-ups completed - fixed BUTTON_CONFIG_COUNT duplication, updated stale comments, improved log output with button labels
- 2026-01-03: Critical code review fixes - resolved HIGH severity bug (extractButtonIndex now returns -1 on failure with validation), fixed MEDIUM severity array overflow risk (buttonConfigSubmenu uses BUTTON_COUNT)
