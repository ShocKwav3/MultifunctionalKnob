# Epic List

## Epic 1: Display & Persistence Foundation
User can see device feedback via serial output and their settings are remembered across power cycles.

**FRs covered:** FR26, FR27, FR28, FR29, FR35, FR36, FR37

**What this delivers:**
- Serial display output for all menu and status messages
- Abstract display interface (ready for future OLED)
- NVS-based configuration persistence
- Automatic config restore on boot with sensible defaults
- Factory reset capability

---

## Epic 2: Menu Navigation System
User can enter, navigate, select items, and exit the hierarchical menu using the encoder's two-input model (wheel rotation + button press).

**FRs covered:** FR1, FR2, FR3, FR4, FR5, FR6, FR7, FR8

**What this delivers:**
- Long-press to enter/exit menu
- Wheel rotation to browse options
- Short-press to select
- Hierarchical navigation with back capability
- Confirmation feedback on selection
- Auto-exit after configuration changes

---

## Epic 3: Wheel Behavior Configuration
User can view available wheel modes and switch between Scroll, Volume, and Zoom at runtime through the menu.

**FRs covered:** FR9, FR10, FR11, FR12, FR13, FR14

**What this delivers:**
- Wheel Behavior submenu with mode options
- Mode selection and immediate application
- Scroll mode (vertical mouse scroll)
- Volume mode (media volume up/down)
- Zoom mode (zoom in/out)
- Persisted mode selection

---

## Epic 4: Button System & Configuration
User can view, configure, and use hardware buttons with assigned media control behaviors.

**FRs covered:** FR15, FR16, FR17, FR18, FR19, FR33, FR41

**What this delivers:**
- Button GPIO setup and event detection
- Button Configurator submenu showing all buttons
- Behavior assignment (Mute, Play, Pause, Next, Previous)
- Immediate application of button behaviors
- HID command sending when buttons pressed
- Persisted button assignments

---

## Epic 5: Device Status & About Screens
User can view device status for troubleshooting and device information.

**FRs covered:** FR20, FR21, FR22, FR23, FR24, FR25

**What this delivers:**
- Device Status submenu showing:
  - Current wheel mode
  - Current button assignments
  - BLE connection status
  - BLE connection strength (when available)
- About screen showing:
  - Device name and firmware version
  - Manufacturer information

---

## Existing Functionality (No New Implementation Required)

The following FRs are already implemented in the existing codebase and will be integrated with the new menu system:

| FR | Description |
|----|-------------|
| FR30 | BLE HID advertising on boot |
| FR31 | Auto-reconnect to last paired device |
| FR32 | Standard BLE pairing flow |
| FR34 | Mouse scroll events for wheel |
| FR38 | Encoder rotation detection |
| FR39 | Short press detection |
| FR40 | Long press detection |
| FR42 | Event queue processing |
