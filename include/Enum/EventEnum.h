#pragma once

namespace EventEnum {
    enum class EncoderInputEventTypes {
        ROTATE,
        SHORT_CLICK,
        LONG_CLICK
    };

    enum class AppEventTypes {
        SCROLL,
        VOLUME,
        __MODE_SELECTION_LIMIT, // acts as the upper bound so we don’t loop into meta events accidentally
        MODE_SELECTION,
        MODE_SELECTED,
        MODE_SELECTION_CANCELLED
    };
}
