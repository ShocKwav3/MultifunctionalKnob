#pragma once

#include <cstdint>

enum class ButtonAction : uint8_t {
    NONE,
    MUTE,
    PLAY,
    PAUSE,
    NEXT,
    PREVIOUS
};

constexpr uint8_t ButtonAction_MAX = static_cast<uint8_t>(ButtonAction::PREVIOUS);

inline const char* buttonActionToString(ButtonAction a) {
    switch (a) {
        case ButtonAction::NONE:     return "NONE";
        case ButtonAction::MUTE:     return "MUTE";
        case ButtonAction::PLAY:     return "PLAY";
        case ButtonAction::PAUSE:    return "PAUSE";
        case ButtonAction::NEXT:     return "NEXT";
        case ButtonAction::PREVIOUS: return "PREVIOUS";
        default:                     return "UNKNOWN";
    }
}

inline const char* buttonActionToDisplayString(ButtonAction a) {
    switch (a) {
        case ButtonAction::NONE:     return "None";
        case ButtonAction::MUTE:     return "Mute";
        case ButtonAction::PLAY:     return "Play";
        case ButtonAction::PAUSE:    return "Pause";
        case ButtonAction::NEXT:     return "Next";
        case ButtonAction::PREVIOUS: return "Previous";
        default:                     return "Unknown";
    }
}
