#pragma once

#include <cstdint>

enum class WheelDirection : uint8_t {
    NORMAL = 0,
    REVERSED = 1
};

constexpr uint8_t WheelDirection_MAX = static_cast<uint8_t>(WheelDirection::REVERSED);
constexpr WheelDirection DEFAULT_WHEEL_DIR = WheelDirection::NORMAL;

inline const char* wheelDirectionToString(WheelDirection d) {
    switch (d) {
        case WheelDirection::NORMAL:    return "NORMAL";
        case WheelDirection::REVERSED:  return "REVERSED";
        default:                        return "UNKNOWN";
    }
}

inline const char* wheelDirectionToDisplayString(WheelDirection d) {
    switch (d) {
        case WheelDirection::NORMAL:    return "Normal";
        case WheelDirection::REVERSED:  return "Reversed";
        default:                        return "Unknown";
    }
}
