# Functional Requirements

## Menu System

- FR1: User can enter the main menu by long-pressing the encoder button
- FR2: User can navigate menu items by rotating the encoder wheel
- FR3: User can select a menu item by short-pressing the encoder button
- FR4: User can return to the previous menu level by long-pressing the encoder button
- FR5: User can exit the menu system by long-pressing from the main menu level
- FR6: System displays the current menu item and available options via the display interface
- FR7: System displays confirmation feedback when a selection is made
- FR8: System automatically exits menu and returns to normal operation after configuration change

## Wheel Behavior Configuration

- FR9: User can view available wheel behavior modes (Scroll, Volume, Zoom)
- FR10: User can select and activate a wheel behavior mode
- FR11: System applies the selected wheel behavior immediately after selection
- FR12: User can use Scroll mode to send mouse scroll events (vertical)
- FR13: User can use Volume mode to send media volume up/down events
- FR14: User can use Zoom mode to send zoom in/out events

## Button Configuration

- FR15: User can view all configured buttons and their current behavior assignments
- FR16: User can select a button to configure its behavior
- FR17: User can assign a predefined behavior to a button (Mute, Play, Pause, Next, Previous)
- FR18: System applies button behavior assignment immediately after selection
- FR19: System sends the configured HID command when user presses a configured button

## Device Status

- FR20: User can view current wheel behavior mode
- FR21: User can view current button behavior assignments
- FR22: User can view Bluetooth connection status
- FR23: User can view Bluetooth connection strength (when available)

## About Screen

- FR24: User can view device name and firmware version
- FR25: User can view manufacturer information

## Configuration Persistence

- FR26: System saves wheel behavior mode to non-volatile storage when changed
- FR27: System saves button behavior assignments to non-volatile storage when changed
- FR28: System restores saved configuration automatically on boot
- FR29: System operates with default configuration if no saved configuration exists

## Bluetooth Connectivity

- FR30: System advertises as BLE HID device on boot
- FR31: System automatically reconnects to last paired host device
- FR32: System supports standard BLE pairing flow
- FR33: System sends HID keyboard/media key events for button actions
- FR34: System sends HID mouse scroll events for wheel actions

## Display Interface

- FR35: System outputs menu and status information to serial interface
- FR36: System provides abstract display interface for future hardware display support
- FR37: Display interface supports text output for menu items and status messages

## Input Handling

- FR38: System detects encoder rotation direction and magnitude
- FR39: System detects encoder button short press (< configurable threshold)
- FR40: System detects encoder button long press (>= configurable threshold)
- FR41: System detects GPIO button presses for configured buttons
- FR42: System processes inputs via event queue (non-blocking)
