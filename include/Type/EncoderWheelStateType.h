#pragma once

#include "Enum/WheelModeEnum.h"
#include "Enum/WheelDirection.h"

/**
 * @brief Encoder wheel state tracking
 *
 * Contains all encoder wheel-related state:
 * - Current wheel mode (Scroll/Volume/Zoom)
 * - Wheel direction (Normal/Reversed)
 *
 * This consolidates encoder wheel state that was previously scattered
 * in HardwareState.
 */
struct EncoderWheelStateType {
    WheelMode mode;              ///< Current wheel mode (SCROLL, VOLUME, ZOOM)
    WheelDirection direction;    ///< Wheel direction (NORMAL, REVERSED)
};
