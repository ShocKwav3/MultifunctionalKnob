# Development

## Prerequisites

- VS Code with the PlatformIO IDE extension. The repository recommends it in `.vscode/extensions.json`.
- A USB-C cable that transfers data.
- The hardware, wired as in [hardware.md](hardware.md).

## Setup

1. Install the **PlatformIO IDE** extension in VS Code. It installs PlatformIO Core (the `pio` CLI) into `~/.platformio/penv/bin/`.
2. Open the repository folder in VS Code. In the PlatformIO IDE Terminal, `pio` works without more setup.
3. To use `pio` in other terminals, add its folder to the end of `PATH`:

   ```bash
   echo 'export PATH="$PATH:$HOME/.platformio/penv/bin"' >> ~/.zshrc
   source ~/.zshrc
   pio --version
   ```

   The folder also contains the `python` and `pip` of the PlatformIO environment. Keep it at the end of `PATH`, so that your system Python stays first.

Install only one PlatformIO Core. A second core, for example from `brew install platformio`, conflicts with the core of the extension.

The first build downloads the `espressif32` platform, the toolchain, the Arduino framework and the libraries in `platformio.ini`. It takes several minutes.

Reference: [Install Shell Commands](https://docs.platformio.org/en/latest/core/installation/shell-commands.html).

## Build environments

| Environment | BLE stack | Display | Use |
| --- | --- | --- | --- |
| `use_nimble` | NimBLE | OLED | Default build |
| `use_stdble` | Standard ESP32 BLE | OLED | Alternative stack |
| `use_nimble_debug` | NimBLE | Serial | Debug build, JTAG |
| `use_stdble_debug` | Standard ESP32 BLE | Serial | Debug build, JTAG |

- `USE_OLED_DISPLAY` selects the OLED. Without it, the firmware writes the display output to the serial port.
- `LOG_LEVEL` sets the log level: 0 none, 1 error, 2 info (default), 3 debug. Add `-D LOG_LEVEL=3` to `build_flags` for debug logs.

## Build, upload and monitor

```bash
pio run -e use_nimble
pio run -e use_nimble -t upload
pio device monitor
```

- The serial monitor uses 460800 baud (`monitor_speed`).
- To decode a crash backtrace, run `pio device monitor --filter esp32_exception_decoder`.
- To force a full rebuild, run `pio run -t clean` first.
- To select a port, add `--upload-port <port>` to the upload command.

## Debug

The debug environments use the built-in USB JTAG of the ESP32-C3 (`debug_tool = esp-builtin`).

```bash
pio debug -e use_nimble_debug
```

## Factory reset

1. Disconnect the power.
2. Hold the encoder button.
3. Connect the power, and hold the button for 5 s or more.

All settings return to their defaults. A wake from deep sleep never starts a factory reset.

## Common tasks

### Change the device name

1. Edit `DEVICE_NAME` in `include/Config/device_config.h`.
2. Change `NVS_NAMESPACE` to the same name in lowercase, with a maximum of 15 characters.
3. Remove the old pairing on the host, and pair again.

Warning: a new `NVS_NAMESPACE` does not read the old settings. The device starts with defaults.

### Change the encoder sensitivity

Edit `ENCODER_STEPS` in `include/Config/encoder_config.h`. A lower value gives more events for each detent.

### Change the press timings

- Encoder button: `ENCODER_SHORT_PRESS_MIN_MS` and `ENCODER_LONG_PRESS_MIN_MS` in `include/Config/encoder_config.h`.
- Buttons: `BUTTON_SHORT_PRESS_MIN_MS`, `BUTTON_LONG_PRESS_MIN_MS` and `DEBOUNCE_MS` in `include/Config/button_config.h`.

### Change the sleep timing

Edit `POWER_WARNING_THRESHOLD_MS` and `POWER_SLEEP_THRESHOLD_MS` in `include/Config/system_config.h`.

### Add a media key action

Add one row to `MEDIA_KEY_ACTIONS` in `src/BLE/BleKeyboardService.cpp`, and increase `BUTTON_ACTION_COUNT` in `src/BLE/BleKeyboardService.h`. A `static_assert` stops the build if the two do not match. The menu shows the new action automatically.

### Add a wheel mode

1. Create a handler in `src/EncoderMode/Handler/` that extends `EncoderModeHandlerAbstract`.
2. Add the mode to `EventEnum::EncoderModeEventTypes` in `include/Enum/EventEnum.h`, before `__ENCODER_MODE_SELECTION_LIMIT`.
3. Add the mode to `WheelMode` in `include/Enum/WheelModeEnum.h`, and update `WheelMode_MAX`.
4. Update the conversions in `src/Helper/EncoderModeHelper.cpp`.
5. Register the handler in `src/main.cpp` with `encoderModeManager.registerHandler()`.
6. Add the mode to `wheelModeSubmenu` in `src/Menu/Model/MenuTree.h`, and update `WHEEL_MODE_COUNT`.

### Add a menu item

1. Add the item to the correct array in `src/Menu/Model/MenuTree.h`.
2. Create a `MenuAction` subclass in `src/Menu/Action/`.
3. Connect the action in the matching `MenuTree::init*Actions()` function, called from `src/main.cpp`.

A menu screen holds a maximum of `DISPLAY_MAX_MENU_ITEMS` (8) items (`src/Display/Model/DisplayRequest.h`).

## Troubleshooting

| Symptom | Check |
| --- | --- |
| `No such file or directory` at build | The include path. Headers under `include/` are included from the folder below it, for example `"Config/device_config.h"`. |
| `multiple definition of` at link | A function or variable defined in a header without `inline` or `constexpr`. |
| Guru Meditation or reset loop | Decode the backtrace with the exception decoder. Look for null pointers, buffer overflows and task stack size. |
| Upload fails | The cable, the port, and the BOOT button (see [hardware.md](hardware.md)). |
| Permission denied on Linux | Add the user to the `dialout` group, then log in again. |
