#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * @brief Application-level state
 *
 * Contains software/application state (FreeRTOS queues, event handlers, etc.).
 * Hardware state is now tracked separately in the global hardwareState instance.
 */
struct AppState {
    QueueHandle_t encoderInputEventQueue = nullptr;
    QueueHandle_t buttonEventQueue = nullptr;
    QueueHandle_t appEventQueue = nullptr;
    QueueHandle_t menuEventQueue = nullptr;
    QueueHandle_t displayRequestQueue = nullptr;
};

extern AppState appState;
