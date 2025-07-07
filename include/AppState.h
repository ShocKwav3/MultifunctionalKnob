#pragma once

#include "atomic"

struct AppState {
    std::atomic<int32_t> encoderValue{0};
    std::atomic<bool> buttonShortPressed{false};
    std::atomic<bool> buttonLongPressed{false};
};

extern AppState appState;
