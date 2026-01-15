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

    /**
     * @brief Convert EncoderModeEventTypes to WheelMode
     * @param type The EncoderModeEventTypes to convert
     * @return Corresponding WheelMode value
     */
    static WheelMode toWheelMode(EventEnum::EncoderModeEventTypes type);
};
