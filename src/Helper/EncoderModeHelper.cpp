#include "EncoderModeHelper.h"

const char* EncoderModeHelper::toString(EventEnum::EncoderModeEventTypes type) {
    switch (type) {
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION: return "Encoder mode: Selection";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTED: return "Encoder mode: Selected";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION_CANCELLED: return "Encoder mode: Selection cancelled";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL: return "Encoder mode: Scroll";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_VOLUME: return "Encoder mode: Volume";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_ZOOM: return "Encoder mode: Zoom";
        default: return "UNKNOWN";
    }
}

EventEnum::EncoderModeEventTypes EncoderModeHelper::fromWheelMode(WheelMode mode) {
    switch (mode) {
        case WheelMode::SCROLL:
            return EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL;
        case WheelMode::VOLUME:
            return EventEnum::EncoderModeEventTypes::ENCODER_MODE_VOLUME;
        case WheelMode::ZOOM:
            return EventEnum::EncoderModeEventTypes::ENCODER_MODE_ZOOM;
        default:
            return EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL;
    }
}

WheelMode EncoderModeHelper::toWheelMode(EventEnum::EncoderModeEventTypes type) {
    switch (type) {
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL:
            return WheelMode::SCROLL;
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_VOLUME:
            return WheelMode::VOLUME;
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_ZOOM:
            return WheelMode::ZOOM;
        default:
            return WheelMode::SCROLL;
    }
}
