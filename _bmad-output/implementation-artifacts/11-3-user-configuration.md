# Story 11.3: User Configuration (Menu + SetMacroAction)

Status: backlog

## Story

As a **user wanting to configure macros**,
I want **to set key combinations for each input through the menu**,
So that **I can customize what each macro slot does**.

## Acceptance Criteria

1. **Given** I navigate to Settings in the menu
   **When** I select "Macros"
   **Then** I see a submenu with 7 items:
   - Wheel Button: [current combo or "None"]
   - Wheel Left: [current combo or "None"]
   - Wheel Right: [current combo or "None"]
   - Button 1: [current combo or "None"]
   - Button 2: [current combo or "None"]
   - Button 3: [current combo or "None"]
   - Button 4: [current combo or "None"]

2. **Given** I select an input to configure (e.g., "Wheel Button")
   **When** the SetMacroAction executes
   **Then** I can select modifier(s) (Ctrl, Shift, Alt, GUI/Cmd)
   **And** I can select a key (A-Z, 0-9, F1-F12, etc.)
   **And** the combination is saved to NVS
   **And** display confirms the save

3. **Given** I configure a macro as "Ctrl+C"
   **When** I view that slot in the menu later
   **Then** it displays "Ctrl+C" (not raw hex values)

4. **Given** I'm in macro configuration
   **When** setting modifiers and key
   **Then** valid combinations are enforced (not all combos allowed)
   **And** empty/clear option exists to remove macro

5. **Given** a macro is configured
   **When** the device reboots
   **Then** the configuration persists
   **And** the menu displays the saved value

6. **Given** the build is run
   **When** compilation completes
   **Then** build succeeds with no errors or warnings
   **And** menu integration compiles cleanly

## Tasks

- [ ] **Task 1: Add Macros Submenu to MenuTree** (AC: 1, 6)
  - [ ] Update `src/Menu/Model/MenuTree.h`:
    - [ ] Find Settings menu section
    - [ ] Add "Macros" submenu item
    - [ ] Under Macros, add 7 items:
      - [ ] "Wheel Button"
      - [ ] "Wheel Left"
      - [ ] "Wheel Right"
      - [ ] "Button 1"
      - [ ] "Button 2"
      - [ ] "Button 3"
      - [ ] "Button 4"
    - [ ] Each item points to SetMacroAction with target MacroInput
    - [ ] Display label format: "{Input Name}: {Combo or "None"}"

- [ ] **Task 2: Create SetMacroAction Class** (AC: 2, 4, 5, 6)
  - [ ] Create `src/Menu/Action/SetMacroAction.h`:
    - [ ] Extend MenuAction (or implement IMenuAction interface)
    - [ ] Members:
      - [ ] `MacroInput targetInput` (which slot to configure)
      - [ ] `MacroManager* macroManager` (for loading/saving)
      - [ ] `ConfigManager* configManager` (for NVS)
      - [ ] `MenuController* menuController` (for navigation)
      - [ ] `DisplayTask* display` (for feedback)
    - [ ] Constructor: `SetMacroAction(MacroInput input, MacroManager*, ConfigManager*, MenuController*, DisplayTask*)`
    - [ ] Method `void execute()` - implement macro configuration flow
  - [ ] Create `src/Menu/Action/SetMacroAction.cpp`:
    - [ ] Implement 3-step flow:
      - [ ] Step 1: Display current macro, ask to modify
      - [ ] Step 2: Modifier selection (multi-select: Ctrl, Shift, Alt, Gui)
      - [ ] Step 3: Key selection (A-Z, 0-9, F1-F12, Del, Enter, etc.)
    - [ ] After selection: Pack, save to NVS via ConfigManager, show confirmation
    - [ ] Provide "Clear" option to remove macro (set to 0x0000)

- [ ] **Task 3: Implement Modifier Selection UI** (AC: 2, 4, 6)
  - [ ] In SetMacroAction.cpp, implement modifier selection:
    - [ ] Display list of modifiers:
      - [ ] [ ] Ctrl
      - [ ] [ ] Shift
      - [ ] [ ] Alt
      - [ ] [ ] Gui (Cmd on Mac, Win on PC)
      - [ ] [ ] (none)
    - [ ] Use wheel to select/toggle modifiers
    - [ ] Use wheel button to confirm and go to key selection
    - [ ] Display: "Select modifiers" on screen
    - [ ] Show selected modifiers in real-time

- [ ] **Task 4: Implement Key Selection UI** (AC: 2, 3, 4, 6)
  - [ ] In SetMacroAction.cpp, implement key selection:
    - [ ] Define supported keys list:
      - [ ] Letters: A-Z
      - [ ] Numbers: 0-9
      - [ ] Function: F1-F12
      - [ ] Special: Enter, Backspace, Tab, Space, Delete
      - [ ] Arrows: Up, Down, Left, Right
      - [ ] Media: Next, Previous, Play, Mute, VolumeUp, VolumeDown
    - [ ] Use wheel to scroll through keys
    - [ ] Use wheel button to select
    - [ ] Display selected key with modifiers: "Ctrl+C"
    - [ ] Show available keys in menu format

- [ ] **Task 5: Pack and Save Macro** (AC: 2, 3, 5, 6)
  - [ ] After modifiers + key selected:
    - [ ] Create MacroDefinition with (modifiers, keycode)
    - [ ] Convert modifiers to HID bitmask (combine Ctrl, Shift, Alt, Gui flags)
    - [ ] Call `macroManager->saveMacro(configManager, targetInput, macroDef)`
    - [ ] Display confirmation: "Saved: Ctrl+C"
    - [ ] Return to macro slot menu
  - [ ] Handle clear/empty:
    - [ ] Create MacroDefinition(0, 0)
    - [ ] Save via macroManager
    - [ ] Display: "Macro cleared"

- [ ] **Task 6: Display Current Macro Value** (AC: 1, 3, 5)
  - [ ] When macro menu item renders (for display label):
    - [ ] Load current macro from macroManager
    - [ ] Convert to display string: "Ctrl+C" style
    - [ ] If empty: display "None"
    - [ ] Update menu item label dynamically
  - [ ] Create helper function `String formatMacroDisplay(MacroDefinition macro)`:
    - [ ] "Ctrl+C" for single modifier
    - [ ] "Ctrl+Shift+V" for multiple
    - [ ] "None" for empty

- [ ] **Task 7: Add Logging for Configuration** (AC: 2, 5, 6)
  - [ ] In SetMacroAction:
    - [ ] Log macro config start: LOG_DEBUG
    - [ ] Log selected modifiers: LOG_DEBUG
    - [ ] Log selected key: LOG_DEBUG
    - [ ] Log final packed value: LOG_INFO
    - [ ] Log save result: LOG_INFO

- [ ] **Task 8: Build and Verify Menu Integration** (AC: 1, 3, 5, 6)
  - [ ] Compile with `pio run -e use_nimble`
  - [ ] Verify no compile errors
  - [ ] Verify no warnings
  - [ ] Manual test: Navigate to Settings > Macros
  - [ ] Manual test: Enter macro configuration
  - [ ] Manual test: Select modifiers and key
  - [ ] Manual test: Verify save and persistence across reboot
  - [ ] Manual test: Verify menu displays saved combo

## Dev Notes

### Modifier Constants Mapping

Use existing BleKeyboard constants:
```cpp
KEY_LEFT_CTRL    // 0x01
KEY_LEFT_SHIFT   // 0x02
KEY_LEFT_ALT     // 0x04
KEY_LEFT_GUI     // 0x08
```

Combine with bitwise OR for multiple modifiers.

### Key Selection List

```cpp
enum class HIDKey : uint8_t {
    A = 0x04, B = 0x05, ..., Z = 0x1D,
    // Numbers, arrows, F-keys, etc.
};
```

Use BleKeyboard library constants or define your own mapping.

### Display Format Convention

```
"None"            // No macro
"Ctrl+C"          // Single modifier + key
"Ctrl+Shift+V"    // Multiple modifiers + key
"Shift+F2"        // Modifier + function key
```

### Menu Integration Pattern

MenuTree item structure:
```cpp
{
    title: "Wheel Button",
    action: new SetMacroAction(MacroInput::WHEEL_BUTTON, ...),
    display: [dynamic - shows "Ctrl+C" or "None"]
}
```

### Configuration Flow

```
Menu: Settings > Macros > Wheel Button (shows "None")
  ↓
User selects "Wheel Button"
  ↓
SetMacroAction.execute()
  ↓
Step 1: "Select modifiers" (display: no modifiers selected)
  → Wheel to toggle Ctrl, Shift, Alt, Gui
  → Wheel button to confirm
  ↓
Step 2: "Select key" (display: available keys list)
  → Wheel to scroll through A-Z, 0-9, F1-F12, etc.
  → Wheel button to select
  ↓
Step 3: Confirmation (display: "Saved: Ctrl+C")
  → Return to macro menu
  ↓
Menu now shows: "Wheel Button: Ctrl+C"
```

### Files to Create

```
src/Menu/Action/SetMacroAction.h
src/Menu/Action/SetMacroAction.cpp
```

### Files to Modify

```
src/Menu/Model/MenuTree.h (add Macros submenu)
```

### Key Design Decisions

- Modifier selection before key (logical flow)
- Wheel + wheel button for navigation (consistent with existing UI)
- Display strings in BleKeyboard format (human-readable)
- Clear/empty option to remove macros
- Persistence automatic (saved via ConfigManager)

### Anti-Patterns to Avoid

```cpp
// ❌ WRONG - Hardcoded modifier values
#define CTRL 0x01  // Use BleKeyboard's KEY_LEFT_CTRL

// ❌ WRONG - Display raw hex
display.show("0x0101");  // Should show "Ctrl+C"

// ❌ WRONG - No clear option
// User can't remove macro, must set to something

// ❌ WRONG - Direct NVS access in action
preferences.putUShort("macro.0", packed);  // Use ConfigManager/MacroManager

// ✅ CORRECT
MacroDefinition macro{KEY_LEFT_CTRL, 'C'};
macroManager->saveMacro(configManager, MacroInput::WHEEL_BUTTON, macro);
display.show("Saved: Ctrl+C");
```

### Testing Approach

1. **Menu Navigation Test**:
   - Go to Settings > Macros
   - Verify all 7 items visible
   - Verify labels show "None" initially

2. **Configuration Flow Test**:
   - Select Wheel Button
   - Choose Ctrl modifier
   - Choose 'C' key
   - Verify saves as "Ctrl+C"

3. **Display Update Test**:
   - Configure macro
   - Exit menu, re-enter
   - Verify "Wheel Button: Ctrl+C" displays
   - Reboot device
   - Verify persistence

4. **Clear Macro Test**:
   - Configure a macro
   - Use clear option
   - Verify "None" displays again

## Dev Agent Record

Status: backlog - ready for implementation

**Story 11.3 covers:** Menu structure, SetMacroAction implementation, modifier/key selection UI, persistence verification.

**Key Components:**
- MenuTree: 7 macro items under Settings > Macros
- SetMacroAction: 3-step (modifiers → key → save) configuration flow
- Display formatter: Show "Ctrl+C" style strings
- Loader: Fetch current value from MacroManager for display

**Dependencies:** Requires Story 11.1 (MacroManager complete) and Story 11.2 (event handling complete)

---
