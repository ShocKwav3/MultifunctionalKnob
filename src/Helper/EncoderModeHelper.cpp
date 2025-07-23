#include "EncoderModeHelper.h"

const char* EncoderModeHelper::toString(EventEnum::AppEventTypes type) {
    switch (type) {
        case EventEnum::AppEventTypes::MODE_SELECTION: return "MODE_SELECTION";
        case EventEnum::AppEventTypes::MODE_SELECTED: return "MODE_SELECTED";
        case EventEnum::AppEventTypes::MODE_SELECTION_CANCELLED: return "MODE_SELECTION_CANCELLED";
        case EventEnum::AppEventTypes::SCROLL: return "SCROLL";
        case EventEnum::AppEventTypes::VOLUME: return "VOLUME";
        default: return "UNKNOWN";
    }
}
