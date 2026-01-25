# Adhoc Task: Button Action Mapping Refactor

Status: done

## Story

As a **developer maintaining the button configuration system**,
I want **a single source of truth for button actions mapped to BleKeyboard media keys**,
So that **adding new actions only requires updating one file, and the menu/handlers automatically reflect changes**.

## Context

**Problem:**
- Button actions were hardcoded in multiple locations (enum, handler switch, display strings, menu items)
- Adding new media key support required touching 5+ files
- ButtonAction enum was tightly coupled to ButtonEventHandler execution logic
- Initial refactor created 6 abstraction files (ButtonAction/definition.h, types.h, lookup.h/cpp, format.h/cpp) with void* casts and unnecessary indirection

**Solution:**
- Created `BleKeyboardService` - a facade/service layer that encapsulates both the action mapping AND execution logic
- Single file implementation (`BleKeyboardService.cpp`) contains all button action definitions
- Injected service into all consumers via dependency injection
- Eliminated transitive BleKeyboard.h coupling in headers while maintaining clean dependencies

## Implementation Summary

### Files Created (2)
- `src/BLE/BleKeyboardService.h` - Service interface with forward declarations
- `src/BLE/BleKeyboardService.cpp` - Service implementation with action mapping table

### Files Modified (18)
- `src/Config/ConfigManager.h/cpp` - Inject BleKeyboardService for action validation
- `src/Event/Handler/ButtonEventHandler.h/cpp` - Inject service, delegate execution
- `src/Menu/Action/SetButtonBehaviorAction.h/cpp` - Inject service for display strings
- `src/Menu/Action/ShowStatusAction.h/cpp` - Inject service for status display
- `src/Menu/Model/MenuTree.h` - Generate menu items from service
- `src/main.cpp` - Instantiate BleKeyboardService, inject into dependencies
- `include/Enum/ButtonActionEnum.h` - Deleted (replaced by service)
- Other files updated for consistency

### Key Changes

**1. Service Layer Pattern:**
```cpp
// BleKeyboardService.cpp - Single source of truth
static const MediaKeyAction MEDIA_KEY_ACTIONS[] = {
    { 0, "NONE",         "None",           nullptr,                   "Button Cleared" },
    { 1, "PLAY_PAUSE",   "Play/Pause",     &KEY_MEDIA_PLAY_PAUSE,     "Play/Pause Assigned" },
    { 2, "NEXT",         "Next Track",     &KEY_MEDIA_NEXT_TRACK,     "Next Track Assigned" },
    { 3, "PREVIOUS",     "Previous Track", &KEY_MEDIA_PREVIOUS_TRACK, "Previous Track Assigned" },
    { 4, "MUTE",         "Mute",           &KEY_MEDIA_MUTE,           "Mute Assigned" },
    { 5, "VOLUME_UP",    "Volume Up",      &KEY_MEDIA_VOLUME_UP,      "Volume Up Assigned" },
    { 6, "VOLUME_DOWN",  "Volume Down",    &KEY_MEDIA_VOLUME_DOWN,    "Volume Down Assigned" },
    { 7, "STOP",         "Stop",           &KEY_MEDIA_STOP,           "Stop Assigned" }
};
// Add new action = ONE LINE here, menu auto-updates ✓
```

**2. Dependency Injection:**
```cpp
// main.cpp
BleKeyboard bleKeyboard(...);
BleKeyboardService bleKeyboardService(&bleKeyboard);
ConfigManager configManager(&preferences, &bleKeyboardService);
ButtonEventHandler buttonEventHandler(..., &bleKeyboardService, ...);
```

**3. Simplified Execution:**
```cpp
// ButtonEventHandler - Before (20+ lines)
const ButtonActionDefinition* actionDef = getButtonActionById(actionId);
if (!actionDef) { /* error */ }
if (!actionDef->mediaKey) { /* skip */ }
bleKeyboard->write(*actionDef->mediaKey);
LOG_INFO(...);

// ButtonEventHandler - After (3 lines)
if (!bleKeyboardService->executeMediaKey(actionId)) {
    LOG_DEBUG("Action %d execution failed or skipped", actionId);
}
```

**4. Clean API:**
```cpp
class BleKeyboardService {
public:
    bool executeMediaKey(ButtonActionId actionId);
    bool isConnected() const;
    const char* getActionDisplayName(ButtonActionId actionId) const;
    const char* getActionConfirmation(ButtonActionId actionId) const;
    const char* getActionIdentifier(ButtonActionId actionId) const;
    uint8_t getActionCount() const;
    bool isValidActionId(ButtonActionId actionId) const;
    ButtonActionId getActionIdByIndex(uint8_t index) const;
};
```

## Technical Design

### Architecture Principles

- **Service Layer Pattern**: Encapsulates both data (action mapping) and behavior (BLE execution)
- **Dependency Injection**: Service injected into ConfigManager, ButtonEventHandler, MenuTree, SetButtonBehaviorAction
- **Single Responsibility**: BleKeyboardService handles all BLE media key concerns
- **Data-Driven**: No switch statements, all logic driven by mapping table
- **Open/Closed**: Add actions by extending table, no handler modifications needed

### Execution Flow

```
Button Press
    ↓
ButtonEventHandler.executeButtonAction(buttonIndex)
    ↓
actionId = actionCache[buttonIndex]  (from NVS cache)
    ↓
bleKeyboardService->executeMediaKey(actionId)
    ↓
Service validates ID, checks connection, executes BLE write
    ↓
Returns true (success) or false (failed/skipped)
```

### Menu Generation Flow

```
main.cpp initialization
    ↓
MenuTree::initButtonBehaviorMenuItems(&bleKeyboardService)
    ↓
for (i = 0; i < ACTION_COUNT; i++) {
    actionId = service->getActionIdByIndex(i);
    buttonBehaviorItems[i].label = service->getActionDisplayName(actionId);
}
    ↓
Menu automatically reflects all actions in service
```

### Memory Impact

- **Before**: 6 ButtonAction files, void* abstractions, forwarding functions
- **After**: 2 files (BleKeyboardService.h/cpp), direct MediaKeyReport pointers
- **ROM Delta**: ~+200 bytes (service table) - 400 bytes (removed abstraction) = **Net -200 bytes**
- **RAM Delta**: 0 bytes (same cache size, no new allocations)

## Benefits Achieved

✅ **Extensibility**: Add action = one line in service .cpp, menu auto-updates
✅ **Maintainability**: All action definitions in BleKeyboardService.cpp (single source of truth)
✅ **Decoupling**: Headers don't transitively include BleKeyboard.h (except service header)
✅ **Testability**: Service can be mocked for unit testing handlers
✅ **Simplicity**: No void* casts, no 6-file abstraction layer
✅ **Type Safety**: Direct MediaKeyReport* types, validated at compile time
✅ **Embedded-Friendly**: Static const array, no dynamic allocation, minimal overhead

## Comparison: Before vs After

| Aspect | Before (Over-engineered) | After (BleKeyboardService) |
|--------|-------------------------|---------------------------|
| Files | 6 new files (definition, types, lookup, format) | 2 files (service h/cpp) |
| Abstraction | void* with casts | Direct MediaKeyReport* |
| Execution | Handler calls lookup functions | Handler calls service.executeMediaKey() |
| Coupling | Transitive BleKeyboard.h in headers via types.h | BleKeyboard.h only in service.cpp |
| LOC | ~400 lines across 6 files | ~200 lines in 2 files |
| Adding action | Update table + recompile | Update table + recompile |
| Service intent | Unclear (mapping? utilities?) | Clear: BLE keyboard operations |

## Code Review Notes

**Design Decision Rationale:**

Initial approach attempted to decouple BleKeyboard.h from ConfigManager using void* and 6 abstraction files. However:
- ConfigManager doesn't actually need BleKeyboard - it only validates action IDs
- ButtonEventHandler already includes BleKeyboard.h (legitimate dependency for execution)
- The void* abstraction added complexity without real benefit

**Final approach:**
- BleKeyboardService encapsulates both mapping AND execution
- ConfigManager gets service for validation (no BleKeyboard.h needed)
- ButtonEventHandler gets service for execution (cleaner than direct BleKeyboard dependency)
- Single file contains all action definitions (true single source of truth)

This follows **service layer pattern** rather than **anemic data model** pattern, resulting in simpler, more maintainable code.

## Future Enhancements

- Add browser media keys (Home, Back, Search, Bookmarks)
- Add calculator/email launcher keys
- Group actions by category in menu (Media, Browser, System)
- Macro system can use similar service pattern

## Review Follow-ups (AI)

- [x] [AI-Review][High] **Synchronization Risk**: `BUTTON_ACTION_COUNT` in `BleKeyboardService.h` must manually match `MEDIA_KEY_ACTIONS` size in `.cpp`. Failure to update both results in menu actions being hidden. ✅ **RESOLVED**: Added compile-time validation via static_assert.
- [x] [AI-Review][Medium] **Missing Safety**: Add `static_assert(ACTION_COUNT == BUTTON_ACTION_COUNT, "Count mismatch");` in `BleKeyboardService.cpp` to enforce compilation failure if they diverge. ✅ **IMPLEMENTED**: `BleKeyboardService.cpp:30-32` - compile-time validation enforces synchronization.

## Additional Improvements

- [x] **Defensive Logging**: Added bounds-check logging in `getActionIdByIndex()` to catch runtime misuse (BleKeyboardService.cpp:109)

## References

- BleKeyboard media keys: `.pio/libdeps/use_nimble/BleKeyboard/NimBleKeyboard.h`
- Service implementation: `src/BLE/BleKeyboardService.cpp:17-25`
- Dependency injection: `src/main.cpp:44-53, 145-187`

---

**Created:** 2026-01-24
**Completed:** 2026-01-25
**Type:** Refactoring / Technical Debt
**Effort:** ~3 hours (design iteration + implementation)