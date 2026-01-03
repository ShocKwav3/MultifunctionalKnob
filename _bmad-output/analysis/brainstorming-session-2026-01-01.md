---
stepsCompleted: [1, 2, 3]
inputDocuments: ['requirement-next.txt']
session_topic: 'ESP32-C3 Bluetooth HID Peripheral - Next Feature Iteration'
session_goals: 'Systematically capture implementation details for prioritized features'
selected_approach: 'Direct Structured Capture (Priority-Ordered)'
techniques_used: ['Direct Structured Capture']
ideas_generated: ['NVS namespace fix', 'button naming consolidation', 'poll() investigation', 'wheel behavior menu restructure', 'bluetooth menu', 'display dual-mode UI', 'deep sleep with warning', 'LED strip control menu']
context_file: ''
priority_order: true
---

# Brainstorming Session Results

**Facilitator:** Feroj
**Date:** 2026-01-01

## Session Overview

**Topic:** ESP32-C3 Bluetooth HID Peripheral - Next Feature Iteration

**Goals:** Systematically capture implementation approach for each feature in priority order

**Input Document:** `requirement-next.txt`

### Approach

**Technique:** Direct Structured Capture (Priority-Ordered)
**Rationale:** Clear feature list already exists - focus on implementation details, not ideation

---

## Feature Priority List

1. Configurable device name via build flag
2. Rename buttons (Top Left/Right, Bottom Left/Right)
3. Revisit `buttonManager->poll()` necessity
4. Reverse wheel direction from menu
5. Bluetooth control from menu (connect, pair, disconnect)
6. Physical display support (128x32 I2C OLED)
7. Deep sleep after 5min inactivity
8. LED control from menu

---

## Feature Breakdown

### Feature 1: Configurable Device Name via Build Flag

**Priority:** 1 (Highest)
**Complexity:** Low

**Current State:**
- Device name defined in `include/config/device_config.h` (this is fine)
- `NVS_NAMESPACE` in `src/Config/ConfigManager.h` has hardcoded value "knobkoky"

**Implementation:**
- `NVS_NAMESPACE` should reference device name constant from `device_config.h`
- Simple include + constant reference change

**Files Affected:**
- `src/Config/ConfigManager.h`

---

### Feature 2: Rename Buttons (Top Left/Right, Bottom Left/Right)

**Priority:** 2
**Complexity:** Low-Medium

**Current State:**
- Button names duplicated in `include/config/button_config.h` AND `src/Menu/Model/MenuTree.h`
- DRY violation to fix

**Implementation:**
- Single source of truth: button names in `button_config.h` only
- `MenuTree.h` references `button_config.h`, no duplication
- New naming convention: Top Left, Top Right, Bottom Left, Bottom Right

**Files Affected:**
- `include/config/button_config.h` (define names)
- `src/Menu/Model/MenuTree.h` (reference, not duplicate)
- `src/Menu/Action/SetButtonBehaviorAction.cpp` (update comments)
- `src/Menu/Action/SetButtonBehaviorAction.h` (update comments)
- `src/Menu/Controller/MenuController.cpp` (update comments)

---

### Feature 3: Revisit `buttonManager->poll()` Necessity

**Priority:** 3
**Complexity:** Unknown (Investigation Required)

**Current State:**
- Wheel uses FreeRTOS event/queue pattern with ISR
- Buttons use polling in main loop

**Decision Criteria:**
- If simple to implement event pattern for buttons → do it
- If complex → keep `poll()` as-is

**Goal:** Simplicity over forced consistency. Investigate first, decide after.

**Files Affected:**
- `main.cpp` (loop function)
- Potentially button-related event/handler files

---

### Feature 4: Reverse Wheel Direction from Menu

**Priority:** 4
**Complexity:** Medium

**Implementation:**
- Restructure "Wheel Behavior" menu item to be a parent with 2 sub-items:
  - "Wheel Mode" → scroll, volume, zoom (existing logic, moved here)
  - "Wheel Direction" → normal, reversed (new)
- Persist direction setting to NVS
- Apply direction in encoder handler

**Menu Structure Change:**
```
Before:
  Wheel Behavior → [scroll, volume, zoom]

After:
  Wheel Behavior
    ├── Wheel Mode → [scroll, volume, zoom]
    └── Wheel Direction → [normal, reversed]
```

**Files Affected:**
- `src/Menu/Model/MenuTree.h`
- `src/Config/ConfigManager.h` (new NVS key)
- Encoder handler files (apply direction)

---

### Feature 5: Bluetooth Control from Menu

**Priority:** 5
**Complexity:** Medium

**Implementation:**
- New top-level menu item: "Bluetooth"
- Sub-items:
  - "Pair" → disconnect if connected, then advertise for pairing
  - "Disconnect" → disconnect current connection

**Serial Display Feedback:**
- Pairing mode → show "pairing"
- Connected → show "connected", then return to Bluetooth menu
- Disconnected → show "disconnected"

**Note:** Physical display will use icons instead (covered in Feature 6)

**Menu Structure:**
```
Bluetooth
  ├── Pair
  └── Disconnect
```

**Files Affected:**
- `src/Menu/Model/MenuTree.h`
- New menu action files for BT control
- BLE-related files for pairing/disconnect logic

---

### Feature 6: Physical Display Support (128x32 I2C OLED)

**Priority:** 6
**Complexity:** High

**Hardware:** 128x32 SSD1306 (library already configured for 128x64 SSD1306)

**Display Modes:**
1. **Normal mode:** Main screen shows status icons only
   - Bluetooth: solid (connected), flashing (pairing), none (disconnected/idle)
   - Wheel direction indicator
   - Battery % (placeholder for now)
   - Wheel mode (Z/V/S)

2. **Menu mode:** Small status icons on top, menu content below

**Power Features:**
- Turn off display from menu
- Wake display on long-press wheel button (which also opens menu)
- Investigate best power-saving approach (blank screen vs actual power off)

**Files Affected:**
- Display driver/wrapper files
- Menu rendering logic
- New icon assets/definitions

---

### Feature 7: Deep Sleep After 5min Inactivity

**Priority:** 7
**Complexity:** Medium

**Implementation:**
- Idle timer, reset on any user input
- Trigger ESP32 deep sleep after 5 minutes of inactivity
- Wake source: Wheel button ONLY
- BT connection does NOT prevent sleep

**User Warning:**
- Display notification at 4 minutes idle (1 minute before sleep)

**Files Affected:**
- `main.cpp` or new power management module
- Display files (for warning notification)
- Button/encoder handlers (reset idle timer)

---

### Feature 8: LED Strip Control from Menu

**Priority:** 8
**Complexity:** Medium-High

**Hardware:** LED strip (external, not onboard LED)
**Library:** FastLED

**Menu Structure:**
```
LED
  ├── LED Power → [On, Off]
  ├── Brightness → Wheel-controlled percentage
  ├── Modes → [list TBD at implementation]
  └── Color → [list TBD at implementation]
```

**Brightness Control:**
- Screen shows percentage
- Wheel rotation changes value
- Real-time preview on strip

**Persistence:** All LED settings saved to NVS

**Files Affected:**
- `platformio.ini` (add FastLED dependency)
- `src/Menu/Model/MenuTree.h`
- New LED controller/driver files
- `src/Config/ConfigManager.h` (new NVS keys)

---

## Summary

| # | Feature | Complexity | Key Change |
|---|---------|------------|------------|
| 1 | Device Name Config | Low | NVS namespace from device_config.h |
| 2 | Button Renaming | Low-Med | Single source of truth, update refs |
| 3 | Button Poll Review | Unknown | Investigate, simplicity wins |
| 4 | Wheel Direction | Medium | Menu restructure, new NVS setting |
| 5 | Bluetooth Menu | Medium | New menu + BT actions |
| 6 | Physical Display | High | Icons, two modes, power control |
| 7 | Deep Sleep | Medium | Idle timer, wake config, warning |
| 8 | LED Strip | Med-High | FastLED, full menu control |

## Dependencies & Suggested Order

1. **Feature 1** → No dependencies, quick win
2. **Feature 2** → No dependencies, cleanup
3. **Feature 3** → Investigation only, no blockers
4. **Feature 4** → Menu restructure, good foundation practice
5. **Feature 5** → Depends on understanding menu patterns from Feature 4
6. **Feature 6** → Independent but complex, display abstraction needed
7. **Feature 7** → Depends on Feature 6 (display warning)
8. **Feature 8** → Independent but leverages menu patterns

## Next Steps

This brainstorming output can feed directly into:
- **Tech Spec** for implementation planning
- **Epic/Story creation** for sprint planning
- **Architecture review** for display abstraction design

