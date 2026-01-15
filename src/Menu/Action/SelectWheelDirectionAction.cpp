#include "SelectWheelDirectionAction.h"
#include "Config/ConfigManager.h"
#include "Config/log_config.h"
#include "Display/Model/DisplayRequest.h"
#include "state/HardwareState.h"

extern HardwareState hardwareState;

SelectWheelDirectionAction::SelectWheelDirectionAction(WheelDirection direction, ConfigManager* config, QueueHandle_t displayQueue)
    : targetDirection(direction), configManager(config), displayRequestQueue(displayQueue) {
}

void SelectWheelDirectionAction::execute(const MenuItem* context) {
    // Context unused - direction is fixed at construction time
    // Persist direction to NVS
    Error saveResult = configManager->setWheelDirection(targetDirection);
    if (saveResult != Error::OK) {
        LOG_ERROR("SelectWheelDir", "Failed to persist wheel direction");
        return;  // Early return on error - don't update state or display
    }

    LOG_INFO("SelectWheelDir", "Wheel direction saved: %s", wheelDirectionToString(targetDirection));

    // Update global hardware state immediately (AC 4)
    hardwareState.encoderWheelState.direction = targetDirection;

    // Trigger display refresh to show new direction indicator
    if (displayRequestQueue != nullptr) {
        DisplayRequest normalModeReq;
        normalModeReq.type = DisplayRequestType::DRAW_NORMAL_MODE;
        normalModeReq.data.normalMode.state = hardwareState;
        xQueueSend(displayRequestQueue, &normalModeReq, pdMS_TO_TICKS(10));
    }
}

const char* SelectWheelDirectionAction::getConfirmationMessage() {
    return "Direction saved";
}
