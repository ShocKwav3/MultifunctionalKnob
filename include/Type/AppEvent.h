#pragma once

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Enum/EventEnum.h"

struct AppEvent {
    EventEnum::EncoderModeEventTypes type;
};
