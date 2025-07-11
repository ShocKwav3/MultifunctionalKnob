#pragma once

#include "atomic"

struct AppState {
    QueueHandle_t rotaryEventQueue = nullptr;
};

extern AppState appState;
