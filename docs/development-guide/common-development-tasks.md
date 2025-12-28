# Common Development Tasks

## Changing Bluetooth Device Name

**Edit:** [`include/Config/device_config.h`](../include/Config/device_config.h:1)
```cpp
#define BLUETOOTH_DEVICE_NAME "MyNewName"
```

**Rebuild and upload:**
```bash
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t upload
```

**Note:** May need to unpair and re-pair device

## Adjusting Encoder Sensitivity

**Edit:** [`include/Config/encoder_config.h`](../include/Config/encoder_config.h:5)
```cpp
#define ENCODER_STEPS 2  // Lower = more sensitive (default: 4)
```

## Changing Display Content

**Edit:** [`src/main.cpp`](../src/main.cpp:81) in `setup()`:
```cpp
display.clearDisplay();
display.setTextSize(2);           // Larger text
display.setTextColor(WHITE);
display.setCursor(10, 20);        // Adjust position
display.println("Custom Text!");
display.display();
```

## Modifying Button Timing

**Edit:** [`include/Config/device_config.h`](../include/Config/device_config.h:5)
```cpp
#define KNOB_BUTTON_SHORT_PRESS_MS 100   // Longer short press
#define KNOB_BUTTON_LONG_PRESS_MS 2000   // Longer long press
```
