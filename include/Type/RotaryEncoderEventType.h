#pragma once

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Enum/EventEnum.h"

struct RotaryEncoderEventType {
    EventEnum::RotaryEncoderEventTypes type;

    int32_t delta = 0;
};
