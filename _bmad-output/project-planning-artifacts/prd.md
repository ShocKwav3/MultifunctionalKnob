---
stepsCompleted: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
inputDocuments: ['product-brief-UtilityButtonsWithKnobUSB-2026-01-01.md', 'brainstorming-session-2026-01-01.md', 'project-context.md']
documentCounts:
  briefs: 1
  research: 0
  brainstorming: 1
  projectDocs: 1
workflowType: 'prd'
lastStep: 3
projectType: 'brownfield-enhancement'
---

# Product Requirements Document - UtilityButtonsWithKnobUSB

**Author:** Feroj
**Date:** 2026-01-01

## Executive Summary

UtilityButtonsWithKnobUSB is an ESP32-C3 Bluetooth HID peripheral that functions as a configurable knob for scroll, volume, and zoom control. This PRD defines **8 enhancement features** that transform the device from a functional prototype into a fully self-configurable peripheral.

**Core Vision:** "Set once, forget forever" — all device settings configurable on-device through the menu system, persisted to NVS storage, requiring no firmware reflashing or external tools.

**Target Users:**
- **Primary:** Makers/hobbyists who build tools for personal use and value devices that "just work"
- **Secondary:** Non-technical recipients who expect intuitive, self-explanatory operation

### What Makes This Special

- **Complete on-device autonomy** — every setting accessible via the physical menu system
- **Persistent configuration** — NVS storage ensures settings survive power cycles indefinitely
- **Handoff-ready simplicity** — designed to be given to a friend without written instructions
- **Focused scope** — purpose-built knob peripheral, deliberately avoiding feature creep

## Project Classification

**Technical Type:** IoT/Embedded
**Domain:** General (hobbyist/maker)
**Complexity:** Low
**Project Context:** Brownfield — extending existing functional system with 8 prioritized features

**Existing Architecture:** Event-driven system using FreeRTOS queues, static allocation, NimBLE stack on ESP32-C3 (RISC-V 160MHz, 400KB SRAM). New features must align with established patterns.

## Success Criteria

### User Success

- Device is intuitive to use without documentation
- Configuration changes are straightforward via the menu system
- Settings "stick" — no surprises after power cycling

### Technical Success

- All 8 features implemented and functional
- No regressions in existing scroll/volume/zoom modes
- NVS persistence works reliably across power cycles
- Code follows existing architecture patterns (event-driven, static allocation)
- Firmware builds and runs on ESP32-C3 Super Mini

### Business Success

N/A — personal hobby project. Success = it works and you're satisfied.

## Product Scope

### MVP (This Iteration)

All 8 features in priority order:
1. Configurable device name via build flag
2. Intuitive button naming (Top Left/Right, Bottom Left/Right)
3. Button input handling review (poll() investigation)
4. Wheel direction reversal from menu
5. Bluetooth control from menu (pair, disconnect)
6. Physical display support (128x32 OLED)
7. Deep sleep with inactivity warning
8. LED strip control from menu

### Out of Scope (Permanently)

- OTA firmware updates
- Companion app

### Future (If/When Wanted)

- Additional wheel modes
- Additional button actions
- Whatever sounds fun later

## User Journeys

### Journey 1: Feroj the Maker — "Finally, a Knob That Stays Configured"

Feroj has been using a rotary encoder hooked to an ESP32-C3 for months now. It works — scroll mode for browsing documentation, volume mode for music during coding sessions, zoom for CAD work. But every time he wants to tweak something — reverse the wheel direction, change the default mode — he's back in VS Code, editing config files, reflashing firmware. It's a 10-minute interruption that breaks his flow.

One Saturday afternoon, he finishes implementing the last of the 8 features. He plugs in the device, long-presses the wheel button, and the OLED lights up with a clean menu. He navigates to Wheel Behavior → Wheel Direction → Reversed. Done. No laptop, no cables, no reflash.

He unplugs the device, plugs it back in a week later. The direction is still reversed. He grins. "Set once, forget forever" is finally real.

### Journey 2: Sam the Curious Friend — "Wait, I Can Just... Use It?"

Sam visits Feroj's workspace and notices a small knob device next to his keyboard. "What's that?" Feroj hands it over: "Bluetooth scroll wheel. Pairs like AirPods."

Sam turns it on. A small OLED shows a Bluetooth icon blinking. He pulls out his laptop, opens Bluetooth settings, sees "UtilityKnob" in the list, clicks Pair. The icon goes solid. He scrolls a webpage with the knob. It works.

"What if I want it to control volume instead?" Sam asks. Feroj shrugs: "Long-press the button." Sam does. A menu appears. He scrolls to Wheel Mode, selects Volume. He spins the knob — his laptop volume changes. No questions needed.

### Journey 3: Daily Driver Mode — "Monday Morning, No Surprises"

Monday morning. Feroj taps the wheel button to wake the device and starts scrolling through Slack. The OLED shows "S" for scroll mode and a solid Bluetooth icon — connected, ready.

Later, he forgets the device for 4 minutes. A warning appears: "Sleep in 1 min." He taps the button — dismissed. At day's end, it sleeps. Tomorrow, a button press wakes it, Bluetooth reconnects, and he's scrolling within seconds.

### Journey Requirements Summary

| Capability Area | Revealed By |
|-----------------|-------------|
| Menu-based configuration | Journey 1, 2 |
| NVS persistence | Journey 1, 3 |
| OLED status display | All |
| Standard BT pairing | Journey 2 |
| Sensible defaults | Journey 2 |
| Deep sleep with warning | Journey 3 |
| Quick wake + auto-reconnect | Journey 3 |

## Functional Requirements

### Device Identity & Configuration

- FR1: User can set device Bluetooth name via build-time configuration
- FR2: System references device name from single source of truth for NVS namespace

### Button System

- FR3: System handles button input efficiently with clear position-based identification (Top Left, Top Right, Bottom Left, Bottom Right) — includes evaluating poll() vs event-driven approach
- FR4: System uses single source of truth for button naming across codebase
- FR5: User can configure button behavior via menu

### Wheel Control

- FR6: User can switch wheel mode (scroll, volume, zoom) via menu
- FR7: User can reverse wheel direction via menu
- FR8: System persists wheel mode and direction to NVS
- FR9: System applies wheel direction setting to encoder input

### Macro Control

- FR10: User can activate/deactivate macro mode by long-pressing the macro button (toggle)
- FR11: When macro mode is active, inputs execute assigned macros; when deactivated, inputs return to normal behavior
- FR12: Macro mode state persists across menu navigation

### Bluetooth Connectivity

- FR13: User can initiate Bluetooth pairing from menu
- FR14: User can disconnect current Bluetooth connection from menu
- FR15: System auto-reconnects to paired device after wake
- FR16: Display shows Bluetooth connection status (connected, pairing, disconnected)

### Display System

- FR17: System displays status information in normal mode (BT status, wheel mode, direction)
- FR18: System displays menu interface when menu is active
- FR19: User can wake display via wheel button press
- FR20: System supports 128x32 OLED hardware

### Power Management

- FR21: System enters deep sleep after 5 minutes of inactivity
- FR22: System displays sleep warning 1 minute before sleep
- FR23: User can wake device via wheel button
- FR24: User input resets inactivity timer

### LED Strip Control

- FR25: User can toggle LED strip power via menu
- FR26: User can adjust LED brightness via menu (wheel-controlled)
- FR27: User can select LED mode via menu
- FR28: User can select LED color via menu
- FR29: System persists all LED settings to NVS

### Menu Navigation

- FR30: User can access menu via long-press on wheel button
- FR31: User can navigate menu items using wheel rotation
- FR32: User can select menu items using wheel button press
- FR33: User can return to parent menu or exit menu

## Non-Functional Requirements

### Performance

- NFR1: Encoder input response < 50ms (no perceptible lag)
- NFR2: Menu navigation feels instant (< 100ms feedback)
- NFR3: BLE HID commands sent within 20ms of input

### Reliability

- NFR4: NVS settings persist across 10,000+ power cycles
- NFR5: Device operates continuously for 8+ hours without requiring restart
- NFR6: BLE reconnection completes within 5 seconds of wake

### Resource Constraints

- NFR7: Firmware fits within ESP32-C3 flash (4MB)
- NFR8: RAM usage stays within safe limits (< 300KB of 400KB)
- NFR9: No heap fragmentation from dynamic allocation (use static allocation)

### Usability

- NFR10: Menu system navigable without documentation
- NFR11: All settings accessible within 3 menu levels

