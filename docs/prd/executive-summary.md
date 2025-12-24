# Executive Summary

UtilityButtonsWithKnobUSB is an ESP32-C3 based Bluetooth HID peripheral device that currently functions as a single-purpose rotary encoder (scroll/volume control). This PRD defines the evolution from a limited single-function device into a fully configurable multi-purpose input peripheral.

The core addition is a hierarchical menu system that allows users to configure wheel behavior and button actions at runtime, displayed via serial output initially with hardware display support planned. Four additional GPIO-connected buttons will be added alongside the existing encoder button, each configurable to perform any action supported by the ESP32-BLE-Keyboard library.

**Target Users:** Makers, developers, and enthusiasts who want a customizable desktop peripheral. The project will be open-sourced for community contribution.

## What Makes This Special

This isn't just adding features to a scroll wheel - it's transforming a single-purpose device into a user-configurable multi-function peripheral. The key differentiators:

1. **Runtime Configuration** - Users configure behavior through the device itself, not by reflashing firmware
2. **Display-Agnostic Architecture** - Works via serial output now, seamlessly transitions to hardware display later
3. **Config-Driven Flexibility** - Button count and behaviors driven by configuration, not hardcoded
4. **Open-Source Ready** - Designed to be shared and extended by the community
