#pragma once

#include <cstdint>

enum class PowerState : uint8_t {
    ACTIVE = 0,
    WARNING = 1,
    SLEEP = 2
};

constexpr uint8_t PowerState_MAX = static_cast<uint8_t>(PowerState::SLEEP);
constexpr PowerState DEFAULT_POWER_STATE = PowerState::ACTIVE;

inline const char* powerStateToString(PowerState state) {
    switch (state) {
        case PowerState::ACTIVE:    return "ACTIVE";
        case PowerState::WARNING:   return "WARNING";
        case PowerState::SLEEP:     return "SLEEP";
        default:                    return "UNKNOWN";
    }
}
