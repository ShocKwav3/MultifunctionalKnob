# Testing and Debugging

## Testing Strategy

**Policy:** Testable Code over Automated Tests (MVP Phase)

We prioritize writing **testable code** (using dependency injection, interfaces, and modular design) but **do not require automated unit tests** (e.g., Google Test, Unity) at this stage.

- **Testable Code:** Components should be designed to be testable in the future. Use interfaces (e.g., `DisplayInterface`) and dependency injection to allow mocking. *Specifically, mode handlers (e.g., `EncoderModeHandlerScroll`, `EncoderModeHandlerZoom`) must accept `BleKeyboard*` via constructor dependency injection to ensure testability of BLE interactions.*
- **Manual Testing:** Primary validation is done via:
  - **Serial Monitor:** Verifying log output and state changes.
  - **Hardware Testing:** Verifying physical interaction (encoder, buttons) and BLE behavior.
- **Future Proofing:** When the project matures, we can introduce a unit testing framework and backfill tests using the testable seams we've created.

## Serial Monitor

### Open Monitor

```bash
pio device monitor --baud 460800
```

**Monitor with filters:**
```bash
pio device monitor --baud 460800 --filter esp32_exception_decoder
```

**Exit:** `Ctrl+C` or `Ctrl+]`

### Serial Output

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

## Hardware Testing

### Test Encoder Input

1. Upload firmware with serial debug enabled
2. Open serial monitor
3. Rotate encoder - observe value changes
4. Press button - observe click events
5. Long press button - observe long click events

### Test OLED Display

1. Upload firmware
2. Verify "Knob firmware!" appears on display
3. Check display brightness and contrast
4. Test different display content

### Test Bluetooth HID

1. Pair device with computer/phone
2. Open document or web page
3. Rotate encoder - observe scroll behavior
4. Short click - observe mode indication
5. Long click - test mode selection

## Debugging Tools

### ESP32 Exception Decoder

**Install filter:**
```bash
pio pkg install --tool platformio/tool-pvvx-ble-monitor
```

**Use in monitor:**
```bash
pio device monitor --filter esp32_exception_decoder
```

**Decodes stack traces from crashes**

### GDB Debugging (Advanced)

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

### Logic Analyzer

**Useful for:**
- I2C bus analysis (OLED communication)
- Encoder signal decoding
- Timing verification

**Recommended Tools:**
- Saleae Logic Analyzer
- PulseView + cheap USB logic analyzer
- Oscilloscope with protocol decoding
