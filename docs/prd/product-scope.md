# Product Scope

## MVP - Minimum Viable Product

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

## Growth Features (Post-MVP)

Features that complete the vision after core functionality is proven:

- **Hardware Display**: OLED display support using the abstraction built in MVP
- **Device Status Screen**: Bluetooth connection strength, battery level, current configurations
- **Expanded Wheel Modes**: Additional modes beyond Scroll/Volume/Zoom
- **Expanded Button Behaviors**: More predefined options (media controls, shortcuts, modifiers)
- **Visual Polish**: Icons, animations, improved display layouts

## Vision (Future)

Long-term possibilities for the platform:

- **Custom Macros**: User-defined key sequences and multi-action behaviors
- **Settings Backup/Restore**: Export and import configurations
- **OTA Firmware Updates**: Update firmware without USB connection
- **Community Behaviors**: Library of community-contributed button behavior definitions
- **Multiple Profiles**: Switch between different configuration profiles
