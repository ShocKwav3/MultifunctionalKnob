#include "SetButtonBehaviorAction.h"
#include "Config/ConfigManager.h"
#include "Config/log_config.h"

SetButtonBehaviorAction::SetButtonBehaviorAction(uint8_t buttonIndex, ButtonAction action, ConfigManager* config)
    : buttonIndex(buttonIndex), action(action), configManager(config) {
}

void SetButtonBehaviorAction::execute() {
    // Persist action to NVS
    Error saveResult = configManager->saveButtonAction(buttonIndex, action);
    if (saveResult != Error::OK) {
        LOG_ERROR("SetButtonBehavior", "Failed to persist button action");
    }

    // Runtime mapping update handled by ButtonManager (if needed)
    // For now, just log the action assignment
    LOG_INFO("SetButtonBehavior", "Button %d set to: %s", buttonIndex, buttonActionToString(action));
}

const char* SetButtonBehaviorAction::getConfirmationMessage() {
    switch (action) {
        case ButtonAction::MUTE:
            return "Mute Assigned";
        case ButtonAction::PLAY:
            return "Play Assigned";
        case ButtonAction::PAUSE:
            return "Pause Assigned";
        case ButtonAction::NEXT:
            return "Next Assigned";
        case ButtonAction::PREVIOUS:
            return "Previous Assigned";
        case ButtonAction::NONE:
            return "Button Cleared";
        default:
            return "Action Assigned";
    }
}
