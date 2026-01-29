# Story 11.1: Required Interface Modifications for Toggle Behavior

**Status:** Documentation Update (Pre-Implementation Planning)
**Date:** 2026-01-26
**Purpose:** Document the minimal interface refactoring needed in Story 11.1 to support toggle-based macro activation

---

## Overview

Story 11.1 (Macro Infrastructure) has been completed with a **data layer** implementation that stores, loads, and executes macros. However, the original implementation assumed a **press-hold-release** behavior model, where MacroManager tracked GPIO button state continuously.

**Recent Decision:** Switch to **toggle-based activation** model (long-press to toggle ON/OFF) to improve UX and single-hand operability.

**Scope of Changes:** The toggle model requires **minimal interface refactoring** to Story 11.1. The core macro logic, NVS persistence, and BLE execution remain unchanged and reusable.

---

## Required Modifications Summary

| Component | Current | New | Impact | Effort |
|-----------|---------|-----|--------|--------|
| Member variable | `bool macroButtonHeld` | `bool macroModeActive` | Semantic clarity | 1 line |
| Public method | `setMacroButtonState(bool held)` | `toggleMacroMode()` | Toggle pattern | 1 line |
| Implementation | `macroButtonHeld = held;` | `macroModeActive = !macroModeActive;` | Toggle logic | 1 line |
| Logging | "Button state: %s" | "Macro mode toggled: %s" | Clarity | 1 line |
| Usage examples | Several examples updated | Updated to use toggle pattern | Docs only | ~5 lines |

**Total Changes:** ~10 lines across 3 files
**Reusable Code:** 95% of Story 11.1 implementation untouched

---

## Detailed Modifications

### File 1: `src/Macro/Manager/MacroManager.h`

**Change 1: Member Variable**
```cpp
// OLD
private:
    bool macroButtonHeld;  // Suggests button GPIO state

// NEW
private:
    bool macroModeActive;  // Clarifies toggle state
```

**Change 2: Method Declaration**
```cpp
// OLD
public:
    void setMacroButtonState(bool held);

// NEW
public:
    void toggleMacroMode();
```

**No changes needed to:**
- `bool isMacroModeActive() const`
- `bool executeMacro(MacroInput input)`
- `void loadFromNVS(ConfigManager& config)`
- `Error saveMacro(ConfigManager& config, MacroInput input, MacroDefinition macro)`

---

### File 2: `src/Macro/Manager/MacroManager.cpp`

**Change 1: Method Implementation**
```cpp
// OLD
void MacroManager::setMacroButtonState(bool held) {
    macroButtonHeld = held;
    LOG_DEBUG("MacroManager", "Button state: %s", held ? "held" : "released");
}

// NEW
void MacroManager::toggleMacroMode() {
    macroModeActive = !macroModeActive;
    LOG_INFO("MacroManager", "Macro mode toggled: %s", macroModeActive ? "ON" : "OFF");
}
```

**No changes needed to:**
- `MacroManager::loadFromNVS()`
- `MacroManager::executeMacro()`
- `MacroManager::saveMacro()`
- Constructor implementation
- Any NVS-related code

---

### File 3: Dev Notes (Documentation Updates)

**Update MacroManager Usage Example**
```cpp
// OLD
macroManager.setMacroButtonState(true);  // Button pressed
if (macroManager.isMacroModeActive()) {
    macroManager.executeMacro(MacroInput::WHEEL_LEFT);
}
macroManager.setMacroButtonState(false);  // Button released

// NEW
macroManager.toggleMacroMode();  // Long-press detected → toggle ON
if (macroManager.isMacroModeActive()) {
    macroManager.executeMacro(MacroInput::WHEEL_LEFT);
}
macroManager.toggleMacroMode();  // Long-press detected again → toggle OFF
```

**Update Anti-Patterns to Avoid**
- Add example of old pattern (tracking button state) as anti-pattern
- Emphasize toggle pattern as correct approach

---

## Architectural Principle

This refactoring maintains **clean separation of concerns**:

| Responsibility | Owner | Change Impact |
|---|---|---|
| **Data Layer** (store, load, execute macros) | Story 11.1 | ✅ No change |
| **Behavior Control** (how button activates mode) | Story 11.2 | Uses new toggle interface |

**Story 11.1** handles the *what* (macros as data)
**Story 11.2** handles the *how* (long-press detection + toggle)

This boundary makes the toggle adoption clean and localized.

---

## Implementation Checklist

Before proceeding with Story 11.2 development:

- [ ] Update `MacroManager.h`: Rename `macroButtonHeld` → `macroModeActive`
- [ ] Update `MacroManager.h`: Replace method `setMacroButtonState()` → `toggleMacroMode()`
- [ ] Update `MacroManager.cpp`: Implement toggle logic (flip boolean with !)
- [ ] Update `MacroManager.cpp`: Update logging messages
- [ ] Update Story 11.1 dev notes: Usage examples
- [ ] Update Story 11.1 dev notes: Anti-patterns section
- [ ] Rebuild: `pio run -e use_nimble`
- [ ] Verify: No compile errors or warnings
- [ ] Verify: All original acceptance criteria (AC 1-5) still pass

---

## Verification

After modifications:

1. **Compilation**
   ```bash
   pio run -e use_nimble
   ```
   Expected: Clean build, no errors or warnings

2. **Interface Contract**
   - `isMacroModeActive()` returns correct boolean state
   - `executeMacro()` still works when macro mode is active
   - NVS persistence unchanged
   - BLE execution unchanged

3. **Integration Ready**
   - Story 11.2 can now call `macroManager->toggleMacroMode()` on long-press
   - All data layer functionality preserved for Stories 11.3 and 11.4

---

## Why This is Minimal

The original implementation was **well-architected** for this change:

1. ✅ **Clear interface boundary** — `isMacroModeActive()` abstracted the state source
2. ✅ **Dependency injection** — BleKeyboard passed in, no coupling to button handling
3. ✅ **Static allocation** — No dynamic memory to refactor
4. ✅ **Pure logic functions** — `executeMacro()` and persistence unchanged
5. ✅ **No external consumers yet** — Only Story 11.2 uses the interface (not yet implemented)

This is why toggle adoption is a simple refactor, not an architectural overhaul.

---

## Next Steps

1. ✅ **Documentation complete** (this file)
2. 📋 **Developer executes** Story 11.1 modifications per implementation checklist
3. 📋 **Rebuild and verify** no regressions
4. 📋 **Proceed with Story 11.2** implementation (long-press detection + toggle calling)

---

