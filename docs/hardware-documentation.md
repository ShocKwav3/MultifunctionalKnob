# Hardware Documentation

**Project:** UtilityButtonsWithKnobUSB
**Hardware Platform:** ESP32-C3 Super Mini
**Last Updated:** 2025-12-14

## Hardware Overview

This project uses the ESP32-C3 Super Mini microcontroller with external peripherals for rotary encoder input and OLED visual feedback.

## Microcontroller Specifications

### ESP32-C3 Super Mini (Nologo Variant)

**Core Specifications:**
- **SoC:** Espressif ESP32-C3
- **CPU:** 32-bit RISC-V single-core @ 160 MHz
- **RAM:** 400 KB SRAM
- **Flash:** 4 MB (typical)
- **Wireless:** 2.4 GHz WiFi + Bluetooth 5 (BLE)
- **USB:** Built-in USB Serial/JTAG (no external USB-UART chip needed)
- **Form Factor:** Super Mini (~22.5mm x 18mm)
- **Supply Voltage:** 3.3V (USB 5V with onboard regulator)

**Key Features:**
- Native USB support (USB CDC, DFU)
- Hardware AES, SHA, RSA acceleration
- Low power consumption
- 22 programmable GPIOs
- Built-in antenna

**Board Definition:** [`boards/nologo_esp32c3_super_mini.json`](../boards/nologo_esp32c3_super_mini.json:1)

## Pin Configuration

### GPIO Assignments

| GPIO | Function | Connected To | Direction | Notes |
|------|----------|--------------|-----------|-------|
| **0** | Encoder DT | Rotary Encoder Channel B | Input (Pull-up) | Quadrature signal |
| **1** | Encoder CLK | Rotary Encoder Channel A | Input (Pull-up) | Quadrature signal, ISR attached |
| **2** | Encoder SW | Rotary Encoder Button | Input (Pull-up) | Press detection |
| **6** | I2C SDA | OLED Display Data | Bidirectional | I2C bus |
| **7** | I2C SCL | OLED Display Clock | Output | I2C bus |
| **-1** | Encoder VCC | (Not used) | - | Power via external supply |

**Configuration Source:** [`include/Config/encoder_config.h`](../include/Config/encoder_config.h:1)

### Pin Electrical Characteristics

- **Input Mode:** Internal pull-up resistors enabled
- **ISR Triggers:** Both rising and falling edges (quadrature decoding)
- **I2C Speed:** Standard mode (100 kHz) or Fast mode (400 kHz)
- **Button Debouncing:** Software debounce via timing (50ms short, 1000ms long)

## Peripheral Hardware

### 1. Rotary Encoder

**Type:** Incremental quadrature encoder with integrated push button

**Specifications:**
- **Steps per Revolution:** Configured for 4 steps per detent
- **Quadrature Outputs:** CLK (Channel A), DT (Channel B)
- **Button:** Momentary push switch (normally open)
- **Operating Voltage:** 3.3V - 5V
- **Output Type:** Open collector or push-pull (requires pull-ups)

**Typical Part:** EC11 or compatible rotary encoder

**Connection Diagram:**
```
Rotary Encoder          ESP32-C3
    CLK (A) ──────────── GPIO 1
    DT  (B) ──────────── GPIO 0
    SW      ──────────── GPIO 2
    +       ──────────── 3.3V (external or VCC pin if available)
    GND     ──────────── GND
```

**Driver Implementation:** Custom wrapper around `ai-esp32-rotary-encoder` library
**Driver Class:** [`lib/EncoderDriver/EncoderDriver.h`](../lib/EncoderDriver/EncoderDriver.h:1)

**Features:**
- Interrupt-driven quadrature decoding
- Software debouncing
- Short click detection (< 50ms)
- Long click detection (≥ 1000ms)
- FreeRTOS task for button timing

### 2. OLED Display

**Model:** SSD1306-based OLED Display

**Specifications:**
- **Resolution:** 128 x 64 pixels
- **Display Technology:** Monochrome OLED
- **Interface:** I2C
- **I2C Address:** 0x3C (fixed)
- **Operating Voltage:** 3.3V - 5V
- **Dimensions:** 0.96" diagonal (typical)

**Connection Diagram:**
```
SSD1306 Display         ESP32-C3
    VCC     ──────────── 3.3V or 5V
    GND     ──────────── GND
    SDA     ──────────── GPIO 6
    SCL     ──────────── GPIO 7
```

**Driver Library:** Adafruit SSD1306 (v2.5.15)
**Graphics Library:** Adafruit GFX (dependency)

**Configuration:**
- **Screen Width:** 128 pixels (defined as `SCREEN_WIDTH`)
- **Screen Height:** 64 pixels (defined as `SCREEN_HEIGHT`)
- **Reset Pin:** -1 (software reset via I2C)
- **I2C Address:** 0x3C (defined as `SCREEN_ADDRESS`)

**I2C Bus Configuration:**
- **SDA Pin:** GPIO 6 (defined as `SDA_PIN`)
- **SCL Pin:** GPIO 7 (defined as `SCL_PIN`)
- **Clock Speed:** Default (typically 100 kHz)

### 3. Bluetooth Radio

**Technology:** Bluetooth Low Energy (BLE) 5.0

**Specifications:**
- **Built-in:** ESP32-C3 integrated radio
- **Frequency:** 2.4 GHz ISM band
- **Protocol Stack:** NimBLE (lightweight BLE stack)
- **Range:** ~10m typical (Class 2)
- **Power Output:** Configurable (up to +21 dBm)

**BLE Profile:** HID over GATT (HOGP)

**Device Configuration:**
- **Device Name:** "KnobKoKy" (defined in [`device_config.h`](../include/Config/device_config.h:1))
- **Manufacturer:** "KoKy"
- **Battery Level:** 100% (default, not actively monitored)
- **HID Services:** Keyboard + Mouse (Consumer Control)

**Library:** ESP32-BLE-Keyboard (ShocKwav3 fork)
**Dependency:** NimBLE-Arduino v2.2.3

**Supported HID Features:**
- Keyboard key presses
- Media keys (play/pause, volume up/down, etc.)
- Mouse movement (X, Y axes)
- Mouse wheel scroll (vertical and horizontal)

### 4. USB Interface

**Type:** Native USB (USB Serial/JTAG)

**Specifications:**
- **Controller:** Built-in ESP32-C3 USB peripheral
- **USB Mode:** USB CDC (Communication Device Class)
- **Connector:** USB-C (on Super Mini board)
- **Speed:** Full Speed USB 2.0 (12 Mbps)
- **Functions:**
  - Serial communication (virtual COM port)
  - Programming/upload (built-in bootloader)
  - JTAG debugging

**Configuration:**
```c
#define ARDUINO_USB_MODE=1          // Enable USB
#define ARDUINO_USB_CDC_ON_BOOT=1   // USB CDC available at boot
```

**Serial Baud Rate:** 460800 (defined in [`main.cpp`](../src/main.cpp:36) and [`platformio.ini`](../platformio.ini:18))

## Power Requirements

### Power Supply

**Input:** USB 5V via USB-C connector

**Onboard Regulation:**
- USB 5V → 3.3V LDO regulator (built-in on Super Mini)
- 3.3V rail powers ESP32-C3, peripherals

### Current Consumption

**Estimated Operating Current:**
- **ESP32-C3 Active (WiFi/BT off):** ~40 mA
- **ESP32-C3 + BLE Active:** ~80-120 mA (depending on TX power)
- **OLED Display:** ~15-30 mA (depending on pixel activity)
- **Total (typical operation):** ~100-150 mA @ 5V USB

**Peak Current:** ~200 mA during BLE transmission

**Notes:**
- No deep sleep implemented (continuous operation)
- Power optimization possible via sleep modes
- Battery operation possible with 3.3V LiPo + voltage regulator

## Build and Assembly

### Bill of Materials (BOM)

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

### Wiring Instructions

#### Step 1: Connect Rotary Encoder
1. Connect encoder CLK → ESP32-C3 GPIO 1
2. Connect encoder DT → ESP32-C3 GPIO 0
3. Connect encoder SW → ESP32-C3 GPIO 2
4. Connect encoder + → 3.3V
5. Connect encoder GND → GND

#### Step 2: Connect OLED Display
1. Connect OLED VCC → 3.3V (or 5V if display supports it)
2. Connect OLED GND → GND
3. Connect OLED SDA → ESP32-C3 GPIO 6
4. Connect OLED SCL → ESP32-C3 GPIO 7

#### Step 3: Power and Programming
1. Connect ESP32-C3 to computer via USB-C cable
2. Verify power LED on board illuminates
3. Board should enumerate as USB serial device

### Testing Hardware

#### Basic Hardware Test Checklist

- [ ] Power: USB connection provides stable 5V
- [ ] USB Serial: Device appears as COM port
- [ ] OLED: Display shows "Knob firmware!" on bootup
- [ ] Encoder Rotation: Serial monitor shows rotation events
- [ ] Short Click: Serial monitor shows click events
- [ ] Long Click: Serial monitor shows long press events
- [ ] Bluetooth: Device appears as "KnobKoKy" in BLE scan
- [ ] HID Function: Scroll events control connected device

#### Hardware Test Procedure

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

## Troubleshooting

### Common Hardware Issues

#### Issue: OLED Display Not Working

**Symptoms:** Blank display, no initialization

**Checks:**
1. Verify I2C address (0x3C) using I2C scanner
2. Check SDA/SCL connections (GPIO 6, 7)
3. Verify 3.3V power to display
4. Check pull-up resistors (usually built-in on display module)
5. Test with different I2C speed

#### Issue: Encoder Not Responding

**Symptoms:** No rotation detected, no click events

**Checks:**
1. Verify CLK, DT, SW pin connections (GPIO 1, 0, 2)
2. Check encoder power connection (3.3V, GND)
3. Verify pull-up resistors enabled in code
4. Test encoder with multimeter (continuity when pressed, voltage change on rotation)
5. Check encoder steps configuration (4 steps per detent)

#### Issue: Bluetooth Not Pairing

**Symptoms:** Device not visible, pairing fails

**Checks:**
1. Verify NimBLE library initialization in serial output
2. Check Bluetooth is enabled on host device
3. Confirm device name "KnobKoKy" in BLE scan
4. Remove old pairings and retry
5. Check ESP32-C3 antenna connection (built-in PCB antenna)
6. Verify sufficient power supply (BLE draws significant current)

#### Issue: USB Serial Not Detected

**Symptoms:** No COM port, cannot upload

**Checks:**
1. Try different USB cable (must support data, not charge-only)
2. Verify USB drivers installed (CP210x or CH340, depending on board)
3. Check USB CDC flags in `platformio.ini`
4. Hold BOOT button while plugging in USB (force bootloader mode)
5. Reset board after connecting

## Hardware Modifications and Extensions

### Possible Hardware Enhancements

1. **Battery Power:**
   - Add LiPo battery (3.7V) with charging circuit
   - Connect to 3.3V rail (bypass USB regulator)
   - Add power switch

2. **Additional Buttons:**
   - Add GPIO buttons for direct mode selection
   - Useful GPIOs: 3, 4, 5, 8, 9, 10

3. **Status LEDs:**
   - Add RGB LED for mode indication
   - PWM control for brightness
   - Available GPIOs: Multiple pins available

4. **External Antenna:**
   - Improve Bluetooth range with external antenna
   - Requires board rework (antenna switch)

5. **Haptic Feedback:**
   - Add vibration motor
   - Connect via transistor to GPIO
   - Provides tactile response

6. **Additional Encoders:**
   - Add second encoder for multi-axis control
   - ESP32-C3 has sufficient GPIOs

## Hardware Design Files

### Schematic

*Note: Schematic not yet created. Hardware connections documented above.*

### PCB Layout

*Note: PCB design not yet created. Current implementation uses breadboard or point-to-point wiring.*

### Mechanical Drawings

*Note: Enclosure design not yet created.*

## References

### Datasheets and Technical Documents

1. **ESP32-C3 Technical Reference Manual:**
   - <https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf>

2. **ESP32-C3 Datasheet:**
   - <https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf>

3. **SSD1306 OLED Controller Datasheet:**
   - <https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf>

4. **EC11 Rotary Encoder Datasheet:**
   - <https://tech.alpsalpine.com/prod/e/html/encoder/incremental/ec11/ec11_list.html>

### Library Documentation

1. **ai-esp32-rotary-encoder:**
   - <https://github.com/igorantolic/ai-esp32-rotary-encoder>

2. **Adafruit SSD1306:**
   - <https://github.com/adafruit/Adafruit_SSD1306>

3. **ESP32-BLE-Keyboard:**
   - <https://github.com/T-vK/ESP32-BLE-Keyboard>
   - Fork used: <https://github.com/ShocKwav3/ESP32-BLE-Keyboard>

4. **NimBLE-Arduino:**
   - <https://github.com/h2zero/NimBLE-Arduino>

---

*Hardware documentation generated on 2025-12-14 during deep scan documentation workflow.*
