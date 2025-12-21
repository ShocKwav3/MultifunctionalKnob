#pragma once

#include "Enum/EventEnum.h"
#include "Enum/WheelModeEnum.h"

class EncoderModeHelper {
public:
    static const char* toString(EventEnum::EncoderModeEventTypes type);

    /**
     * @brief Convert WheelMode to EncoderModeEventTypes
     * @param mode The WheelMode to convert
     * @return Corresponding EncoderModeEventTypes value
     */
    static EventEnum::EncoderModeEventTypes fromWheelMode(WheelMode mode);
};
