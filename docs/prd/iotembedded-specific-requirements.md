# IoT/Embedded Specific Requirements

## Hardware Architecture

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

## Connectivity Protocol

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

## Power Profile

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

## Security Model

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

## Update Mechanism

**MVP:** USB reflash only
- PlatformIO build and upload
- No over-the-air updates
- Acceptable for maker/developer audience

**Future Consideration:**
- OTA updates listed in Vision scope
- Not required for MVP or Growth phases
