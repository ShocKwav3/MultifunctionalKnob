#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Type/BlePairingState.h"

struct AppState {
    QueueHandle_t encoderInputEventQueue = nullptr;
    QueueHandle_t buttonEventQueue = nullptr;
    QueueHandle_t appEventQueue = nullptr;
    QueueHandle_t menuEventQueue = nullptr;
    QueueHandle_t displayRequestQueue = nullptr;

    BlePairingState blePairingState;
};

extern AppState appState;
