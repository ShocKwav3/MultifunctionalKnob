# Development Guide

**Project:** UtilityButtonsWithKnobUSB
**Last Updated:** 2025-12-14

## Development Environment Setup

### Prerequisites

#### Required Software

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

#### Optional Tools

- **Serial Monitor:** PlatformIO built-in, or alternatives (PuTTY, screen, minicom)
- **Bluetooth Scanner:** nRF Connect app (mobile) or similar
- **Logic Analyzer:** For debugging I2C/encoder signals (optional)

### Installation Steps

#### 1. Install PlatformIO

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

#### 2. Clone Repository

```bash
git clone <repository-url>
cd UtilityButtonsWithKnobUSB
```

#### 3. Install Project Dependencies

PlatformIO will automatically install dependencies on first build:

```bash
pio run
```

This downloads:
- ESP32 platform (espressif32)
- Arduino framework
- Required libraries (NimBLE, BleKeyboard, SSD1306, etc.)

### Project Configuration

#### Build Environments

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

#### Configuration Files

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

## Building the Project

### Basic Build Commands

#### Compile
```bash
pio run
```

**Output:**
- Firmware binary: `.pio/build/<env>/firmware.bin`
- ELF file: `.pio/build/<env>/firmware.elf`
- MAP file: `.pio/build/<env>/firmware.map`

#### Clean Build
```bash
pio run --target clean
pio run
```

#### Verbose Build
```bash
pio run --verbose
```

### Build Flags

**Current Flags** (from [`platformio.ini`](../platformio.ini:15)):

```ini
build_flags =
    -D ARDUINO_USB_MODE=1          # Enable USB
    -D ARDUINO_USB_CDC_ON_BOOT=1   # USB CDC at boot
```

**Additional Flags** (examples):

```ini
build_flags =
    -D DEBUG_MODE=1                # Enable debug output
    -D LOG_LEVEL=4                 # Set log verbosity
    -D CORE_DEBUG_LEVEL=3          # ESP32 core debug level
```

### Dependency Management

**Library Dependencies** (from [`platformio.ini`](../platformio.ini:19)):

```ini
lib_deps =
    h2zero/NimBLE-Arduino@^2.2.3
    https://github.com/igorantolic/ai-esp32-rotary-encoder@^1.7
    adafruit/Adafruit SSD1306@^2.5.15
    BleKeyboard = https://github.com/ShocKwav3/ESP32-BLE-Keyboard.git
```

**Update Dependencies:**
```bash
pio pkg update
```

**Install Specific Library:**
```bash
pio pkg install --library "library-name@version"
```

## Uploading Firmware

### Upload to Device

#### 1. Connect ESP32-C3 via USB

Ensure device is connected and recognized:

```bash
# List available devices
pio device list
```

#### 2. Upload Firmware

**Auto-detect port:**
```bash
pio run --target upload
```

**Specific port:**
```bash
pio run --target upload --upload-port /dev/ttyACM0    # Linux/Mac
pio run --target upload --upload-port COM3            # Windows
```

#### 3. Monitor Output

**Upload and Monitor (combined):**
```bash
pio run --target upload --target monitor
```

### Troubleshooting Upload

#### Issue: Device Not Found

**Solutions:**
1. Check USB cable (must support data transfer)
2. Try different USB port
3. Install/update USB drivers
4. Press BOOT button while connecting (enter bootloader mode)

#### Issue: Permission Denied (Linux)

**Solution:**
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER
# Log out and back in for changes to take effect
```

#### Issue: Upload Fails Midway

**Solutions:**
1. Hold BOOT button during upload
2. Reset board after connecting
3. Lower upload speed in `platformio.ini`:
   ```ini
   upload_speed = 115200
   ```

## Testing and Debugging

### Serial Monitor

#### Open Monitor

```bash
pio device monitor --baud 460800
```

**Monitor with filters:**
```bash
pio device monitor --baud 460800 --filter esp32_exception_decoder
```

**Exit:** `Ctrl+C` or `Ctrl+]`

#### Serial Output

**In Code:**
```cpp
Serial.begin(460800);
Serial.println("Debug message");
Serial.printf("Value: %d\n", value);
```

**Current Debug Output:**
- Encoder rotation events
- Button click events
- Bluetooth connection status
- Scroll commands (test loop)

### Hardware Testing

#### Test Encoder Input

1. Upload firmware with serial debug enabled
2. Open serial monitor
3. Rotate encoder - observe value changes
4. Press button - observe click events
5. Long press button - observe long click events

#### Test OLED Display

1. Upload firmware
2. Verify "Knob firmware!" appears on display
3. Check display brightness and contrast
4. Test different display content

#### Test Bluetooth HID

1. Pair device with computer/phone
2. Open document or web page
3. Rotate encoder - observe scroll behavior
4. Short click - observe mode indication
5. Long click - test mode selection

### Debugging Tools

#### ESP32 Exception Decoder

**Install filter:**
```bash
pio pkg install --tool platformio/tool-pvvx-ble-monitor
```

**Use in monitor:**
```bash
pio device monitor --filter esp32_exception_decoder
```

**Decodes stack traces from crashes**

#### GDB Debugging (Advanced)

**Requirements:**
- JTAG debugger or ESP32-C3 built-in JTAG
- OpenOCD configuration

**Debug Configuration (`platformio.ini`):**
```ini
debug_tool = esp-builtin
debug_init_break = tbreak setup
```

**Start Debug Session:**
```bash
pio debug
```

#### Logic Analyzer

**Useful for:**
- I2C bus analysis (OLED communication)
- Encoder signal decoding
- Timing verification

**Recommended Tools:**
- Saleae Logic Analyzer
- PulseView + cheap USB logic analyzer
- Oscilloscope with protocol decoding

## Development Workflow

### Typical Development Cycle

1. **Edit Code**
   - Modify source files in `src/` or `lib/`
   - Update configuration in `include/Config/`

2. **Build**
   ```bash
   pio run
   ```

3. **Upload**
   ```bash
   pio run --target upload
   ```

4. **Test**
   ```bash
   pio device monitor --baud 460800
   ```

5. **Iterate**
   - Fix issues
   - Repeat from step 1

### Quick Development Commands

**Build + Upload + Monitor (all-in-one):**
```bash
pio run -t upload && pio device monitor
```

**Clean + Build + Upload:**
```bash
pio run -t clean && pio run -t upload
```

### Code Organization Best Practices

#### File Structure

**Headers:**
- Place in `include/` directory
- Use `#pragma once` for include guards
- Group by functionality (Config, Type, Enum, etc.)

**Implementation:**
- Place `.cpp` files in `src/` or `lib/<LibraryName>/`
- Match header file names

**Libraries:**
- Custom reusable code in `lib/`
- Each library has own directory

#### Naming Conventions

**Current Conventions:**
- Classes: PascalCase (`EncoderDriver`, `AppEventHandler`)
- Files: Match class names (`EncoderDriver.h`, `EncoderDriver.cpp`)
- Constants: UPPER_SNAKE_CASE (`ENCODER_PIN_A`, `SCREEN_WIDTH`)
- Functions: camelCase (`handleRotate`, `setOnValueChange`)
- Enums: Scoped enums in namespace (`EventEnum::EncoderInputEventTypes`)

#### Code Style

**Indentation:** 4 spaces (consistent throughout project)

**Braces:** Opening brace on same line for functions

**Comments:** Use descriptive variable/function names, add comments for complex logic

### Adding New Features

#### Example: Add New Encoder Mode

1. **Create Mode Handler:**
   ```cpp
   // src/EncoderMode/Handler/EncoderModeHandlerCustom.h
   #pragma once
   #include "EncoderModeHandlerAbstract.h"

   class EncoderModeHandlerCustom : public EncoderModeHandlerAbstract {
   public:
       EncoderModeHandlerCustom(AppEventDispatcher* dispatcher);
       void handleRotate(int delta) override;
       void handleShortClick() override;
       const char* getModeName() const override;
   };
   ```

2. **Implement Handler:**
   ```cpp
   // src/EncoderMode/Handler/EncoderModeHandlerCustom.cpp
   #include "EncoderModeHandlerCustom.h"

   EncoderModeHandlerCustom::EncoderModeHandlerCustom(AppEventDispatcher* dispatcher)
       : EncoderModeHandlerAbstract(dispatcher) {}

   void EncoderModeHandlerCustom::handleRotate(int delta) {
       // Custom rotation logic
   }

   void EncoderModeHandlerCustom::handleShortClick() {
       // Custom click logic
   }

   const char* EncoderModeHandlerCustom::getModeName() const {
       return "Custom Mode";
   }
   ```

3. **Add Mode to Enum:**
   ```cpp
   // include/Enum/EventEnum.h
   enum class EncoderModeEventTypes {
       ENCODER_MODE_SCROLL,
       ENCODER_MODE_VOLUME,
       ENCODER_MODE_CUSTOM,  // Add here
       __ENCODER_MODE_SELECTION_LIMIT,
       // ...
   };
   ```

4. **Register in main.cpp:**
   ```cpp
   static EncoderModeHandlerCustom encoderModeHandlerCustom(&appDispatcher);
   encoderModeManager.registerHandler(
       EventEnum::EncoderModeEventTypes::ENCODER_MODE_CUSTOM,
       &encoderModeHandlerCustom
   );
   ```

5. **Build and Test:**
   ```bash
   pio run -t upload && pio device monitor
   ```

## Common Development Tasks

### Changing Bluetooth Device Name

**Edit:** [`include/Config/device_config.h`](../include/Config/device_config.h:1)
```cpp
#define BLUETOOTH_DEVICE_NAME "MyNewName"
```

**Rebuild and upload:**
```bash
pio run -t upload
```

**Note:** May need to unpair and re-pair device

### Adjusting Encoder Sensitivity

**Edit:** [`include/Config/encoder_config.h`](../include/Config/encoder_config.h:5)
```cpp
#define ENCODER_STEPS 2  // Lower = more sensitive (default: 4)
```

### Changing Display Content

**Edit:** [`src/main.cpp`](../src/main.cpp:81) in `setup()`:
```cpp
display.clearDisplay();
display.setTextSize(2);           // Larger text
display.setTextColor(WHITE);
display.setCursor(10, 20);        // Adjust position
display.println("Custom Text!");
display.display();
```

### Modifying Button Timing

**Edit:** [`include/Config/device_config.h`](../include/Config/device_config.h:5)
```cpp
#define KNOB_BUTTON_SHORT_PRESS_MS 100   // Longer short press
#define KNOB_BUTTON_LONG_PRESS_MS 2000   // Longer long press
```

## Performance Optimization

### Memory Usage

**Check Memory Usage:**
```bash
pio run --verbose
```

Look for output:
```
RAM:   [=         ]  XX.X% (used XXXXX bytes from XXXXXX bytes)
Flash: [===       ]  XX.X% (used XXXXXX bytes from XXXXXXX bytes)
```

**Optimization Tips:**
- Use `const` for read-only data (stored in flash)
- Minimize global variables
- Use stack allocation for temporary objects
- Free dynamically allocated memory when done

### CPU Performance

**FreeRTOS Task Priorities:**
- Encoder handler: Default priority
- App event handler: Default priority
- Adjust if real-time requirements change

**Monitor with Stats:**
- Enable StatsMonitor library (future)
- Track task execution times
- Identify bottlenecks

### Power Optimization

**Current Consumption:**
- Active BLE: ~100-150 mA
- Deep sleep possible (not implemented)

**Future Improvements:**
- Implement light sleep between events
- Reduce BLE advertising interval
- Turn off OLED when idle

## Troubleshooting Development Issues

### Build Errors

#### Error: "No such file or directory"

**Cause:** Missing include path or file

**Solution:**
1. Verify file exists in `include/` or `lib/`
2. Check include path in `#include` statement
3. Rebuild from clean: `pio run -t clean && pio run`

#### Error: "Multiple definition of..."

**Cause:** Function/variable defined in header without `inline` or `static`

**Solution:**
1. Move implementation to `.cpp` file
2. Use `inline` for small functions in headers
3. Use `static` or anonymous namespace for file-scope variables

### Runtime Errors

#### Hard Fault / Guru Meditation

**Symptoms:** Device crashes, stack trace in serial output

**Debug:**
1. Use exception decoder filter
2. Check for null pointer dereferences
3. Verify stack size for tasks
4. Look for buffer overflows

#### Bluetooth Not Working

**Symptoms:** Device not advertising, pairing fails

**Debug:**
1. Check serial output for BLE initialization
2. Verify NimBLE library version
3. Check for conflicting WiFi usage
4. Increase BLE TX power if range is issue

## Version Control

### Git Workflow

**Initialize Repository (if not done):**
```bash
git init
git add .
git commit -m "Initial commit"
```

**Typical Workflow:**
```bash
# Create feature branch
git checkout -b feature/new-mode

# Make changes, test
# ...

# Commit changes
git add .
git commit -m "Add new encoder mode"

# Merge back to main
git checkout main
git merge feature/new-mode
```

### .gitignore

**Already Configured:**
- `.pio/` - Build artifacts
- `.vscode/` - IDE settings
- Compiled binaries

## Resources and References

### Documentation

- **PlatformIO:** <https://docs.platformio.org/>
- **ESP32-C3:** <https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/>
- **Arduino ESP32:** <https://docs.espressif.com/projects/arduino-esp32/>
- **FreeRTOS:** <https://www.freertos.org/Documentation/RTOS_book.html>

### Community

- **PlatformIO Community:** <https://community.platformio.org/>
- **ESP32 Forum:** <https://esp32.com/>
- **Arduino Forum:** <https://forum.arduino.cc/>

### Example Projects

- **ESP32-BLE-Keyboard Examples:** <https://github.com/T-vK/ESP32-BLE-Keyboard/tree/master/examples>
- **Rotary Encoder Examples:** <https://github.com/igorantolic/ai-esp32-rotary-encoder/tree/master/examples>

---

*Development guide generated on 2025-12-14 during deep scan documentation workflow.*
