---
stepsCompleted: [1, 2, 3]
inputDocuments: []
session_topic: 'Evolving ESP32-C3 Bluetooth HID device from simple mode-switching wheel into a multi-function configurable peripheral with robust menu and navigation system'
session_goals: 'Design hierarchical menu structure, define wheel behavior options, design button configurator interface, plan device status display, identify technical architecture needs, map user interaction patterns'
selected_approach: 'Direct Structured Capture'
techniques_used: ['Direct Structured Capture']
ideas_generated: ['4-item main menu structure', 'wheel behavior modes', 'button configurator flow', 'device status display', 'universal long-press back navigation', 'display-agnostic architecture', 'config-driven button management']
context_file: '.bmad/bmm/data/project-context-template.md'
---

# Brainstorming Session Results

**Facilitator:** Feroj
**Date:** 2025-12-14

## Session Overview

**Topic:** Evolving ESP32-C3 Bluetooth HID device from simple mode-switching wheel into a multi-function configurable peripheral with robust menu and navigation system

**Goals:**
- Design menu structure and navigation flow (long press → menu, short press → select/navigate)
- Define wheel behavior options and selection mechanism
- Design button configurator interface (list buttons → select button → choose behavior from predefined options)
- Plan device status display functionality (connectivity strength, battery, etc.)
- Identify technical architecture needs to support this expanded functionality
- Map user interaction patterns and navigation paths

### Context Guidance

This brainstorming session focuses on software and product development considerations for a brownfield ESP32-C3 Bluetooth peripheral device. The project currently has a working wheel with modes (scroll, volume, zoom) and basic long-press menu functionality. We're evolving it into a comprehensive multi-level menu system with wheel configuration, button configuration, and device status features. Key technical needs include menu framework architecture, button input handling, and state management.

### Session Setup

**Current State:**
- ✅ Working: Wheel with modes (scroll, volume, zoom) + basic long-press menu
- 🎯 Evolving To: Multi-level menu system with wheel config, button config, and device status
- 🔧 Technical Need: Menu framework, button input handling, state management

## Technique Selection

**Approach:** Direct Structured Capture
**Rationale:** User has clear vision - fastest approach is direct capture and organization without elaborate brainstorming techniques.

---

## Menu Structure & Navigation Design

### Main Menu (Root Level)

**Top-Level Menu Items:**
1. Wheel Behavior
2. Button Configurator
3. Device Status
4. About

_Note: Designed for extensibility - additional items can be added in future iterations_

---

### Sub-Menu 1: Wheel Behavior

**Purpose:** Configure wheel operating mode

**Available Modes:**
- Scroll
- Volume
- Zoom

_Note: Mode list designed for future expansion_

**Navigation:**
- Wheel rotation: Navigate between mode options
- Short press: Select mode (becomes active wheel behavior)
- Long press: Return to Main Menu

---

### Sub-Menu 2: Button Configurator

**Purpose:** Configure behavior for all connected buttons

**Button List Display:**
- Shows all connected buttons (dynamically read from config file)
- Includes wheel's built-in button (treated as standard connected button)
- Count varies based on actual hardware configuration

**Configuration Flow:**
1. Display list of all connected buttons
2. User selects button (wheel rotation + short press)
3. Display predefined behavior options for that button
4. User selects behavior from list

**Predefined Button Behaviors:**
_(To be provided - examples include)_
- Mute
- Play
- Pause
- Next
- Previous
- _[Additional behaviors TBD]_

**Navigation:**
- Wheel rotation: Navigate button list OR navigate behavior options (context-dependent)
- Short press: Select button → Enter behavior selection → Confirm behavior assignment
- Long press: Return to previous level (behavior list → button list → Main Menu)

---

### Sub-Menu 3: Device Status

**Purpose:** Display read-only device information

**Status Information Displayed:**
- Bluetooth connection strength
- Battery level
- Current active wheel behavior
- Current button behavior configurations

**Interaction:**
- Read-only display (no configuration options)
- Wheel rotation: Scroll through status information (if needed)
- Long press: Return to Main Menu

---

### Sub-Menu 4: About

**Purpose:** Display device/firmware information

**Content:** _[To be defined]_

---

## Navigation Interaction Model

### Global Navigation Rules

**Wheel Rotation:**
- Navigate between menu items (up/down through list)
- Works at all menu levels

**Short Press:**
- Select current menu item
- Navigate into sub-menu or activate selection

**Long Press:**
- Go back to previous menu level
- If at root (Main Menu): Exit to normal operating mode
- If in sub-menu: Return to parent menu
- Consistent "back" behavior throughout entire menu system

---

## Technical Architecture Considerations

### Display Strategy

**Current Development State:**
- Using serial monitor as display for development/debugging
- Allows testing full menu navigation without hardware display

**Future State:**
- Physical display will be added later
- Architecture must be display-agnostic
- Menu system should work via abstract display interface
- Easy swap from serial output to hardware display (OLED/LCD)

**Design Principle:** Develop menu logic and navigation independently of display implementation

---

### Configuration Management

**Button Configuration:**
- Read button list from config file (dynamic)
- Allows flexible hardware configurations
- Supports different button counts without code changes

**Extensibility Points:**
- New wheel behaviors can be added to mode list
- New button behaviors can be added to predefined list
- New main menu items can be added
- Menu structure supports unlimited nesting depth

---

### State Management Requirements

**System Must Track:**
1. Current menu level and position
2. Active wheel behavior (when not in menu)
3. Button behavior assignments for all connected buttons
4. Navigation history for proper back-button behavior
5. Device status (connection, battery, etc.)

**State Persistence:**
- Active configurations must persist across power cycles
- Menu position can reset on exit (doesn't need persistence)

---

## Implementation Notes

### Hardware Context
- ESP32-C3 microcontroller
- Bluetooth HID device (connects to PC as peripheral)
- Rotary encoder (wheel) with integrated button
- Multiple external buttons (count configurable)
- Future: Display module (type TBD)

### Memory Considerations
- ESP32-C3 has limited memory
- Menu strings should be stored efficiently (PROGMEM/Flash)
- State tracking should be minimal and efficient

### Interaction Design Philosophy
- Two-button interface (wheel rotation + press)
- Simple, predictable navigation model
- Long press = always "back"
- Short press = always "select/confirm"
- Wheel = always "navigate"

---

## Next Steps / Outstanding Items

**Information Still Needed:**
1. Complete list of predefined button behaviors
2. About menu content/information to display
3. Display hardware selection (OLED size/type)
4. Battery monitoring implementation details
5. Bluetooth signal strength measurement approach

**Future Enhancements Considered:**
- Additional wheel behavior modes
- More main menu items
- Advanced button configuration (macros, sequences)
- Settings persistence/backup
- Firmware update mechanism

---

## Session Summary

**Outcome:** Clear, implementable menu structure and navigation model for ESP32-C3 Bluetooth peripheral device.

**Key Decisions Made:**
- Four main menu items with room for expansion
- Hierarchical navigation with consistent interaction model
- Display-agnostic architecture for future hardware flexibility
- Config-driven button management for hardware flexibility
- Long-press as universal "back" mechanism

**Ready for Next Phase:**
This specification can feed directly into:
- PRD (Product Requirements Document)
- Technical Architecture design
- Implementation planning
- UX/Display design (when hardware selected)
