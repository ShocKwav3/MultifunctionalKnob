#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

/**
 * @brief Application-level state
 *
 * Contains software/application state (FreeRTOS queues, timers, event handlers, etc.).
 * Hardware state is now tracked separately in the global hardwareState instance.
 */
struct AppState {
    QueueHandle_t encoderInputEventQueue = nullptr;
    QueueHandle_t buttonEventQueue = nullptr;
    QueueHandle_t appEventQueue = nullptr;
    QueueHandle_t menuEventQueue = nullptr;
    QueueHandle_t displayRequestQueue = nullptr;
    TimerHandle_t btFlashTimer = nullptr;  ///< BT icon flash animation timer
};

extern AppState appState;
