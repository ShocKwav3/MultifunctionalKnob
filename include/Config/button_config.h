#pragma once

#include <cstdint>
#include <cstddef>

struct ButtonConfig {
    uint8_t pin;
    const char* label;
    bool activeLow;
};

inline constexpr ButtonConfig BUTTONS[] = {
    {3, "Button 1", true},
    {4, "Button 2", true},
    {5, "Button 3", true},
    {9, "Button 4", true}  // GPIO 8 is strapping pin, using GPIO 9
};

inline constexpr size_t BUTTON_COUNT = sizeof(BUTTONS) / sizeof(BUTTONS[0]);
inline constexpr uint32_t DEBOUNCE_MS = 50;
