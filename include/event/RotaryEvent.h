#pragma once

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "enums/EventEnum.h"


struct RotaryEvent {
    EventEnum::RotaryEventType type;

    int32_t delta = 0;
};
