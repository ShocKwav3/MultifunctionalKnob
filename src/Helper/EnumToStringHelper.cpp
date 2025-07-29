#include "EnumToStringHelper.h"

const char* EnumToStringHelper::toString(EventEnum::EncoderInputEventTypes type) {
    switch (type) {
        case EventEnum::EncoderInputEventTypes::ROTATE: return "Encoder Input: Rotate";
        case EventEnum::EncoderInputEventTypes::SHORT_CLICK: return "Encoder Input: Short Click";
        case EventEnum::EncoderInputEventTypes::LONG_CLICK: return "Encoder Input: Long Click";
        default: return "Input: Unknown";
    }
}

const char* EnumToStringHelper::toString(EventEnum::EncoderModeAppEventTypes type) {
    switch (type) {
        case EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_SCROLL: return "Encoder Mode: Scroll";
        case EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_VOLUME: return "Encoder Mode: Volume";
        case EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_SELECTION: return "Encoder mode Action: Selection";
        case EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_SELECTED: return "Encoder mode Action: Selected";
        case EventEnum::EncoderModeAppEventTypes::ENCODER_MODE_SELECTION_CANCELLED: return "Encoder mode Action: Selection Cancelled";
        default: return "EncoderMode AppEvent: Unknown";
    }
}
