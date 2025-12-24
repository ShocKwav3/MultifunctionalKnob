# Build and Assembly

## Bill of Materials (BOM)

| Qty | Component | Specifications | Notes |
|-----|-----------|----------------|-------|
| 1 | ESP32-C3 Super Mini | Nologo variant | Microcontroller board |
| 1 | Rotary Encoder | EC11 or compatible, w/ button | Input device |
| 1 | OLED Display | SSD1306, 128x64, I2C, 0.96" | Visual output |
| 5 | Jumper Wires | Female-Female, ~10cm | Connections (if breadboarding) |
| 1 | USB-C Cable | Data capable | Programming and power |

**Optional:**
- Breadboard or custom PCB
- Enclosure/case
- Knob cap for encoder shaft

## Wiring Instructions

### Step 1: Connect Rotary Encoder
1. Connect encoder CLK → ESP32-C3 GPIO 1
2. Connect encoder DT → ESP32-C3 GPIO 0
3. Connect encoder SW → ESP32-C3 GPIO 2
4. Connect encoder + → 3.3V
5. Connect encoder GND → GND

### Step 2: Connect OLED Display
1. Connect OLED VCC → 3.3V (or 5V if display supports it)
2. Connect OLED GND → GND
3. Connect OLED SDA → ESP32-C3 GPIO 6
4. Connect OLED SCL → ESP32-C3 GPIO 7

### Step 3: Power and Programming
1. Connect ESP32-C3 to computer via USB-C cable
2. Verify power LED on board illuminates
3. Board should enumerate as USB serial device

## Testing Hardware

### Basic Hardware Test Checklist

- [ ] Power: USB connection provides stable 5V
- [ ] USB Serial: Device appears as COM port
- [ ] OLED: Display shows "Knob firmware!" on bootup
- [ ] Encoder Rotation: Serial monitor shows rotation events
- [ ] Short Click: Serial monitor shows click events
- [ ] Long Click: Serial monitor shows long press events
- [ ] Bluetooth: Device appears as "KnobKoKy" in BLE scan
- [ ] HID Function: Scroll events control connected device

### Hardware Test Procedure

1. **Upload Firmware:**
   ```bash
   pio run --target upload
   ```

2. **Open Serial Monitor:**
   ```bash
   pio device monitor --baud 460800
   ```

3. **Verify Display:**
   - OLED should show welcome message
   - Display should be clearly visible

4. **Test Encoder:**
   - Rotate encoder clockwise/counter-clockwise
   - Verify value changes in encoder driver
   - Short press button (< 50ms)
   - Long press button (> 1s)

5. **Test Bluetooth:**
   - Pair device with computer/phone
   - Verify "KnobKoKy" appears in device list
   - Confirm pairing completes
   - Test scroll functionality
