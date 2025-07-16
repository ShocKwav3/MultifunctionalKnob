#pragma once

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Enum/ModeEnum.h"

struct AppEvent {
    ModeEnum::AppEventTypes type;
};
