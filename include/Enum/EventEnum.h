#pragma once

namespace EventEnum {
    enum class EncoderInputEventTypes {
        ROTATE,
        SHORT_CLICK,
        LONG_CLICK
    };

    enum class ButtonEventTypes {
        SHORT_PRESS,
        LONG_PRESS
    };

    enum class EncoderModeEventTypes {
        ENCODER_MODE_SCROLL,
        ENCODER_MODE_VOLUME,
        ENCODER_MODE_ZOOM,
        __ENCODER_MODE_SELECTION_LIMIT, // acts as the upper bound so we don't loop into meta events accidentally
        ENCODER_MODE_SELECTION,
        ENCODER_MODE_SELECTED,
        ENCODER_MODE_SELECTION_CANCELLED
    };
}
