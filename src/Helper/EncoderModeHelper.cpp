#include "EncoderModeHelper.h"

const char* EncoderModeHelper::toString(EventEnum::EncoderModeEventTypes type) {
    switch (type) {
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION: return "Encoder mode: Selection";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTED: return "Encoder mode: Selected";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SELECTION_CANCELLED: return "Encoder mode: Selection cancelled";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_SCROLL: return "Encoder mode: Scroll";
        case EventEnum::EncoderModeEventTypes::ENCODER_MODE_VOLUME: return "Encoder mode: Volume";
        default: return "UNKNOWN";
    }
}
