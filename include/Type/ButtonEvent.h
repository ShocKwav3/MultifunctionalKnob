#pragma once

#include <cstdint>
#include "Enum/EventEnum.h"

struct ButtonEvent {
    EventEnum::ButtonEventTypes type;
    uint8_t buttonIndex = 0;
};
