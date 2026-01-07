#pragma once

#include <cstdint>

enum class WheelMode : uint8_t {
    SCROLL,
    VOLUME,
    ZOOM
};

constexpr uint8_t WheelMode_MAX = static_cast<uint8_t>(WheelMode::ZOOM);

inline const char* wheelModeToString(WheelMode m) {
    switch (m) {
        case WheelMode::SCROLL: return "SCROLL";
        case WheelMode::VOLUME: return "VOLUME";
        case WheelMode::ZOOM:   return "ZOOM";
        default:                return "UNKNOWN";
    }
}

inline const char* wheelModeToDisplayString(WheelMode m) {
    switch (m) {
        case WheelMode::SCROLL: return "Scroll";
        case WheelMode::VOLUME: return "Volume";
        case WheelMode::ZOOM:   return "Zoom";
        default:                return "Unknown";
    }
}
