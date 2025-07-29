#pragma once

#include "Enum/EventEnum.h"

class EnumToStringHelper {
public:
    static const char* toString(EventEnum::EncoderInputEventTypes type);
    static const char* toString(EventEnum::EncoderModeAppEventTypes type);
};
