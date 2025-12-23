#pragma once

struct AppState {
    QueueHandle_t encoderInputEventQueue = nullptr;
    QueueHandle_t buttonEventQueue = nullptr;
    QueueHandle_t appEventQueue = nullptr;
};

extern AppState appState;
