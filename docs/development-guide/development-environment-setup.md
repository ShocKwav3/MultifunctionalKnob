# Development Environment Setup

## Prerequisites

### Required Software

1. **PlatformIO Core or IDE**
   - PlatformIO Core (CLI): Recommended
   - PlatformIO IDE (VSCode extension): Alternative
   - Install: <https://docs.platformio.org/en/latest/core/installation.html>

2. **Python** (for PlatformIO)
   - Version: 3.7 or newer
   - Install: <https://www.python.org/downloads/>

3. **USB Drivers** (ESP32-C3)
   - Built-in USB CDC support (no additional drivers typically needed)
   - Windows: May need WinUSB driver (<https://zadig.akeo.ie/>)

4. **Git** (recommended for version control)
   - Install: <https://git-scm.com/downloads>

### Optional Tools

- **Serial Monitor:** PlatformIO built-in, or alternatives (PuTTY, screen, minicom)
- **Bluetooth Scanner:** nRF Connect app (mobile) or similar
- **Logic Analyzer:** For debugging I2C/encoder signals (optional)

## Installation Steps

### 1. Install PlatformIO

**Via pip (recommended):**
```bash
pip install platformio
```

**Via script:**
```bash
# Linux/Mac
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py
python3 get-platformio.py

# Windows (PowerShell)
Invoke-WebRequest -Uri https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -OutFile get-platformio.py
python get-platformio.py
```

**Via VSCode:**
1. Install Visual Studio Code
2. Install PlatformIO IDE extension from marketplace
3. Restart VSCode

### 2. Clone Repository

```bash
git clone <repository-url>
cd UtilityButtonsWithKnobUSB
```

### 3. Install Project Dependencies

PlatformIO will automatically install dependencies on first build:

```bash
pio run
```

This downloads:
- ESP32 platform (espressif32)
- Arduino framework
- Required libraries (NimBLE, BleKeyboard, SSD1306, etc.)

## Project Configuration

### Build Environments

**File:** [`platformio.ini`](../platformio.ini:1)

**Available Environments:**

1. **`env` (default):**
   - Platform: Espressif32
   - Board: ESP32-C3 Super Mini (custom)
   - Framework: Arduino
   - USB CDC enabled

2. **`use_nimble`:**
   - Inherits from `env`
   - Defines `USE_NIMBLE` flag
   - Uses NimBLE stack

3. **`use_stdble`:**
   - Inherits from `env`
   - Defines `USE_STDBLE` flag
   - Uses standard BLE stack

**Default Build:**
```bash
pio run
# Builds default environment (env)
```

**Specific Environment:**
```bash
pio run -e use_nimble
```

### Configuration Files

**Device Configuration:** [`include/Config/device_config.h`](../include/Config/device_config.h:1)
- Bluetooth device name: `BLUETOOTH_DEVICE_NAME`
- Manufacturer: `BLUETOOTH_DEVICE_MANUFACTURER`
- Battery level: `BLUETOOTH_DEVICE_BATTERY_LEVEL_DEFAULT`
- Button timings: `KNOB_BUTTON_SHORT_PRESS_MS`, `KNOB_BUTTON_LONG_PRESS_MS`

**Encoder Configuration:** [`include/Config/encoder_config.h`](../include/Config/encoder_config.h:1)
- Pin assignments: `ENCODER_PIN_A`, `ENCODER_PIN_B`, `ENCODER_PIN_BUTTON`
- Encoder parameters: `ENCODER_STEPS`, `ENCODER_PIN_VCC`

**Display Configuration:** [`src/main.cpp`](../src/main.cpp:22)
- Display size: `SCREEN_WIDTH`, `SCREEN_HEIGHT`
- I2C address: `SCREEN_ADDRESS`
- I2C pins: `SDA_PIN`, `SCL_PIN`
