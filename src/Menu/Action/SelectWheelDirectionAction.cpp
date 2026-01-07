#include "SelectWheelDirectionAction.h"
#include "Config/ConfigManager.h"
#include "Config/log_config.h"

SelectWheelDirectionAction::SelectWheelDirectionAction(WheelDirection direction, ConfigManager* config)
    : targetDirection(direction), configManager(config) {
}

void SelectWheelDirectionAction::execute(const MenuItem* context) {
    // Context unused - direction is fixed at construction time
    // Persist direction to NVS
    Error saveResult = configManager->setWheelDirection(targetDirection);
    if (saveResult != Error::OK) {
        LOG_ERROR("SelectWheelDir", "Failed to persist wheel direction");
    } else {
        LOG_INFO("SelectWheelDir", "Wheel direction saved: %s", wheelDirectionToString(targetDirection));
    }
}

const char* SelectWheelDirectionAction::getConfirmationMessage() {
    return "Direction saved";
}
