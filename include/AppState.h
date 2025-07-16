#pragma once

#include "atomic"

struct AppState {
    QueueHandle_t encoderInputEventQueue = nullptr;
    QueueHandle_t appEventQueue = nullptr;
};

extern AppState appState;
