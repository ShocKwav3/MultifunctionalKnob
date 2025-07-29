#pragma once

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "Type/AppEvent.h"
#include "Enum/EventEnum.h"
#include "Helper/EnumToStringHelper.h"

class AppEventDispatcher {
public:
    AppEventDispatcher(QueueHandle_t queue);

    void dispatchAppEvent(EventEnum::EncoderModeAppEventTypes type); //should also take another type in future when a new setting (like Encoder Mode) is added

private:
    QueueHandle_t appEventQueue;
};
