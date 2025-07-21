#pragma once

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "Type/AppEvent.h"
#include "Enum/EventEnum.h"
#include "Helper/ModeHelper.h"

class AppEventDispatcher {
public:
    AppEventDispatcher(QueueHandle_t queue);

    void dispatchAppEvent(EventEnum::AppEventTypes type);

private:
    QueueHandle_t appEventQueue;
};
