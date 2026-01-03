---
stepsCompleted: [1, 2, 3, 4, 5]
inputDocuments: ['brainstorming-session-2026-01-01.md', 'project-context.md', 'index.md']
date: 2026-01-01
author: Feroj
---

# Product Brief: UtilityButtonsWithKnobUSB

## Executive Summary

UtilityButtonsWithKnobUSB is a multi-role versatile Bluetooth HID peripheral built on the ESP32-C3 platform. This iteration focuses on transforming the device from a functional prototype into a truly usable, self-contained peripheral that can be configured entirely on-device without reflashing firmware.

The core value proposition: **set once, forget forever**. Users configure wheel behavior, button mappings, Bluetooth pairing, display preferences, and more — all through the on-device menu system, with settings persisted to NVS storage.

---

## Core Vision

### Problem Statement

The device currently works as a Bluetooth HID peripheral (scroll, volume, zoom modes), but lacks easy configurability. Changing settings requires code modifications and firmware reflashing, which breaks the seamless "peripheral" experience users expect.

### Problem Impact

- **Friction in daily use:** Users can't quickly adapt the device to different workflows or preferences
- **Barrier to adoption:** Anyone beyond the original developer would struggle to customize the device
- **Incomplete product feel:** Despite solid core functionality, the device feels like a prototype rather than a finished peripheral

### Why Existing Solutions Fall Short

Current state requires developer-level intervention for basic configuration changes. The menu system exists but doesn't yet expose all the settings users need. There's no on-device way to reverse wheel direction, manage Bluetooth connections, control the display, or configure LED strips.

### Proposed Solution

Implement 8 prioritized features that collectively deliver full on-device configurability:

1. Configurable device name via build flag
2. Intuitive button naming (Top Left/Right, Bottom Left/Right)
3. Streamlined button input handling (investigate poll() necessity)
4. Wheel direction reversal from menu
5. Bluetooth control from menu (pair, disconnect)
6. Physical display support with status icons and menu UI
7. Deep sleep with inactivity warning
8. LED strip control from menu

All configuration changes persist to NVS, enabling true "set once, forget forever" operation.

### Key Differentiators

- **Self-contained configuration:** No companion app, no computer, no reflashing — configure everything on-device
- **NVS persistence:** Settings survive power cycles and reconnections
- **Multi-function versatility:** Scroll, volume, zoom, configurable buttons — one device, many roles
- **Focused simplicity:** Purpose-built as a configurable knob peripheral, not an over-engineered Swiss Army knife

---

## Target Users

### Primary Users

**The Maker/Hobbyist**

A hands-on tinkerer who builds tools for personal use and satisfaction. They appreciate well-designed hardware that "just works" without constant fiddling.

**Profile:**
- Builds or customizes their own peripherals
- Values configurability but prefers "set once, forget forever"
- Primarily uses scroll and volume modes for everyday tasks
- Connects the device when needed, uses it casually
- Technical enough to flash firmware, but doesn't want to do it repeatedly

**What success looks like:**
- Device pairs easily, settings persist, no surprises
- Menu system is intuitive enough that configuration is self-explanatory
- Can hand it to a friend without writing a user manual

### Secondary Users

**The Curious Friend**

Someone who receives the device as a gift or sees it in action and wants one. Not a maker themselves, but comfortable with technology.

**Expectations:**
- Should be able to figure out the menu system without guidance
- Pairs like any other Bluetooth device
- "Just works" out of the box with sensible defaults

### User Journey

1. **Build/Flash:** One-time setup by the maker
2. **Configure:** Use on-device menu to set preferences (wheel mode, direction, buttons, etc.)
3. **Pair:** Connect to host device via Bluetooth
4. **Use:** Scroll, volume, zoom — whatever the task needs
5. **Forget:** Settings persist; device is ready whenever needed
6. **Reconfigure (rare):** Open menu, tweak, done

---

## Success Metrics

### Feature Completeness

| # | Feature | Required |
|---|---------|----------|
| 1 | Configurable device name via build flag | Yes |
| 2 | Button renaming (Top Left/Right, Bottom Left/Right) | Yes |
| 3 | Button input handling review (poll() investigation) | Yes |
| 4 | Wheel direction reversal from menu | Yes |
| 5 | Bluetooth control from menu (pair, disconnect) | Yes |
| 6 | Physical display support (128x32 OLED with status icons) | Yes |
| 7 | Deep sleep after 5min inactivity with warning | Yes |
| 8 | LED strip control from menu | Yes (lowest priority) |

**Success:** All 8 features implemented, accessible via menu, settings persisted to NVS.

### Reliability Criteria

- **No regressions:** Existing scroll/volume/zoom modes continue to work
- **NVS persistence:** All configurable settings survive power cycles
- **Bluetooth stability:** Pairing and connection work consistently
- **Menu UX:** Intuitive navigation, no dead ends or confusing states

### Code Quality Standards

- **Follow `.claude/skills/coding-standard`:** SOLID principles, Clean Code patterns, appropriate design patterns for embedded context
- **Follow `.claude/skills/pio-wrapper`:** Use wrapper for build commands to reduce log noise
- **Pattern consistency:** New code follows existing codebase conventions (event-driven architecture, static allocation, etc.)
- **Build constraints:** Firmware fits in flash, no unnecessary bloat

### "Ready to Share" Test

- All 8 features work as expected during personal use
- Device can be handed to a friend without written instructions
- Menu system is self-explanatory

### Business Objectives

N/A — This is a personal/hobbyist project, not a commercial product.

### Key Performance Indicators

N/A — No formal KPIs. Success is defined by functional completeness and personal satisfaction.

---

## MVP Scope

### Core Features (Priority Order)

All 8 features are in scope for this iteration. Developers should implement in the order listed:

| Priority | Feature | Description |
|----------|---------|-------------|
| 1 | Device Name Config | NVS namespace references device name from `device_config.h` |
| 2 | Button Renaming | Single source of truth for button names (Top Left/Right, Bottom Left/Right) |
| 3 | Button Poll Investigation | Evaluate whether button polling is necessary vs. event-driven approach like wheel. Outcome may go either way — goal is design validation, not forced change. |
| 4 | Wheel Direction | Menu option to reverse wheel direction, persisted to NVS |
| 5 | Bluetooth Menu | Pair and Disconnect options accessible from menu |
| 6 | Physical Display | 128x32 OLED support with status icons (normal mode) and menu UI (menu mode) |
| 7 | Deep Sleep | 5-minute inactivity triggers sleep; 1-minute warning on display; wake via wheel button |
| 8 | LED Strip Control | Full menu control: power, brightness, modes, color — all persisted to NVS |

### Out of Scope

**Permanently out of scope (will never be implemented):**
- OTA firmware updates
- Companion app

**Deferred to future iterations:**
- New wheel modes (easy to add later due to existing architecture)
- New button actions (easy to add later due to existing architecture)
- Any features not listed above

### Implementation Notes

- **Independence:** Features are independent and can be developed without blocking each other
- **Order matters:** Implement in priority order (1→8) as specified
- **Investigation outcome:** Feature 3 is exploratory — the result may be "keep poll()" or "switch to event-driven," either is acceptable

### MVP Success Criteria

- All 8 features implemented and working
- Settings persist via NVS across power cycles
- No regressions in existing functionality
- Menu system remains intuitive and self-explanatory
- Code follows existing patterns and `.claude/skills` standards

### Future Vision

**Next iteration (TBD):**
- To be decided after this iteration is complete
- New wheel modes and button actions are easy additions when needed

**Architecture investment:**
- Current event-driven design makes future extensions straightforward
- No major refactoring expected for common enhancement patterns
