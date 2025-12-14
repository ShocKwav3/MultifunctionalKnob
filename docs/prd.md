---
stepsCompleted: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
inputDocuments:
  - docs/analysis/brainstorming-session-2025-12-14.md
  - docs/index.md
  - docs/project-overview.md
  - docs/hardware-documentation.md
  - docs/development-guide.md
  - docs/source-tree-analysis.md
documentCounts:
  briefs: 0
  research: 0
  brainstorming: 1
  projectDocs: 5
workflowType: 'prd'
lastStep: 11
project_name: 'UtilityButtonsWithKnobUSB'
user_name: 'Feroj'
date: '2025-12-14'
---

# Product Requirements Document - UtilityButtonsWithKnobUSB

**Author:** Feroj
**Date:** 2025-12-14

## Executive Summary

UtilityButtonsWithKnobUSB is an ESP32-C3 based Bluetooth HID peripheral device that currently functions as a single-purpose rotary encoder (scroll/volume control). This PRD defines the evolution from a limited single-function device into a fully configurable multi-purpose input peripheral.

The core addition is a hierarchical menu system that allows users to configure wheel behavior and button actions at runtime, displayed via serial output initially with hardware display support planned. Four additional GPIO-connected buttons will be added alongside the existing encoder button, each configurable to perform any action supported by the ESP32-BLE-Keyboard library.

**Target Users:** Makers, developers, and enthusiasts who want a customizable desktop peripheral. The project will be open-sourced for community contribution.

### What Makes This Special

This isn't just adding features to a scroll wheel - it's transforming a single-purpose device into a user-configurable multi-function peripheral. The key differentiators:

1. **Runtime Configuration** - Users configure behavior through the device itself, not by reflashing firmware
2. **Display-Agnostic Architecture** - Works via serial output now, seamlessly transitions to hardware display later
3. **Config-Driven Flexibility** - Button count and behaviors driven by configuration, not hardcoded
4. **Open-Source Ready** - Designed to be shared and extended by the community

## Project Classification

**Technical Type:** IoT/Embedded Device
**Domain:** General (Consumer Electronics)
**Complexity:** Low
**Project Context:** Brownfield - extending existing ESP32-C3 BLE HID system

The existing codebase provides a solid foundation with event-driven architecture (FreeRTOS queues), handler pattern for modes, and working BLE HID connectivity. New features will integrate with these established patterns.

## Success Criteria

### User Success

Success is defined by user empowerment through runtime customization:

- **Self-Service Configuration**: Users configure wheel behavior and button actions through the device itself, not by reflashing firmware
- **Intuitive Navigation**: Users navigate the menu system naturally using the two-input model (wheel rotation + button press) without consulting documentation
- **Persistent Personalization**: Custom configurations survive power cycles - users pick up exactly where they left off
- **Predictable Interaction**: Long press always means "back," short press always means "select" - no confusion or mode-dependent behavior

### Business Success

As a personal maker project with open-source sharing potential:

- **Daily Driver**: Device becomes a regularly-used part of the creator's desktop workflow
- **Completeness**: Project reaches a state where it does everything intended without feeling half-finished
- **Shareable Quality**: Codebase and documentation are clean enough to publish and share with the maker community
- **Learning Foundation**: Architecture and patterns learned can be applied to future embedded projects

### Technical Success

The core technical challenge is the menu system architecture:

- **Display-Agnostic Framework**: Menu logic operates identically whether output goes to serial monitor or hardware OLED - swapping displays requires zero changes to menu code
- **Config-Driven Flexibility**: Button count, button behaviors, and wheel modes are all driven by configuration files, not hardcoded values
- **Clean Integration**: New menu system integrates with existing event-driven architecture (FreeRTOS queues, handler pattern) without requiring refactoring of working code
- **Extensible Structure**: Adding new menu items, wheel modes, or button behaviors requires minimal code changes

### Measurable Outcomes

| Outcome | Measurement |
|---------|-------------|
| Runtime configuration works | User changes wheel mode and button behavior without reflashing |
| Display abstraction verified | Same firmware runs with serial output and later OLED with no code changes |
| Config-driven design proven | Adding a new button requires config file change + firmware rebuild |
| Daily usability achieved | Creator uses device for real work, not just demos |

## Product Scope

### MVP - Minimum Viable Product

The MVP delivers a fully functional configurable peripheral:

**Menu System:**
- 4-item main menu: Wheel Behavior, Button Configurator, Device Status, About
- Hierarchical navigation with consistent interaction model
- Serial output as initial display mechanism

**Wheel Configuration:**
- Selectable modes: Scroll, Volume, Zoom
- Mode selection persists across power cycles
- Mode list designed for future expansion

**Button Configuration:**
- Dynamic button list read from configuration
- Predefined behaviors: Mute, Play, Pause, Next, Previous
- Behavior assignment persists across power cycles

**Core Infrastructure:**
- Display-agnostic architecture (abstract display interface)
- Configuration persistence (non-volatile storage)
- Integration with existing BLE HID and event system

### Growth Features (Post-MVP)

Features that complete the vision after core functionality is proven:

- **Hardware Display**: OLED display support using the abstraction built in MVP
- **Device Status Screen**: Bluetooth connection strength, battery level, current configurations
- **Expanded Wheel Modes**: Additional modes beyond Scroll/Volume/Zoom
- **Expanded Button Behaviors**: More predefined options (media controls, shortcuts, modifiers)
- **Visual Polish**: Icons, animations, improved display layouts

### Vision (Future)

Long-term possibilities for the platform:

- **Custom Macros**: User-defined key sequences and multi-action behaviors
- **Settings Backup/Restore**: Export and import configurations
- **OTA Firmware Updates**: Update firmware without USB connection
- **Community Behaviors**: Library of community-contributed button behavior definitions
- **Multiple Profiles**: Switch between different configuration profiles

## User Journeys

### Journey 1: Daily Use - The Invisible Tool

**User:** Feroj (Creator/Primary User)

Feroj sits down at his desk and starts working. The KnobKoKy is there, already paired to his PC, configured exactly how he left it yesterday. He's scrolling through code in VS Code, reviewing documentation, adjusting Spotify volume during a quick break - the wheel just does what he expects without conscious thought.

He doesn't "use" the device. He just works. The only reminder it exists is when he instinctively reaches for it and it's exactly where his hand expects it to be. Hours pass. The device never demands attention, never needs "managing." It's an extension of his workflow, not an interruption to it.

**Requirements Revealed:**
- Configuration persistence across power cycles
- Instant-on operation (no setup on boot)
- Seamless BLE reconnection

---

### Journey 2: Mode Switch Mid-Workflow

**User:** Feroj (Creator/Primary User)

Feroj is deep in a coding session, using Scroll mode to navigate through files. A Slack notification pops up - his team wants to do a quick call. He reaches for the knob, long-presses to enter the menu, rotates to "Wheel Behavior," short-presses, rotates to "Volume," and short-presses to confirm.

A brief notification confirms "Volume Mode Active" - then the menu disappears and he's back to his desktop. He joins the call, adjusts volume naturally with the wheel throughout the conversation. After the call, he long-presses again, switches back to Scroll, gets confirmation, and continues coding.

The whole interaction takes a few seconds. He never lost focus on his actual work.

**Requirements Revealed:**
- Quick menu access via long-press
- Visual/audio confirmation of mode change
- Auto-exit menu after selection (return to normal operation)
- Intuitive navigation (wheel to browse, short-press to select, long-press to back/exit)

---

### Journey 3: First-Time Setup - The New Owner

**User:** Alex (Future Maker)

Alex discovers the UtilityButtonsWithKnobUSB project on GitHub while looking for a DIY volume knob. He's got an ESP32-C3 Super Mini and a rotary encoder from an old project. He clones the repo, opens the config file, and sees clear documentation for adding buttons.

He wires three buttons to GPIO 3, 4, and 5, updates the config file to define these pins, and builds the firmware using PlatformIO. The build succeeds, he flashes it, and the device boots.

The encoder works immediately as a scroll wheel. Alex pairs it with his PC and it just works. He enters the Button Configurator through the menu and assigns behaviors: Mute to button 1, Play to button 2, Next to button 3. Each button works exactly as configured.

Alex now has a personalized 3-button + knob controller. The config file edit and rebuild took 5 minutes, and he has complete control over his hardware setup.

**Requirements Revealed:**
- Clear documentation for hardware configuration
- Config file defines button GPIO pins (compile-time)
- Straightforward build process via PlatformIO
- Works out-of-box with just encoder (minimal hardware)
- Button behavior assignment through menu (runtime)
- Sensible defaults for new installations

---

### Journey 4: Hardware Modification - Adding a Button Later

**User:** Feroj (Creator/Primary User)

Feroj has been using his 4-button setup for months. He decides to add a fifth button for "Play/Pause" because he's tired of reaching for the keyboard during music sessions.

He wires a new button to GPIO 8, opens the config file, adds the new pin definition, and rebuilds the firmware. The rebuild takes under a minute. He flashes the new firmware, and the device boots with all five buttons recognized.

He enters the Button Configurator and assigns "Play" to the new button. His existing button behavior assignments are preserved - only the new button needs configuration. The whole process takes about 5 minutes.

**Requirements Revealed:**
- Config file clearly documents how to add buttons
- Firmware rebuild is fast and straightforward
- Existing behavior assignments persist across firmware updates
- New buttons appear in Button Configurator automatically after rebuild

---

### Journey 5: Troubleshooting - Something's Wrong

**User:** Feroj (Creator/Primary User)

Feroj notices one of his buttons stopped working. He enters the menu, goes to "Device Status" to check current button configurations. He sees the button is still configured for GPIO 4 with "Pause" behavior.

He checks the hardware - loose wire. He fixes it, and the button works again. The Device Status screen helped him confirm the firmware was fine, narrowing the problem to hardware.

Another time, he notices Bluetooth seems laggy. He checks Device Status and sees connection strength is weak. He moves the device closer to his PC's Bluetooth adapter and the issue resolves.

**Requirements Revealed:**
- Device Status shows current configuration state
- Connection strength visibility
- Button configuration visibility for troubleshooting
- Read-only status (no accidental changes while troubleshooting)

---

### Journey Requirements Summary

| Journey | Key Capabilities Required |
|---------|---------------------------|
| Daily Use | Config persistence, instant-on, seamless BLE |
| Mode Switch | Menu system, confirmation feedback, auto-exit |
| First-Time Setup | Clear config docs, easy build process, behavior assignment |
| Hardware Mod | Config file edit + rebuild, behavior persistence across updates |
| Troubleshooting | Device status display, config visibility |

## IoT/Embedded Specific Requirements

### Hardware Architecture

**Microcontroller:** ESP32-C3 Super Mini
- RISC-V single-core @ 160 MHz
- 400 KB SRAM, 4 MB Flash
- Native USB support (programming and serial)
- Built-in BLE 5.0 radio

**Peripherals:**
- Rotary encoder with integrated button (GPIO 0, 1, 2)
- SSD1306 OLED display 128x64 (I2C on GPIO 6, 7)
- GPIO buttons (configurable via config file, requires rebuild)

**Hardware Constraints:**
- Limited GPIO pins available for buttons (must avoid I2C, encoder, and reserved pins)
- Single-core processor - event-driven architecture preferred over blocking operations
- 4 MB flash must accommodate firmware + configuration storage

### Connectivity Protocol

**Primary:** Bluetooth Low Energy 5.0
- Protocol stack: NimBLE (lightweight, ESP32-optimized)
- Profile: HID over GATT (HOGP)
- Device name: "KnobKoKy"
- Supports: Keyboard keys, media keys, mouse scroll (horizontal/vertical)

**Connection Behavior:**
- Auto-advertise on boot
- Reconnect to last paired device automatically
- No multi-device pairing (single host at a time)

**Latency Requirements:**
- Input-to-HID response: < 50ms (imperceptible to user)
- Menu navigation: Responsive feel, no noticeable lag

### Power Profile

**MVP (USB Powered):**
- Power source: USB 5V via USB-C
- Typical consumption: 100-150 mA (BLE active + OLED)
- No power optimization required
- Always-on operation (no sleep modes)

**Future (Battery Powered - Growth Feature):**
- Target: LiPo 3.7V with onboard charging
- Will require sleep mode implementation
- Display timeout for power savings
- BLE advertising interval optimization

### Security Model

**Authentication:** Simple pairing
- Standard BLE pairing flow
- No PIN/passkey required
- "Pair once, trust forever" model

**Threat Model:** Personal device, low risk
- Physical access assumed to be authorized
- No sensitive data transmitted (HID commands only)
- No encryption beyond standard BLE

**Security Non-Goals (MVP):**
- Multi-user access control
- Secure boot
- Encrypted configuration storage

### Update Mechanism

**MVP:** USB reflash only
- PlatformIO build and upload
- No over-the-air updates
- Acceptable for maker/developer audience

**Future Consideration:**
- OTA updates listed in Vision scope
- Not required for MVP or Growth phases

## Functional Requirements

### Menu System

- FR1: User can enter the main menu by long-pressing the encoder button
- FR2: User can navigate menu items by rotating the encoder wheel
- FR3: User can select a menu item by short-pressing the encoder button
- FR4: User can return to the previous menu level by long-pressing the encoder button
- FR5: User can exit the menu system by long-pressing from the main menu level
- FR6: System displays the current menu item and available options via the display interface
- FR7: System displays confirmation feedback when a selection is made
- FR8: System automatically exits menu and returns to normal operation after configuration change

### Wheel Behavior Configuration

- FR9: User can view available wheel behavior modes (Scroll, Volume, Zoom)
- FR10: User can select and activate a wheel behavior mode
- FR11: System applies the selected wheel behavior immediately after selection
- FR12: User can use Scroll mode to send mouse scroll events (vertical)
- FR13: User can use Volume mode to send media volume up/down events
- FR14: User can use Zoom mode to send zoom in/out events

### Button Configuration

- FR15: User can view all configured buttons and their current behavior assignments
- FR16: User can select a button to configure its behavior
- FR17: User can assign a predefined behavior to a button (Mute, Play, Pause, Next, Previous)
- FR18: System applies button behavior assignment immediately after selection
- FR19: System sends the configured HID command when user presses a configured button

### Device Status

- FR20: User can view current wheel behavior mode
- FR21: User can view current button behavior assignments
- FR22: User can view Bluetooth connection status
- FR23: User can view Bluetooth connection strength (when available)

### About Screen

- FR24: User can view device name and firmware version
- FR25: User can view manufacturer information

### Configuration Persistence

- FR26: System saves wheel behavior mode to non-volatile storage when changed
- FR27: System saves button behavior assignments to non-volatile storage when changed
- FR28: System restores saved configuration automatically on boot
- FR29: System operates with default configuration if no saved configuration exists

### Bluetooth Connectivity

- FR30: System advertises as BLE HID device on boot
- FR31: System automatically reconnects to last paired host device
- FR32: System supports standard BLE pairing flow
- FR33: System sends HID keyboard/media key events for button actions
- FR34: System sends HID mouse scroll events for wheel actions

### Display Interface

- FR35: System outputs menu and status information to serial interface
- FR36: System provides abstract display interface for future hardware display support
- FR37: Display interface supports text output for menu items and status messages

### Input Handling

- FR38: System detects encoder rotation direction and magnitude
- FR39: System detects encoder button short press (< configurable threshold)
- FR40: System detects encoder button long press (>= configurable threshold)
- FR41: System detects GPIO button presses for configured buttons
- FR42: System processes inputs via event queue (non-blocking)

## Non-Functional Requirements

### Performance

- NFR1: Menu navigation shall feel responsive with no perceptible lag between input and display update

### Reliability

- NFR2: Saved configuration shall persist across power cycles with zero data loss
- NFR3: System shall handle invalid/corrupted configuration gracefully by falling back to defaults
- NFR4: Input events shall not be dropped under normal operation (queue-based processing)

### Maintainability

- NFR5: Code shall follow consistent naming conventions throughout the codebase
- NFR6: Architecture shall follow established patterns (event-driven, handler pattern, singleton where appropriate)
- NFR7: Display abstraction shall allow new display implementations without modifying menu logic
- NFR8: New wheel modes shall be addable by implementing a defined handler interface
- NFR9: New button behaviors shall be addable by extending the behavior registry
- NFR10: Code shall include header documentation for public interfaces
- NFR11: Build process shall be documented and reproducible via PlatformIO

### Compatibility

- NFR12: Device shall work with ESP32-C3 Super Mini board as reference hardware
