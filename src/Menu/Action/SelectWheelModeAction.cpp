#include "SelectWheelModeAction.h"
#include "Config/ConfigManager.h"
#include "EncoderMode/Manager/EncoderModeManager.h"
#include "Helper/EncoderModeHelper.h"
#include "Config/log_config.h"

SelectWheelModeAction::SelectWheelModeAction(WheelMode mode, ConfigManager* config, EncoderModeManager* modeMgr)
    : targetMode(mode), configManager(config), modeManager(modeMgr) {
}

void SelectWheelModeAction::execute() {
    // Persist mode to NVS
    Error saveResult = configManager->saveWheelMode(targetMode);
    if (saveResult != Error::OK) {
        LOG_ERROR("SelectWheelMode", "Failed to persist wheel mode");
    }

    // Apply mode immediately to runtime
    EventEnum::EncoderModeEventTypes encoderMode = EncoderModeHelper::fromWheelMode(targetMode);
    modeManager->setMode(encoderMode);

    // Log the mode change
    LOG_INFO("SelectWheelMode", "Wheel mode selected: %s", wheelModeToString(targetMode));
}

const char* SelectWheelModeAction::getConfirmationMessage() {
    switch (targetMode) {
        case WheelMode::SCROLL:
            return "Scroll Mode Active";
        case WheelMode::VOLUME:
            return "Volume Mode Active";
        case WheelMode::ZOOM:
            return "Zoom Mode Active";
        default:
            return "Mode Changed";
    }
}
