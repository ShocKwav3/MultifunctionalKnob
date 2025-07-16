#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "Type/AppEvent.h"
#include "Enum/ModeEnum.h"

class AppEventDispatcher {
public:
    AppEventDispatcher(QueueHandle_t queue);

    void dispatchAppEvent(ModeEnum::AppEventTypes type);

private:
    QueueHandle_t appEventQueue;
};
