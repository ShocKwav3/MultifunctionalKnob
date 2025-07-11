#pragma once

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Enum/EventEnum.h"

struct RotaryEncoderEvent {
    EventEnum::RotaryEncoderEventType type;

    int32_t delta = 0;
};
