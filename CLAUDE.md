# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32-C3 Bluetooth HID device (BLE keyboard) with rotary encoder and OLED display. Sends keystrokes/media controls to paired devices via BLE. Built with PlatformIO and Arduino framework.

## Build Commands

**Important:** Use the `pio-wrapper` skill to run PlatformIO commands. This reduces token usage by filtering verbose build output and returning only error lines on failures.

See `.claude/skills/pio-wrapper/SKILL.md` for full details.

Command examples (using the wrapper):

```bash
# Build (default NimBLE env)
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble

# Build + upload + monitor
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble -t upload && pio device monitor

# Clean build
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -t clean

# Debug build
./.claude/skills/pio-wrapper/scripts/pio-wrapper.py run -e use_nimble_debug
```

Monitor baud rate: 460800

## Build Environments

- `use_nimble` - NimBLE stack (recommended, lower memory)
- `use_stdble` - Standard ESP32 BLE stack
- `use_nimble_debug` / `use_stdble_debug` - Debug variants
- `debug` - Base debug config

## Architecture

**Event-driven system using FreeRTOS queues:**

```
Hardware Input → Dispatcher → Queue → Handler (FreeRTOS Task) → Mode Handler → BLE Output
```

Three event systems:
- **Encoder events**: `EncoderEventDispatcher` → `encoderInputEventQueue` → `EncoderEventHandler`
- **Button events**: `ButtonEventDispatcher` → `buttonEventQueue` → `ButtonEventHandler`
- **App events**: `AppEventDispatcher` → `appEventQueue` → `AppEventHandler`

**Mode Handler Pattern**: `EncoderModeManager` routes encoder events to active mode handler (Scroll/Volume/Zoom). Handlers extend `EncoderModeHandlerAbstract`.

**Key initialization flow** (`main.cpp`):
1. Factory reset check (hold encoder button 5+ sec at boot)
2. I2C/Display/BLE init
3. Create FreeRTOS queues
4. Register mode handlers with `EncoderModeManager`
5. Load saved mode from NVS via `ConfigManager`
6. Start event handler tasks
7. Initialize button manager and encoder driver

## Hardware Configuration

| Component | GPIO |
|-----------|------|
| Encoder CLK | 1 |
| Encoder DT | 0 |
| Encoder Button | 2 |
| Buttons 1-4 | 3, 4, 5, 9 |
| OLED SDA/SCL | 6, 7 |

Board: `nologo_esp32c3_super_mini`

## Key Source Locations

- `src/EncoderMode/Handler/` - Mode handlers (Scroll, Volume, Zoom)
- `src/Event/Dispatcher/` - Event dispatchers for encoder, button, app events
- `src/Event/Handler/` - FreeRTOS task-based event handlers
- `src/Menu/` - Menu system (MenuController, MenuTree)
- `lib/EncoderDriver/` - Custom encoder driver with ISR callbacks
- `include/Config/` - Pin definitions, BLE config, button config
- `include/Enum/EventEnum.h` - All event type enums
- `include/Type/` - Event structs (EncoderInputEvent, ButtonEvent, AppEvent)

## Adding New Encoder Mode

1. Create handler in `src/EncoderMode/Handler/` extending `EncoderModeHandlerAbstract`
2. Add enum to `EventEnum::EncoderModeEventTypes` in `include/Enum/EventEnum.h`
3. Register in `main.cpp` with `encoderModeManager.registerHandler()`
4. Add to `WheelModeEnum.h` if persisting to NVS
5. Update `EncoderModeHelper` for enum conversion

## Persistence

`ConfigManager` wraps ESP32 Preferences (NVS) for persistent storage. Currently stores wheel mode selection.
