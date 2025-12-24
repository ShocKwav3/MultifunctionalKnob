# Peripheral Hardware

## 1. Rotary Encoder

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

## 2. OLED Display

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

## 3. Bluetooth Radio

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

## 4. USB Interface

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
