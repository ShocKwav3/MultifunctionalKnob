#pragma once

// Device Identity - Single Source of Truth
constexpr const char* DEVICE_NAME = "KnobKoKy";
constexpr const char* NVS_NAMESPACE = "knobkoky";  // lowercase, derived from DEVICE_NAME

// Bluetooth Configuration
#define BLUETOOTH_DEVICE_NAME DEVICE_NAME
#define BLUETOOTH_DEVICE_MANUFACTURER "KoKy"
#define BLUETOOTH_DEVICE_BATTERY_LEVEL_DEFAULT 100

#define KNOB_BUTTON_SHORT_PRESS_MS 50
#define KNOB_BUTTON_LONG_PRESS_MS 1000

// Wheel Configuration
constexpr const char* KEY_WHEEL_MODE = "wheel.mode";
constexpr const char* KEY_WHEEL_DIR = "wheel.dir";
