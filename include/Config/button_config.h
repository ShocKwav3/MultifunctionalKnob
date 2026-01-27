#pragma once

#include <cstdint>
#include <cstddef>

struct ButtonConfig {
    uint8_t pin;
    const char* label;
    bool activeLow;
};

inline constexpr ButtonConfig BUTTONS[] = {
    {3, "Top Left", true},
    {4, "Top Right", true},
    {10, "Bottom Left", true},
    {20, "Bottom Right", true},
    {5, "Macro", true}            // GPIO 5 - Macro toggle button
};

inline constexpr size_t BUTTON_COUNT = sizeof(BUTTONS) / sizeof(BUTTONS[0]);
inline constexpr uint32_t DEBOUNCE_MS = 50;

// Button press timing thresholds
inline constexpr uint32_t BUTTON_SHORT_PRESS_MIN_MS = 50;   // Minimum for valid short press
inline constexpr uint32_t BUTTON_LONG_PRESS_MIN_MS = 1000;  // Threshold for long press

// Macro button index in BUTTONS[] array
inline constexpr uint8_t MACRO_BUTTON_INDEX = 4;
