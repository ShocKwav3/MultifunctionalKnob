# Common Development Tasks

## Changing Bluetooth Device Name

**Edit:** `include/Config/device_config.h`
```cpp
// Device Identity - Single Source of Truth
constexpr const char* DEVICE_NAME = "MyNewName";
constexpr const char* NVS_NAMESPACE = "mynewname";  // lowercase, derived from DEVICE_NAME
```

**Rebuild and upload:**
```bash
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble -t upload
```

**Note:** May need to unpair and re-pair device. The NVS namespace should be lowercase and match the device name to avoid confusion.

## Adjusting Encoder Sensitivity

**Edit:** `include/Config/encoder_config.h`
```cpp
#define ENCODER_STEPS 2  // Lower = more sensitive (default: 4)
```

## Changing Display Content

The display system uses a DisplayInterface abstraction. To change status screen content:

**Normal Mode Display:**
- Edit `src/Display/Impl/OLEDDisplay.cpp` - `showStatusScreen()` method

**Menu Display:**
- Edit `src/Display/Impl/OLEDDisplay.cpp` - `showMenu()` and `showMenuHeader()` methods

**Status Messages:**
- Use DisplayRequest queue to send messages:
```cpp
DisplayRequest request;
request.type = DisplayRequestType::SHOW_MENU;
request.menuText = "Custom Text";
xQueueSend(displayRequestQueue, &request, portMAX_DELAY);
```

## Modifying Button Timing

**Edit:** `include/Config/device_config.h`
```cpp
#define KNOB_BUTTON_SHORT_PRESS_MS 100   // Longer short press (default: 50)
#define KNOB_BUTTON_LONG_PRESS_MS 2000   // Longer long press (default: 1000)
```

## Changing Button Actions

Button actions are configured via the menu system. To change default actions programmatically:

**Edit:** `src/Config/ConfigManager.cpp` in `resetToDefaults()` method to change factory defaults.

**Runtime:** Users can configure button actions via Menu > Button Config > [Button Name] > [Action].

## Adding a New Menu Item

1. Add the menu item to `src/Menu/Model/MenuTree.h`:
   ```cpp
   inline MenuItem mainMenu[] = {
       // ... existing items ...
       { "New Item", nullptr, nullptr, 0, nullptr },
   };
   ```

2. Create an action class in `src/Menu/Action/` (see existing actions for pattern)

3. Initialize the action in `MenuTree::init*Actions()` functions

## Adjusting Power Management Timers

**Edit:** `include/Config/system_config.h`
```cpp
constexpr uint32_t INACTIVITY_WARNING_MS = 240000;  // 4 minutes (default)
constexpr uint32_t INACTIVITY_SLEEP_MS = 300000;    // 5 minutes (default)
```

---

*Last Updated: 2026-01-22*
