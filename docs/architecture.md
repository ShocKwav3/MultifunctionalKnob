# Architecture

The firmware is event-driven on FreeRTOS, with static allocation. The pin map is in [hardware.md](hardware.md).

## Constraints

- Single core at 160 MHz and 400 KB SRAM. Keep RAM use below 300 KB.
- No dynamic allocation at runtime: no `new`, no `malloc`, no Arduino `String`. Long-lived objects are `static` in `setup()`.
- Do not access hardware in static constructors. Initialize hardware in `setup()`.
- Encoder input responds in less than 50 ms. A HID report goes out less than 20 ms after the input.
- Each abstraction costs RAM, flash or CPU: a vtable, an extra object or an extra task. Prefer compile-time selection and plain functions when they are sufficient.

## Event pipelines

Input never calls a handler directly. Each pipeline is: driver callback, then dispatcher, then FreeRTOS queue (10 items), then handler task.

```text
EncoderDriver  → EncoderEventDispatcher → encoderInputEventQueue → EncoderEventHandler → EncoderModeManager → mode handler → BLE
ButtonDriver   → ButtonEventDispatcher  → buttonEventQueue       → ButtonEventHandler  → BleKeyboardService / MacroManager → BLE
Mode handlers  → AppEventDispatcher     → appEventQueue          → AppEventHandler
MenuController → MenuEventDispatcher    → menuEventQueue         → MenuEventHandler
Any component  → displayRequestQueue    → DisplayTask            → DisplayInterface (OLED or serial)
```

- `EncoderEventDispatcher` applies the wheel direction before the event enters the queue.
- `EncoderEventHandler` routes each event in this order: the menu (if open), macro mode (if on and the slot has a macro), then the active mode handler.
- `ButtonEventHandler` routes each event in this order: a long press on the macro button toggles macro mode, then macro mode, then the media key of the button. It does not check the menu.
- `EncoderEventHandler` and `ButtonEventHandler` reset the inactivity timer of `PowerManager` for each event.
- Only `DisplayTask` touches the display. Other components send a `DisplayRequest`.

## Tasks and timers

| Task or timer | Role |
| --- | --- |
| `EncoderDriverTask`, `ButtonDriver` task | Input detection and press timing |
| `EncoderEventTask`, `ButtonEventTask` | Input routing |
| `AppEventHandlerTask`, `MenuEventHandler` | Mode and menu events |
| `DisplayTask` | The only display writer |
| `PowerManager` task | Checks inactivity every 1 s. Warns at `POWER_WARNING_THRESHOLD_MS`, and enters deep sleep at `POWER_SLEEP_THRESHOLD_MS` with GPIO 2 low as the wake source. |
| `BTFlash` timer | Flashes the Bluetooth icon at 1 Hz during pairing |

## Components

| Component | Responsibility |
| --- | --- |
| `ConfigManager` | All NVS access. Validates stored enums against their `_MAX` value and falls back to the default. Returns `Error`. |
| `BleKeyboardService` | The single table of media key actions: IDs, labels, confirmation texts and the HID write. |
| `MacroManager` | Macro mode on or off, the seven macros, and the macro send. |
| `EncoderModeManager` | Holds the mode handlers (Scroll, Volume, Zoom) and routes to the active one. |
| `MenuController`, `MenuTree` | A static menu tree with parent pointers. Each leaf holds a `MenuAction` (command pattern). |
| `DisplayInterface` | Implemented by `OLEDDisplay` and `SerialDisplay`. `DisplayFactory` selects one with `USE_OLED_DISPLAY`. |
| `PowerManager` | Inactivity timer, sleep warning and deep sleep. |
| `FactoryReset` | Clears the NVS namespace if the encoder button is held for 5 s at boot. Skipped after a wake from deep sleep. |
| `HardwareState` | Global state: wheel mode and direction, battery level, BLE state, display power, macro mode. |

## Records

| Record | Meaning |
| --- | --- |
| `EncoderInputEvent` | `ROTATE` with a signed `delta`, `SHORT_CLICK` or `LONG_CLICK` |
| `ButtonEvent` | `SHORT_PRESS` or `LONG_PRESS`, with a `buttonIndex` into `BUTTONS[]` |
| `MenuEvent` | Activated, deactivated, navigation changed or item selected, with the current item, the selected index and the item count |
| `DisplayRequest` | A typed union: `DRAW_MENU`, `SHOW_STATUS`, `SHOW_MESSAGE`, `CLEAR`, `CLEAR_WARNING`, `DRAW_NORMAL_MODE`. A menu holds a maximum of `DISPLAY_MAX_MENU_ITEMS` (8) labels. |
| `MacroDefinition` | `modifiers` (HID bitmask) and `keycode`. Packed as `(modifiers << 8) \| keycode`. `0x0000` means no macro. |
| `MacroInput` | `WHEEL_BUTTON` = 0, `WHEEL_LEFT` = 1, `WHEEL_RIGHT` = 2, `BUTTON_1` to `BUTTON_4` = 3 to 6. The value is also the NVS slot. |
| `Error` | `OK`, `INVALID_PARAM`, `NVS_READ_FAIL`, `NVS_WRITE_FAIL`, `INVALID_STATE` |

## NVS key schema

Namespace: `knobkoky` (`NVS_NAMESPACE` in `include/Config/device_config.h`).

| Key | Type | Meaning | Default |
| --- | --- | --- | --- |
| `wheel.mode` | `uint8_t` | `WheelMode`: 0 Scroll, 1 Volume, 2 Zoom | Scroll |
| `wheel.dir` | `uint8_t` | `WheelDirection`: 0 Normal, 1 Reversed | Normal |
| `btn0.action` to `btn3.action` | `uint8_t` | Media key action ID from `BleKeyboardService` | None |
| `macro.0` to `macro.6` | `uint16_t` | Packed `MacroDefinition` | `0x0000` |

- An NVS write takes about 20 ms. Write only after a user action, never in a loop.
- Check the return value of each write. A full NVS partition makes a write fail.
- The display power is not saved. The display is always on after boot.

## Code conventions

| Element | Convention | Example |
| --- | --- | --- |
| Files, classes, structs | PascalCase, file name matches the class | `MenuController.cpp` |
| Methods, variables | camelCase | `handleEvent()`, `currentItem` |
| Constants, macros | UPPER_SNAKE_CASE | `BUTTON_COUNT`, `LOG_INFO` |
| Enums | `enum class`, UPPER_SNAKE values | `enum class Error { OK, NVS_WRITE_FAIL }` |
| Interfaces | Suffix `Interface` | `DisplayInterface` |

- Use `#pragma once` in every header.
- Use `constexpr` for constants. Use `#define` only for conditional compilation.
- Use `explicit` on single-argument constructors. Give each base class with virtual methods a virtual destructor.
- File placement:
  - config constants: `include/Config/<domain>_config.h`;
  - enums: `include/Enum/`;
  - event structs: `include/Type/`;
  - domain code: `src/<Domain>/{Handler,Interface,Impl,Model,Action,Manager}/`.
- Include order: the matching header, the C library, Arduino and ESP32 headers, third-party libraries, then project headers in alphabetical order.
- Log with `LOG_ERROR`, `LOG_INFO` and `LOG_DEBUG` from `include/Config/log_config.h`. Do not call `Serial.print` directly.
- Return `Error` from every fallible operation.
- Zero-initialize event structs. Read only the union member that matches the type.
- Send to a queue with a timeout and check for `pdPASS`. Do not block or call `delay()` in a handler.
- Check `isConnected()` before every HID write.
- Do not add debounce logic in handlers. The drivers debounce the inputs.
