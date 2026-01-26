---
stepsCompleted: [1, 2, 3, 4, 5, 6, 7, 8]
status: 'complete'
completedAt: '2026-01-22'
inputDocuments:
  - '_bmad-output/analysis/macro-feature-requirement-summary.md'
  - '_bmad-output/architecture/core-architectural-decisions.md'
  - '_bmad-output/architecture/implementation-patterns-consistency-rules.md'
  - '_bmad-output/architecture/project-structure-boundaries.md'
  - '_bmad-output/project-context.md'
workflowType: 'architecture-addendum'
project_name: 'UtilityButtonsWithKnobUSB'
feature_name: 'Macro Activation System'
user_name: 'Feroj'
date: '2026-01-22'
---

# Macro Activation System - Architecture Addendum

_This document extends the existing architecture with decisions specific to the macro activation feature. It references established patterns from the core architecture documents._

**Parent Architecture:** [Core Architectural Decisions](./core-architectural-decisions.md)

---

## Project Context Analysis

### Requirements Overview

**Functional Requirements:**
- FR-1: New macro button enables toggle-based macro mode activation
- FR-2: 7 configurable macro slots (wheel button, wheel left/right, buttons 1-4)
- FR-3: Macros are key combinations (modifier keys + keycode)
- FR-4: Macros persist to NVS across power cycles
- FR-5: Menu-based configuration for each macro slot
- FR-6: Long-press and release macro button activates macro mode; inputs execute assigned macros. Long-press again deactivates macro mode; inputs return to normal behavior

**Non-Functional Requirements:**
- NFR-1: No additional latency on normal input path when macro not active
- NFR-2: Memory footprint within ESP32-C3 constraints (static allocation)
- NFR-3: Consistent with existing event-driven architecture patterns

**Scale & Complexity:**
- Primary domain: Embedded input handling / BLE HID
- Complexity level: Low
- Estimated new components: 3-4 (MacroManager, MacroConfigAction, NVS extension, menu items)

### Technical Constraints & Dependencies

| Constraint | Impact |
|------------|--------|
| ESP32-C3 GPIO availability | Must select from pins 10, 20, or 21 |
| Single-core RISC-V | Macro check must be non-blocking |
| NVS write speed (~20ms) | Configure in menu only, not runtime |
| BLE HID key combo format | Must map to HID usage codes |

**Dependencies on Existing Components:**
- ConfigManager (NVS persistence)
- MenuController + MenuTree (configuration UI)
- ButtonManager pattern (macro button input)
- BleKeyboard (sending key combinations)
- Existing event dispatchers

### Cross-Cutting Concerns Identified

1. **Event Routing Priority** - Macro mode must intercept BEFORE mode handlers but AFTER menu (menu takes precedence)
2. **State Visibility** - Display should indicate macro mode active (optional enhancement)
3. **Factory Reset** - Macro configurations should clear on factory reset

## Starter Template Evaluation

### Primary Technology Domain

Embedded systems / BLE HID device - extending established ESP32-C3 project

### Engineering Standards (Non-Negotiable)

From project context (85 rules established):
- Header guards: `#pragma once`
- Error handling: Return `Error` enum from fallible operations
- Logging: Use `LOG_*` macros, never direct `Serial.print`
- Memory: Static allocation only, no runtime `new`/`malloc`
- Events: Always use queue pattern (never bypass)

### Existing Foundation (No Changes Required)

| Component | Status | Notes |
|-----------|--------|-------|
| PlatformIO config | ✅ Established | `use_nimble` environment |
| Event dispatchers | ✅ Established | Encoder, Button, App, Menu |
| Menu system | ✅ Established | MenuController, MenuTree, MenuAction pattern |
| ConfigManager | ✅ Established | NVS persistence in "knobkoky" namespace |
| BleKeyboard | ✅ Established | HID key sending capability |
| ButtonManager | ✅ Established | GPIO button input pattern |

### New Components Required

| Component | Type | Location |
|-----------|------|----------|
| MacroManager | Manager class | `src/Macro/Manager/MacroManager.cpp/.h` |
| MacroDefinition | Data struct | `include/Type/MacroDefinition.h` |
| MacroInputEnum | Enum | `include/Enum/MacroInputEnum.h` |
| SetMacroAction | Menu action | `src/Menu/Action/SetMacroAction.cpp/.h` |
| macro_config.h | Config | `include/Config/macro_config.h` |

### Testability Design

Per project context, automated testing is **deferred** for MVP. Testability via:
- `LOG_DEBUG` tracing through event flow
- Manual testing with serial monitor at 460800 baud
- Direct method testing when unit tests enabled later

### Architectural Decisions Established

**By existing project:**
- Event-driven architecture with FreeRTOS queues
- Menu intercepts events before mode handlers
- NVS with validated enum storage
- Command pattern for menu actions
- Display arbitration via DisplayTask

**Note:** No project initialization required - extending existing codebase.

## Core Architectural Decisions

### Decision Priority Analysis

**Critical Decisions (Block Implementation):**
- Macro button GPIO pin selection
- NVS storage schema for macro definitions
- Event interception point for macro mode

**Important Decisions (Shape Architecture):**
- MacroInput enum structure
- MacroManager responsibility boundaries
- Menu configuration flow

**Deferred Decisions (Post-MVP):**
- Display indicator for macro mode active
- Per-profile macro sets

### Hardware Architecture

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Macro Button GPIO | GPIO 10 | No special function, clean pin with no conflicts |
| Button Configuration | Active-low with internal pull-up | Matches existing button pattern |
| Debounce | 50ms (existing DEBOUNCE_MS) | Consistent with other buttons |

**GPIO Assignment Update:**
```cpp
// include/Config/macro_config.h
constexpr uint8_t MACRO_BUTTON_PIN = 10;
constexpr bool MACRO_BUTTON_ACTIVE_LOW = true;
```

### Data Architecture

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Macro Storage | Packed uint16_t per macro | Compact, single NVS operation |
| NVS Keys | "macro.0" through "macro.6" | Direct enum-to-key mapping |
| Default Value | 0x0000 (no macro) | Zero = disabled, no action |
| Validation | Check modifiers ≤ 0xFF, keycode ≤ 0xFF | Graceful fallback on corruption |

**Macro Definition Structure:**
```cpp
// include/Type/MacroDefinition.h
struct MacroDefinition {
    uint8_t modifiers;  // HID modifier bitmask
    uint8_t keycode;    // HID usage code

    bool isEmpty() const { return modifiers == 0 && keycode == 0; }
    uint16_t toPacked() const { return (modifiers << 8) | keycode; }
    static MacroDefinition fromPacked(uint16_t packed) {
        return { static_cast<uint8_t>(packed >> 8), static_cast<uint8_t>(packed & 0xFF) };
    }
};
```

**NVS Key Structure:**
```
Namespace: "knobkoky" (existing)
Keys: "macro.0", "macro.1", "macro.2", "macro.3", "macro.4", "macro.5", "macro.6"
Type: uint16_t
```

**Input Enum:**
```cpp
// include/Enum/MacroInputEnum.h
enum class MacroInput : uint8_t {
    WHEEL_BUTTON = 0,
    WHEEL_LEFT = 1,
    WHEEL_RIGHT = 2,
    BUTTON_1 = 3,
    BUTTON_2 = 4,
    BUTTON_3 = 5,
    BUTTON_4 = 6
};
constexpr uint8_t MACRO_INPUT_COUNT = 7;
```

### Component Architecture (MacroManager)

| Decision | Choice | Rationale |
|----------|--------|-----------|
| State Pattern | Toggle state machine | Macro mode = active/inactive toggle (long-press to transition) |
| Long-Press Detection | In ButtonEventHandler | 500ms hold time threshold |
| Responsibility | State toggle + lookup + execute | Single class manages macro mode state and execution |
| BLE Dependency | Injected BleKeyboard* | Matches existing handler pattern |

**MacroManager Interface:**
```cpp
// src/Macro/Manager/MacroManager.h
class MacroManager {
    BleKeyboard* bleKeyboard;
    bool macroModeActive;  // Toggle state (not button state)
    MacroDefinition macros[MACRO_INPUT_COUNT];

public:
    explicit MacroManager(BleKeyboard* ble);

    void toggleMacroMode();  // Toggle on/off
    bool isMacroModeActive() const { return macroModeActive; }

    bool executeMacro(MacroInput input);  // Returns true if macro executed

    void loadFromNVS(ConfigManager& config);
    Error saveMacro(ConfigManager& config, MacroInput input, MacroDefinition macro);
};
```

### Communication Patterns

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Event Interception | Handler level | Matches menu pattern, clean separation |
| Priority Order | Menu > Macro > Normal Mode | Menu always takes precedence |
| Macro Button Events | Via existing ButtonEventDispatcher | No new dispatcher needed |

**Event Interception Pattern:**
```cpp
// In EncoderEventHandler (updated)
void handleEncoderEvent(EncoderInputEvent& event) {
    // Priority 1: Menu intercepts
    if (menuController.isActive()) {
        menuController.handleEvent(event);
        return;
    }

    // Priority 2: Macro mode active (toggled by long-press)
    if (macroManager.isMacroModeActive()) {
        MacroInput input = mapEncoderEventToMacroInput(event);
        if (macroManager.executeMacro(input)) {
            return;  // Macro consumed event
        }
    }

    // Priority 3: Normal mode handling
    currentModeHandler->handleEvent(event);
}

// In ButtonEventHandler (updated) - Macro button long-press detection
if (event.pin == MACRO_BUTTON_PIN) {
    if (event.pressed) {
        macroPressStart = millis();
    } else {
        uint32_t duration = millis() - macroPressStart;
        if (duration >= 500) {
            macroManager->toggleMacroMode();  // Toggle on/off
        }
    }
    return;  // Don't process as normal button
}
```

**Macro Button State Flow (Long-Press Toggle):**
```
ButtonEventHandler receives macro button GPIO event
  → Checks hold duration (long-press ≥ 500ms?)
    ├─ YES (long-press released) → macroManager.toggleMacroMode()
    └─ NO (short-press) → Ignored, no action
  → No further action (toggle control only, doesn't trigger macros directly)
```

### Decision Impact Analysis

**Implementation Sequence:**
1. Add macro_config.h with GPIO definition
2. Add MacroInputEnum.h and MacroDefinition.h
3. Implement MacroManager (state + lookup + execute)
4. Extend ConfigManager with macro NVS methods
5. Update EncoderEventHandler with macro interception
6. Update ButtonEventHandler with macro interception + macro button handling
7. Add menu items for macro configuration
8. Implement SetMacroAction for menu

**Cross-Component Dependencies:**
- MacroManager depends on BleKeyboard (injected)
- MacroManager depends on ConfigManager (for persistence)
- EncoderEventHandler depends on MacroManager (for interception)
- ButtonEventHandler depends on MacroManager (for interception + button state)
- SetMacroAction depends on MacroManager + ConfigManager

## Implementation Patterns & Consistency Rules

### Critical Conflict Points Addressed

5 potential conflict areas identified and standardized for AI agent consistency.

### Naming Patterns

| Element | Pattern | Example |
|---------|---------|---------|
| NVS keys | `"macro."` + index | `"macro.0"`, `"macro.6"` |
| Enum values | UPPER_SNAKE_CASE | `WHEEL_BUTTON`, `BUTTON_1` |
| Display labels | Title Case | `"Wheel Button"`, `"Button 1"` |
| Helper functions | `mapXToY()` | `mapEncoderEventToMacroInput()` |
| Class names | PascalCase | `MacroManager`, `MacroDefinition` |
| Config constants | UPPER_SNAKE_CASE | `MACRO_BUTTON_PIN`, `MACRO_INPUT_COUNT` |

### Structure Patterns

**Directory Organization:**
```
src/
└── Macro/
    └── Manager/
        ├── MacroManager.cpp
        └── MacroManager.h

include/
├── Config/
│   └── macro_config.h
├── Enum/
│   └── MacroInputEnum.h
└── Type/
    └── MacroDefinition.h

src/Menu/Action/
└── SetMacroAction.cpp/.h
```

**File Placement Rules:**
- Manager classes → `src/<Domain>/Manager/`
- Data structs → `include/Type/`
- Enums → `include/Enum/`
- Config constants → `include/Config/`
- Menu actions → `src/Menu/Action/`

### Format Patterns

**Modifier Constants:**
Use BleKeyboard's existing constants (do not redefine):
```cpp
// From BleKeyboard library
KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI
KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, KEY_RIGHT_ALT, KEY_RIGHT_GUI
```

**Key Combo Display Format:**
```cpp
// Display format: "Modifier+Key" with Title Case
"Ctrl+C"      // Single modifier
"Ctrl+Shift+V" // Multiple modifiers
"None"        // Empty macro
```

**Packed Storage Format:**
```cpp
uint16_t packed = (modifiers << 8) | keycode;
// Upper byte: modifier bitmask
// Lower byte: HID keycode
```

### Communication Patterns

**Macro Button Identification:**
```cpp
// In ButtonEventHandler - macro button is handled separately from BUTTONS[]
if (event.pin == MACRO_BUTTON_PIN) {
    macroManager.setMacroButtonState(event.pressed);
    return;  // Don't process as normal button
}
```

**Event Interception Order:**
```cpp
// Priority order (highest to lowest):
// 1. Menu active → MenuController handles
// 2. Macro mode active → MacroManager handles
// 3. Normal mode → ModeHandler handles
```

**Logging Pattern:**
```cpp
LOG_DEBUG("Macro", "Button state: %s", held ? "held" : "released");
LOG_DEBUG("Macro", "Executing macro for input %d", static_cast<int>(input));
LOG_INFO("Macro", "Saved macro for input %d: 0x%04X", index, packed);
```

### Process Patterns

**Empty Macro Handling:**
```cpp
bool MacroManager::executeMacro(MacroInput input) {
    MacroDefinition& macro = macros[static_cast<uint8_t>(input)];
    if (macro.isEmpty()) {
        return false;  // Silent - expected case, no log
    }
    // ... execute
    return true;
}
```

**BLE Disconnected Handling:**
```cpp
bool MacroManager::executeMacro(MacroInput input) {
    if (!bleKeyboard->isConnected()) {
        LOG_DEBUG("Macro", "BLE not connected, skipping macro");
        return false;
    }
    // ... execute
}
```

**Menu Save Pattern:**
```cpp
// Immediate save with confirmation (matches existing actions)
void SetMacroAction::execute() {
    Error err = macroManager.saveMacro(config, targetInput, newMacro);
    if (err == Error::OK) {
        // Show confirmation via existing menu feedback pattern
    }
}
```

### Enforcement Guidelines

**All AI Agents MUST:**
1. Use `"macro."` prefix for all macro-related NVS keys
2. Place MacroManager in `src/Macro/Manager/` directory
3. Use existing BleKeyboard modifier constants, never redefine
4. Check `bleKeyboard->isConnected()` before any HID operation
5. Follow Menu > Macro > Normal priority order in event handlers
6. Return `Error` enum from NVS operations, not bool

**Pattern Verification:**
- Build must succeed with no warnings
- `LOG_DEBUG` statements trace macro activation flow
- Manual testing confirms priority order (menu overrides macro)

### Pattern Examples

**Good Example - MacroManager Method:**
```cpp
Error MacroManager::saveMacro(ConfigManager& config, MacroInput input, MacroDefinition macro) {
    uint8_t index = static_cast<uint8_t>(input);
    if (index >= MACRO_INPUT_COUNT) {
        return Error::INVALID_PARAM;
    }

    macros[index] = macro;
    return config.saveMacro(index, macro.toPacked());
}
```

**Good Example - Event Interception:**
```cpp
// In EncoderEventHandler
if (menuController.isActive()) {
    menuController.handleEvent(event);
    return;
}
if (macroManager.isMacroModeActive()) {
    MacroInput input = mapEncoderEventToMacroInput(event);
    if (macroManager.executeMacro(input)) {
        return;
    }
}
encoderModeManager.handleEvent(event);
```

**Anti-Patterns to Avoid:**
```cpp
// ❌ Wrong: Tracking button state instead of toggle state
void setMacroButtonState(bool held) { }  // Use toggleMacroMode() instead

// ❌ Wrong: Not detecting long-press duration
if (event.pin == MACRO_BUTTON_PIN && event.pressed) {
    macroManager->toggleMacroMode();  // Should check duration on release
}

// ❌ Wrong NVS key format
config.saveMacro("macros_0", value);  // Should be "macro.0"

// ❌ Redefining modifier constants
#define CTRL_KEY 0x01  // Use BleKeyboard's KEY_LEFT_CTRL

// ❌ Missing BLE check
bleKeyboard->press(keycode);  // Must check isConnected() first

// ❌ Wrong priority order
if (macroManager.isMacroModeActive()) { ... }
if (menuController.isActive()) { ... }  // Menu should be checked FIRST

// ✅ Correct: Long-press detection with toggle
if (event.pin == MACRO_BUTTON_PIN) {
    if (event.pressed) {
        macroPressStart = millis();
    } else {
        uint32_t duration = millis() - macroPressStart;
        if (duration >= 500) {
            macroManager->toggleMacroMode();  // Toggle on/off
        }
    }
    return;
}
```

## Project Structure & Boundaries

### Complete Project Directory Structure

**New Files (Macro Feature):**
```
include/
├── Config/
│   └── macro_config.h                    # GPIO pin, macro constants
├── Enum/
│   └── MacroInputEnum.h                  # MacroInput enum (7 inputs)
└── Type/
    └── MacroDefinition.h                 # Macro struct (modifiers + keycode)

src/
├── Macro/
│   └── Manager/
│       ├── MacroManager.cpp
│       └── MacroManager.h
└── Menu/
    └── Action/
        ├── SetMacroAction.cpp
        └── SetMacroAction.h
```

**Modified Files:**
```
src/Config/ConfigManager.cpp/.h           # Add macro NVS read/write
src/Config/FactoryReset.cpp               # Clear macros on reset
src/Event/Handler/EncoderEventHandler.cpp # Macro interception
src/Event/Handler/ButtonEventHandler.cpp  # Macro button + interception
src/Menu/Model/MenuTree.h                 # Add Macros submenu
src/main.cpp                              # MacroManager initialization
```

### Design Clarifications

**MacroManager Placement:**
MacroManager lives in `src/Macro/Manager/` (not `src/EncoderMode/`) because:
- It's a separate domain from encoder modes
- It intercepts across multiple input types (encoder + buttons)
- It doesn't implement `EncoderModeHandlerInterface`

**SetMacroAction vs Existing Actions:**
SetMacroAction is more complex than SelectWheelModeAction because it requires:
- Multi-step UI (select modifier → select key)
- Building the MacroDefinition before save
- Displaying current macro value

### Architectural Boundaries

**Component Ownership:**

| Component | Owns | Does Not Own |
|-----------|------|--------------|
| MacroManager | Macro state, execution | NVS, GPIO, Menu UI |
| ConfigManager | NVS read/write | Macro logic, validation |
| ButtonEventHandler | Macro button GPIO | Macro definitions |
| SetMacroAction | Menu UI flow | Macro storage |

**Event Flow Boundaries:**
```
┌─────────────────────────────────────────────────────────────────┐
│                      Input Layer                                 │
│  ButtonManager (GPIO 10) ──→ ButtonEventDispatcher               │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                   Handler Layer (Priority Order)                 │
│  ButtonEventHandler:                                            │
│    1. Is macro button? → MacroManager.setMacroButtonState()     │
│    2. Menu active? → MenuController                             │
│    3. Macro active? → MacroManager.executeMacro()               │
│    4. Else → Normal button action                               │
│                                                                 │
│  EncoderEventHandler:                                           │
│    1. Menu active? → MenuController                             │
│    2. Macro active? → MacroManager.executeMacro()               │
│    3. Else → EncoderModeManager                                 │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Output Layer                                │
│  MacroManager ──→ BleKeyboard.press()/release()                 │
└─────────────────────────────────────────────────────────────────┘
```

### Build Order Dependencies

**Compilation Order:**
1. `include/Config/macro_config.h` - no dependencies
2. `include/Enum/MacroInputEnum.h` - no dependencies
3. `include/Type/MacroDefinition.h` - no dependencies
4. `src/Config/ConfigManager.cpp` - depends on MacroDefinition
5. `src/Macro/Manager/MacroManager.cpp` - depends on ConfigManager, BleKeyboard
6. `src/Menu/Action/SetMacroAction.cpp` - depends on MacroManager
7. `src/Event/Handler/*EventHandler.cpp` - depends on MacroManager
8. `src/main.cpp` - wires everything together

### Requirements to Structure Mapping

| FR | Primary File(s) | Secondary File(s) |
|----|-----------------|-------------------|
| FR-1 Macro button | `macro_config.h`, `ButtonEventHandler.cpp` | `main.cpp` |
| FR-2 7 slots | `MacroInputEnum.h`, `MacroManager.h` | - |
| FR-3 Key combos | `MacroDefinition.h`, `MacroManager.cpp` | - |
| FR-4 NVS persist | `ConfigManager.cpp/.h` | `FactoryReset.cpp` |
| FR-5 Menu config | `MenuTree.h`, `SetMacroAction.cpp/.h` | - |
| FR-6 Modifier mode | `EncoderEventHandler.cpp`, `ButtonEventHandler.cpp` | `MacroManager.cpp` |

### File Modification Summary

**High Impact (Core Logic):**
- `EncoderEventHandler.cpp` - Add macro interception check
- `ButtonEventHandler.cpp` - Add macro button handling + interception
- `ConfigManager.cpp/.h` - Add 7 new NVS methods

**Medium Impact (Integration):**
- `MenuTree.h` - Add Macros submenu with 7 items
- `main.cpp` - Initialize MacroManager, inject into handlers
- `FactoryReset.cpp` - Clear macro keys

**Low Impact (Additions Only):**
- All new files in `src/Macro/` and `include/`

## Architecture Validation Results

### Coherence Validation ✅

**Decision Compatibility:**
All macro feature decisions integrate cleanly with existing architecture:
- GPIO 10 is unused and conflict-free
- NVS schema extends existing namespace pattern
- Event interception follows established menu pattern
- BleKeyboard dependency uses existing injection pattern

**Pattern Consistency:**
All patterns align with project-context.md rules:
- Naming conventions match (PascalCase, camelCase, UPPER_SNAKE)
- File placement follows established structure
- Error handling uses Error enum
- Logging uses LOG_* macros

**Structure Alignment:**
Project structure supports all decisions:
- New domain `src/Macro/` follows existing domain pattern
- Integration points clearly defined at handler level
- Boundaries respect single responsibility

### Requirements Coverage Validation ✅

**Functional Requirements:**

| FR | Status | Architectural Support |
|----|--------|----------------------|
| FR-1 | ✅ | GPIO 10 + ButtonEventHandler |
| FR-2 | ✅ | MacroInput enum + MacroManager array |
| FR-3 | ✅ | MacroDefinition struct |
| FR-4 | ✅ | ConfigManager NVS extension |
| FR-5 | ✅ | SetMacroAction + MenuTree |
| FR-6 | ✅ | Handler-level interception |

**Non-Functional Requirements:**

| NFR | Status | How Addressed |
|-----|--------|---------------|
| NFR-1 No latency | ✅ | Single boolean check |
| NFR-2 Memory | ✅ | Static allocation |
| NFR-3 Patterns | ✅ | Matches existing |

### Implementation Readiness Validation ✅

**Decision Completeness:** All critical decisions documented with specifics
**Structure Completeness:** All files and paths defined
**Pattern Completeness:** Examples and anti-patterns provided

### Gap Analysis Results

**Critical Gaps:** None

**Minor Clarifications Added:**
- Macro button activity SHOULD reset inactivity timer (consistent with other inputs)
- HID keycodes available from BleKeyboard library constants

### Architecture Completeness Checklist

**✅ Requirements Analysis**
- [x] Functional requirements mapped to components
- [x] Non-functional requirements addressed
- [x] Technical constraints (GPIO, memory) documented
- [x] Cross-cutting concerns (factory reset, priority) identified

**✅ Architectural Decisions**
- [x] Hardware decisions (GPIO 10)
- [x] Data architecture (packed uint16_t)
- [x] Component architecture (MacroManager)
- [x] Communication patterns (handler interception)

**✅ Implementation Patterns**
- [x] Naming conventions established
- [x] Structure patterns defined
- [x] Format patterns specified
- [x] Process patterns documented

**✅ Project Structure**
- [x] New files listed with paths
- [x] Modified files identified
- [x] Build dependencies ordered
- [x] Requirements mapped to files

### Architecture Readiness Assessment

**Overall Status:** ✅ READY FOR IMPLEMENTATION

**Confidence Level:** High

**Key Strengths:**
- Clean integration with existing event-driven architecture
- Minimal modification to existing code paths
- Clear separation of concerns (MacroManager owns state, handlers own interception)
- Comprehensive patterns prevent AI agent conflicts

**Deferred Enhancements (Post-MVP):**
- Display indicator for macro mode active
- Per-profile macro sets

### Implementation Handoff

**AI Agent Guidelines:**
1. Follow Implementation Sequence in Core Architectural Decisions
2. Use exact file paths specified in Project Structure
3. Apply all Naming Patterns consistently
4. Check Anti-Patterns before completing each file
5. Verify build succeeds after each major component

**First Implementation Step:**
Create `include/Config/macro_config.h` with GPIO definition

---

## Architecture Completion Summary

### Workflow Completion

**Architecture Decision Workflow:** COMPLETED ✅
**Total Steps Completed:** 8
**Date Completed:** 2026-01-22
**Document Location:** `_bmad-output/architecture/macro-feature-architecture.md`

### Final Architecture Deliverables

**Complete Architecture Addendum**
- All macro feature decisions documented with specifics
- Implementation patterns ensuring AI agent consistency
- File structure with new and modified files identified
- Requirements to architecture mapping complete
- Validation confirming coherence and completeness

**Implementation Ready Foundation**
- 4 core architectural decisions (GPIO, NVS schema, interception pattern, component design)
- 5 implementation pattern categories defined
- 6 new files + 6 modified files specified
- 6 functional requirements fully supported

**AI Agent Implementation Guide**
- Technology integration with existing stack verified
- Consistency rules that prevent conflicts
- Project structure with clear boundaries
- Good examples and anti-patterns documented

### Implementation Sequence

**PHASE 0: Story 11.1 Refactoring (Toggle Behavior Support)**
- [MODIFY] `src/Macro/Manager/MacroManager.h` - Rename `macroButtonHeld` → `macroModeActive`
- [MODIFY] `src/Macro/Manager/MacroManager.cpp` - Replace `setMacroButtonState()` → `toggleMacroMode()`
- [REBUILD] Verify compilation with `pio run -e use_nimble`

**PHASE 1: Original Implementation (Completed in Story 11.1)**
1. `include/Config/macro_config.h` - GPIO definition ✅
2. `include/Enum/MacroInputEnum.h` - Input enum ✅
3. `include/Type/MacroDefinition.h` - Data struct ✅
4. `src/Config/ConfigManager.cpp/.h` - NVS methods ✅
5. `src/Macro/Manager/MacroManager.cpp/.h` - Core logic ✅ (with pending Phase 0 modifications)

**PHASE 2: Story 11.2+ Implementation (Ready for Development)**
6. `src/Event/Handler/ButtonEventHandler.cpp` - Long-press detection + toggle
7. `src/Event/Handler/EncoderEventHandler.cpp` - Macro interception
8. `src/Menu/Model/MenuTree.h` - Macros submenu
9. `src/Menu/Action/SetMacroAction.cpp/.h` - Configuration action
10. `src/main.cpp` - Initialization with toggle pattern
11. `src/Config/FactoryReset.cpp` - Clear on reset

### Quality Assurance Checklist

**✅ Architecture Coherence**
- [x] All decisions integrate with existing architecture
- [x] Technology choices are compatible
- [x] Patterns follow established project conventions
- [x] Structure aligns with existing organization

**✅ Requirements Coverage**
- [x] All 6 functional requirements supported
- [x] All 3 non-functional requirements addressed
- [x] Cross-cutting concerns (factory reset, priority) handled
- [x] Integration points clearly defined

**✅ Implementation Readiness**
- [x] Decisions are specific and actionable
- [x] Patterns prevent agent conflicts
- [x] Structure is complete and unambiguous
- [x] Examples provided for clarity

---

**Architecture Status:** READY FOR IMPLEMENTATION ✅

**Next Phase:** Create epic and stories for macro feature implementation

