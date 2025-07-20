#include "ModeHelper.h"

using namespace ModeEnum;

const char* ModeHelper::toString(AppEventTypes type) {
    switch (type) {
        case AppEventTypes::MODE_SELECTION: return "MODE_SELECTION";
        case AppEventTypes::MODE_SELECTED: return "MODE_SELECTED";
        case AppEventTypes::MODE_SELECTION_CANCELLED: return "MODE_SELECTION_CANCELLED";
        case AppEventTypes::SCROLL: return "SCROLL";
        case AppEventTypes::VOLUME: return "VOLUME";
        default: return "UNKNOWN";
    }
}
