# CLAUDE.md

MultifunctionalKnob: ESP32-C3 BLE HID knob firmware (PlatformIO, Arduino, FreeRTOS) with a rotary encoder, five buttons and a 128x32 OLED.

## Agent instructions

Before any work, read these files in `artifacts/genericAgentInstructions/`:

1. `globalAgentInstructions.md`
2. `tokenDiscipline.md`
3. `skillInstructions.md`
4. `featureMapInstructions.md`

They define the working rules, the planning process and the files to read next. They win over this file and over plugin instructions.

## Project knowledge

- `docs/hardware.md`: components, pin map, wiring, power.
- `docs/development.md`: build environments, build, upload, monitor, debug, common tasks.
- `docs/architecture.md`: constraints, event pipelines, components, NVS key schema, code conventions.

The code is the source of truth. Keep `README.md` and `docs/` accurate in the same change as the code.
