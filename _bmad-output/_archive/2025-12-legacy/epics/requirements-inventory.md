# Requirements Inventory

## Functional Requirements

**Menu System (FR1-FR8):**
- FR1: User can enter the main menu by long-pressing the encoder button
- FR2: User can navigate menu items by rotating the encoder wheel
- FR3: User can select a menu item by short-pressing the encoder button
- FR4: User can return to the previous menu level by long-pressing the encoder button
- FR5: User can exit the menu system by long-pressing from the main menu level
- FR6: System displays the current menu item and available options via the display interface
- FR7: System displays confirmation feedback when a selection is made
- FR8: System automatically exits menu and returns to normal operation after configuration change

**Wheel Behavior Configuration (FR9-FR14):**
- FR9: User can view available wheel behavior modes (Scroll, Volume, Zoom)
- FR10: User can select and activate a wheel behavior mode
- FR11: System applies the selected wheel behavior immediately after selection
- FR12: User can use Scroll mode to send mouse scroll events (vertical)
- FR13: User can use Volume mode to send media volume up/down events
- FR14: User can use Zoom mode to send zoom in/out events

**Button Configuration (FR15-FR19):**
- FR15: User can view all configured buttons and their current behavior assignments
- FR16: User can select a button to configure its behavior
- FR17: User can assign a predefined behavior to a button (Mute, Play, Pause, Next, Previous)
- FR18: System applies button behavior assignment immediately after selection
- FR19: System sends the configured HID command when user presses a configured button

**Device Status (FR20-FR23):**
- FR20: User can view current wheel behavior mode
- FR21: User can view current button behavior assignments
- FR22: User can view Bluetooth connection status
- FR23: User can view Bluetooth connection strength (when available)

**About Screen (FR24-FR25):**
- FR24: User can view device name and firmware version
- FR25: User can view manufacturer information

**Configuration Persistence (FR26-FR29):**
- FR26: System saves wheel behavior mode to non-volatile storage when changed
- FR27: System saves button behavior assignments to non-volatile storage when changed
- FR28: System restores saved configuration automatically on boot
- FR29: System operates with default configuration if no saved configuration exists

**Bluetooth Connectivity (FR30-FR34):**
- FR30: System advertises as BLE HID device on boot
- FR31: System automatically reconnects to last paired host device
- FR32: System supports standard BLE pairing flow
- FR33: System sends HID keyboard/media key events for button actions
- FR34: System sends HID mouse scroll events for wheel actions

**Display Interface (FR35-FR37):**
- FR35: System outputs menu and status information to serial interface
- FR36: System provides abstract display interface for future hardware display support
- FR37: Display interface supports text output for menu items and status messages

**Input Handling (FR38-FR42):**
- FR38: System detects encoder rotation direction and magnitude
- FR39: System detects encoder button short press (< configurable threshold)
- FR40: System detects encoder button long press (>= configurable threshold)
- FR41: System detects GPIO button presses for configured buttons
- FR42: System processes inputs via event queue (non-blocking)

## Non-Functional Requirements

**Performance:**
- NFR1: Menu navigation shall feel responsive with no perceptible lag between input and display update

**Reliability:**
- NFR2: Saved configuration shall persist across power cycles with zero data loss
- NFR3: System shall handle invalid/corrupted configuration gracefully by falling back to defaults
- NFR4: Input events shall not be dropped under normal operation (queue-based processing)

**Maintainability:**
- NFR5: Code shall follow consistent naming conventions throughout the codebase
- NFR6: Architecture shall follow established patterns (event-driven, handler pattern, singleton where appropriate)
- NFR7: Display abstraction shall allow new display implementations without modifying menu logic
- NFR8: New wheel modes shall be addable by implementing a defined handler interface
- NFR9: New button behaviors shall be addable by extending the behavior registry
- NFR10: Code shall include header documentation for public interfaces
- NFR11: Build process shall be documented and reproducible via PlatformIO

**Compatibility:**
- NFR12: Device shall work with ESP32-C3 Super Mini board as reference hardware

## Additional Requirements

**From Architecture - Technical Constraints:**
- Static menu tree with parent pointers (no dynamic allocation, compile-time menu definition)
- Event interceptor pattern for menu activation (menu intercepts events at higher level)
- Command pattern for action execution (MenuAction base class with execute() method)
- NVS persistence using single namespace "knobkoky" with prefixed keys (wheel.mode, btn0.action, etc.)
- Enum storage with validation wrapper for graceful fallback on corruption
- 5-phase build order with specific component dependencies

**From Architecture - Implementation Requirements:**
- Display abstraction (DisplayInterface) with SerialDisplay implementation for MVP
- Button configuration via constexpr array of ButtonConfig structs
- Factory reset capability (encoder button held 5+ seconds during boot clears NVS)
- BLE disconnect handling - exit menu to root and show "Disconnected" status
- DI-enabled ConfigManager accepting Preferences* for testability
- LOG_* macros (LOG_ERROR, LOG_INFO, LOG_DEBUG) for all logging - no direct Serial.print
- Error enum return from all fallible operations (OK, INVALID_PARAM, NVS_READ_FAIL, NVS_WRITE_FAIL, INVALID_STATE)

**From Architecture - Directory Structure:**
- New directories: Menu/, Display/, Config/, Button/
- Subdirectory pattern: Controller/, Handler/, Interface/, Impl/, Model/, Action/
- Header files in include/Config/, include/Enum/, include/Type/

## FR Coverage Map

| FR | Epic | Description |
|----|------|-------------|
| FR1 | Epic 2 | Long-press enters menu |
| FR2 | Epic 2 | Wheel navigates menu |
| FR3 | Epic 2 | Short-press selects item |
| FR4 | Epic 2 | Long-press returns to previous level |
| FR5 | Epic 2 | Long-press from main exits menu |
| FR6 | Epic 2 | Display shows current menu/options |
| FR7 | Epic 2 | Confirmation feedback on selection |
| FR8 | Epic 2 | Auto-exit after config change |
| FR9 | Epic 3 | View available wheel modes |
| FR10 | Epic 3 | Select wheel mode |
| FR11 | Epic 3 | Immediate mode application |
| FR12 | Epic 3 | Scroll mode functionality |
| FR13 | Epic 3 | Volume mode functionality |
| FR14 | Epic 3 | Zoom mode functionality |
| FR15 | Epic 4 | View button assignments |
| FR16 | Epic 4 | Select button to configure |
| FR17 | Epic 4 | Assign predefined behavior |
| FR18 | Epic 4 | Immediate behavior application |
| FR19 | Epic 4 | HID command on button press |
| FR20 | Epic 5 | View current wheel mode |
| FR21 | Epic 5 | View button assignments |
| FR22 | Epic 5 | View BLE connection status |
| FR23 | Epic 5 | View BLE connection strength |
| FR24 | Epic 5 | View device name/version |
| FR25 | Epic 5 | View manufacturer info |
| FR26 | Epic 1 | Save wheel mode to NVS |
| FR27 | Epic 1 | Save button behaviors to NVS |
| FR28 | Epic 1 | Restore config on boot |
| FR29 | Epic 1 | Default config if none exists |
| FR30 | Existing | BLE advertising |
| FR31 | Existing | Auto-reconnect |
| FR32 | Existing | BLE pairing |
| FR33 | Epic 4 | HID keyboard/media events |
| FR34 | Existing | HID scroll events |
| FR35 | Epic 1 | Serial output |
| FR36 | Epic 1 | Abstract display interface |
| FR37 | Epic 1 | Text output support |
| FR38 | Existing | Encoder rotation detection |
| FR39 | Existing | Short press detection |
| FR40 | Existing | Long press detection |
| FR41 | Epic 4 | GPIO button detection |
| FR42 | Existing | Event queue processing |

---
